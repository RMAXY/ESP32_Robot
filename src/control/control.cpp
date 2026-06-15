#include <Arduino.h>
#include "protocol.h"
// ... 保持你原有的其他 include ...

// ==========================================
// 🚗 运动学时间参数宏定义 (必须加在文件顶部)
// ==========================================
#define ALIGN_DELAY         150  // 路口轴心对齐盲冲时间(ms)
#define BLIND_TURN_DELAY    220  // 转向初期传感器屏蔽时间(ms)
#define FORWARD_CROSS_DELAY 180  // 直行路口强行跨越时间(ms)

// ==========================================
// 📡 净化后的蓝牙指令接收与分流
// ==========================================
void handleIncomingCommand() {
    if (!hasNewCommand()) return;

    String json = getBLECommand();
    
    // 统一交由协议层解析
    ParsedCommand parsed = parseCommand(json);

    // 1. 路由路径指令分流
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

    // 3. 正常遥控/模式切换指令分发
    executeCommand(parsed.type);
}

// ==========================================
// 🚦 完美的网格转向与过路口状态机
// ==========================================
static bool isIntersectionDetected() {
    // 夹线跑状态下，双触黑线（双 false）代表到达路口
    return !isLeftDetected() && !isRightDetected();
}

static void executeIntersectionManeuver(char cmd) {
    switch (cmd) {
        case 'F':
            moveForward(currentDriveSpeed);
            delay(FORWARD_CROSS_DELAY); // 瞎冲过线，不需要 stopMotor
            break;

        case 'L':
            moveForward(currentDriveSpeed);
            delay(ALIGN_DELAY);         // 1. 轴心对齐
            turnLeft(currentDriveSpeed);
            delay(BLIND_TURN_DELAY);    // 2. 盲转脱离
            while (isLeftDetected()) {  // 3. 闭环捕获新线 (撞线时返回 false 退出)
                delay(5); 
            }
            break;

        case 'R':
            moveForward(currentDriveSpeed);
            delay(ALIGN_DELAY);         // 1. 轴心对齐
            turnRight(currentDriveSpeed);
            delay(BLIND_TURN_DELAY);    // 2. 盲转脱离
            while (isRightDetected()) { // 3. 闭环捕获新线
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

void updateRobotControl() {
    if (currentMode != MODE_TRACKING) return;
    if (obstacleDetected()) { stopMotor(); return; }

    // 1. 路口锁自锁与去抖
    if (isIntersectionDetected()) {
        if (!intersectionLocked) {
            intersectionLocked = true;
            handleIntersection(); 
        }
        return;
    }

    // 只有当两个传感器都安全回到白区时，才解开路口锁
    if (intersectionLocked && isLeftDetected() && isRightDetected()) {
        intersectionLocked = false;
    }

    // 2. 日常夹线循迹姿态微调 (true=白, false=黑)
    bool leftDetected = isLeftDetected();   
    bool rightDetected = isRightDetected();

    if (leftDetected && rightDetected) {
        moveForward(currentDriveSpeed); // 双白直行
    } 
    else if (!leftDetected && rightDetected) {
        turnLeft(currentDriveSpeed);    // 左黑：向左纠偏
    } 
    else if (leftDetected && !rightDetected) {
        turnRight(currentDriveSpeed);   // 右黑：向右纠偏
    } 
    else {
        moveForward(currentDriveSpeed); // 兜底
    }
}