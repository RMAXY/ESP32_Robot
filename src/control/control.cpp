#include "controller.h"
#include <ArduinoJson.h>
#include "../motor/motor.h"
#include "../lift/lift.h"
#include "../ble/ble_server.h"
#include "../config/config.h"
#include "../sensor/ultrasonic.h"
#include "../sensor/tracking.h"
#include "../protocol/protocol.h" // 👈 确保引入了协议头文件

// ==========================================
// 🚗 网格 AGV 运动学时间参数定义 (单位: 毫秒)
// ==========================================
#define ALIGN_DELAY         150  // 1. 轴心对齐盲冲时间
#define BLIND_TURN_DELAY    220  // 2. 转向初期传感器屏蔽时间
#define FORWARD_CROSS_DELAY 180  // 3. 直行路口跨越时间

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

// 【🎯 已修正 Bug】：夹线跑模式下，双触黑线（双 false）才代表到达路口
static bool isIntersectionDetected() {
    return !isLeftDetected() && !isRightDetected();
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
            // 退出后直接交还给 updateRobotControl，传感器已过线，自动继续直行
            break;

        case 'L':
            // 1. 轴心对齐路口中心
            moveForward(currentDriveSpeed);
            delay(ALIGN_DELAY);
            // 2. 启动盲转，逃离当前路口黑线干扰
            turnLeft(currentDriveSpeed);
            delay(BLIND_TURN_DELAY);
            // 3. 闭环捕获：持续左转，直到左传感器率先撞击前方全新的纵向黑线
            while (isLeftDetected()) {
                delay(5); // 喂狗，释放 CPU
            }
            // 4. 撞线瞬间直接退出，交由日常循迹拉正最后 10 度
            break;

        case 'R':
            // 右转镜像对称
            moveForward(currentDriveSpeed);
            delay(ALIGN_DELAY);
            turnRight(currentDriveSpeed);
            delay(BLIND_TURN_DELAY);
            // 右转时，右传感器率先撞击目标黑线
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
            if (currentMode == MODE_REMOTE) { tryMoveForward(); }
            break;
        case CMD_BACKWARD:
            if (currentMode == MODE_REMOTE) { moveBackward(currentDriveSpeed); }
            break;
        case CMD_LEFT:
            if (currentMode == MODE_REMOTE) { turnLeft(currentDriveSpeed); }
            break;
        case CMD_RIGHT:
            if (currentMode == MODE_REMOTE) { turnRight(currentDriveSpeed); }
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

    // 1. 路口锁状态机
    if (isIntersectionDetected()) {
        if (!intersectionLocked) {
            intersectionLocked = true;
            handleIntersection();
        }
        return;
    }

    // 只有当两个传感器都安全回到白区时，才释放路口锁，防止重入
    if (intersectionLocked && isLeftDetected() && isRightDetected()) {
        intersectionLocked = false;
    }

    // 2. 日常夹线循迹姿态微调 (true=白区, false=黑线)
    bool leftDetected = isLeftDetected();
    bool rightDetected = isRightDetected();

    if (leftDetected && rightDetected) {
        moveForward(currentDriveSpeed);
    } else if (!leftDetected && rightDetected) {
        turnLeft(currentDriveSpeed);    // 左触黑：向左修正
    } else if (leftDetected && !rightDetected) {
        turnRight(currentDriveSpeed);   // 右触黑：向右修正
    } else {
        moveForward(currentDriveSpeed); // 双黑兜底直行
    }
}

// 【🎯 已重构】：彻底移除内部多余的 JSON 解析，与全功能 Protocol 完美对齐
void handleIncomingCommand() {
    if (!hasNewCommand()) {
        return;
    }

    String json = getBLECommand();
    ParsedCommand parsed = parseCommand(json);

    // 1. 路由路径指令统一分流
    if (parsed.type == CMD_SET_ROUTE) {
        currentRoute = parsed.route;
        intersectionLocked = false;
        notifyRouteProgress("queued", '-', currentRoute);
        return;
    }

    // 2. 调速指令分流
    if (parsed.type == CMD_SET_SPEED) {
        currentDriveSpeed = clampDriveSpeed(parsed.speed);
        return;
    }

    // 3. 基础控制指令分发
    executeCommand(parsed.type);
}