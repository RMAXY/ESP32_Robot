#include "controller.h"
#include <ArduinoJson.h>
#include "../motor/motor.h"
#include "../lift/lift.h"
#include "../ble/ble_server.h"
#include "../config/config.h"
#include "../sensor/ultrasonic.h"
#include "../sensor/tracking.h"

#define ALIGN_DELAY         150  
// 1. 轴心对齐盲冲时间
// 含义：当传感器刚踩到路口黑线时，轮轴中心其实还在路口后方。
//       需要让小车继续向前盲冲这段时间，把两轮连线的中心死死钉在十字路口的几何中心上。
// 调测建议：如果小车转弯时总是不在正中心，或者转过去之后偏离赛道，就微调这个值。
#define BLIND_TURN_DELAY    220
// 2. 转向初期传感器屏蔽时间
// 含义：原地旋转刚启动时，传感器可能还没彻底甩开当前的横向黑线。
//       在这段盲转时间内，系统不读取传感器状态，强行旋转，防止原地“鬼畜”或误触发退出。
// 调测建议：只要能让车头转过大约 20°~30° 即可，不宜设得太大，否则会错过对向目标线的捕获。
#define FORWARD_CROSS_DELAY 180 
// 3. 直行路口跨越时间
// 含义：收到指令 'F'（直行）时，小车需要盲冲这段时间，确保传感器彻底越过十字路口的横向黑线，进入下一格。
// 调测建议：必须确保冲过去之后，传感器完全进入下一格的白区。

static RobotMode currentMode = MODE_REMOTE;
static const float OBSTACLE_DISTANCE_CM = 20.0f;
static int currentDriveSpeed = DEFAULT_SPEED;
static String currentRoute = "";
static bool intersectionLocked = false;

static int clampDriveSpeed(int speed) {
    if (speed < 0) return 0;
    if (speed > MAX_MOTOR_PWM) return MAX_MOTOR_PWM;
    return speed;
}

static bool obstacleDetected() {
    float distance = getDistance();
    return (distance > 0.0f && distance < OBSTACLE_DISTANCE_CM);
}

static bool tryMoveForward() {
    if (obstacleDetected()) {
        stopMotor();
        return false;
    }

    moveForward(currentDriveSpeed);
    return true;
}

static bool isIntersectionDetected() {
    return isLeftDetected() && isRightDetected();
}

static void notifyRouteProgress(const String &state, char cmd, const String &remainingRoute) {
    if (!isBLEConnected()) {
        return;
    }

    String payload = "{\"type\":\"route_progress\",\"state\":\"";
    payload += state;
    payload += "\",\"cmd\":\"";
    payload += cmd;
    payload += "\",\"remaining\":\"";
    payload += remainingRoute;
    payload += "\",\"queue_len\":";
    payload += String(currentRoute.length());
    payload += "}";

    sendStatus(payload);
}

static void executeIntersectionManeuver(char cmd) {
    switch (cmd) {
        case 'F':
            moveForward(currentDriveSpeed);
            delay(FORWARD_CROSS_DELAY); 
            // 退出后直接交还给 updateRobotControl，此时传感器已在白区，逻辑会自动继续直行
            break;

        case 'L':
            // 1. 轴心对齐路口中心
            moveForward(currentDriveSpeed);
            delay(ALIGN_DELAY);
            // 2. 启动盲转，逃离当前路口黑线干扰
            turnLeft(currentDriveSpeed);
            delay(BLIND_TURN_DELAY);
            // 3. 闭环捕获：持续左转，直到左传感器率先撞击前方全新的纵向黑线
            // (isLeftDetected() == true 代表还在白区，变为 false 代表撞击黑线)
            while (isLeftDetected()) {
                delay(5); // 喂狗，释放 CPU
            }
            // 4. 撞线瞬间直接退出！不调用 stopMotor，让日常循迹的“左偏左拉”自动拉正最后的 10 度
            break;

        case 'R':
            // 右转同理（镜像对称）
            moveForward(currentDriveSpeed);
            delay(ALIGN_DELAY);
            turnRight(currentDriveSpeed);
            delay(BLIND_TURN_DELAY);
            // 右转时，右传感器会率先撞击目标黑线
            while (isRightDetected()) {
                delay(5);
            }
            break;
            
        default:
            stopMotor();
            break;
    }
}

static void handleIntersection() {
    if (currentRoute.length() == 0) {
        stopMotor();
        notifyRouteProgress("waiting", '-', "");
        return;
    }

    char cmd = currentRoute.charAt(0);
    currentRoute.remove(0, 1);

    executeIntersectionManeuver(cmd);
    notifyRouteProgress("executed", cmd, currentRoute);
}

void executeCommand(CommandType cmd) {
    switch (cmd) {
        case CMD_SET_REMOTE_MODE:
            currentMode = MODE_REMOTE;
            stopMotor();
            break;
        case CMD_SET_TRACKING_MODE:
            currentMode = MODE_TRACKING;
            stopMotor();
            break;
        case CMD_FORWARD:
            if (currentMode == MODE_REMOTE) {
                tryMoveForward();
            }
            break;
        case CMD_BACKWARD:
            if (currentMode == MODE_REMOTE) {
                moveBackward(currentDriveSpeed);
            }
            break;
        case CMD_LEFT:
            if (currentMode == MODE_REMOTE) {
                turnLeft(currentDriveSpeed);
            }
            break;
        case CMD_RIGHT:
            if (currentMode == MODE_REMOTE) {
                turnRight(currentDriveSpeed);
            }
            break;
        case CMD_STOP:
            stopMotor();
            stopLift();
            break;
        case CMD_LIFT_UP:
            liftUp();
            break;
        case CMD_LIFT_DOWN:
            liftDown();
            break;
        default:
            break;
    }
}

void updateRobotControl() {
    if (currentMode != MODE_TRACKING) {
        return;
    }

    if (obstacleDetected()) {
        stopMotor();
        return;
    }

    //路口锁状态机
    if (isIntersectionDetected()) {
        if (!intersectionLocked) {
            intersectionLocked = true;
            handleIntersection();
        }
        return;
    }

    if (intersectionLocked && isLeftDetected() && isRightDetected()) {
        intersectionLocked = false;
    }

    bool leftDetected = isLeftDetected();
    bool rightDetected = isRightDetected();

    if (leftDetected && rightDetected) {
        moveForward(currentDriveSpeed);
    } else if (!leftDetected && rightDetected) {
        turnLeft(currentDriveSpeed);
    } else if (leftDetected && !rightDetected) {
        turnRight(currentDriveSpeed);
    } else {
        moveForward(currentDriveSpeed);
    }
}

void handleIncomingCommand() {
    if (!hasNewCommand()) {
        return;
    }

    String json = getBLECommand();

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (!error && doc.containsKey("Route")) {
        currentRoute = doc["Route"].as<String>();
        intersectionLocked = false;
        notifyRouteProgress("queued", '-', currentRoute);
        return;
    }

    ParsedCommand parsed = parseCommand(json);

    if (parsed.type == CMD_SET_SPEED) {
        currentDriveSpeed = clampDriveSpeed(parsed.speed);
        return;
    }

    executeCommand(parsed.type);
}