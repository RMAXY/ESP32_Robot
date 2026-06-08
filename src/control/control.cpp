#include "controller.h"
#include "../motor/motor.h"
#include "../lift/lift.h"
#include "../ble/ble_server.h"
#include "../config/config.h"
#include "../sensor/ultrasonic.h"
#include "../sensor/tracking.h"

static RobotMode currentMode = MODE_REMOTE;
static const float OBSTACLE_DISTANCE_CM = 20.0f;
static int currentDriveSpeed = DEFAULT_SPEED;

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
    if (currentMode == MODE_TRACKING) {
        if (obstacleDetected()) {
            stopMotor();
            return;
        }

        bool leftDetected = isLeftDetected();
        bool rightDetected = isRightDetected();

        if (leftDetected && rightDetected) {
            moveForward(currentDriveSpeed);
        } else if (leftDetected && !rightDetected) {
            turnLeft(currentDriveSpeed);
        } else if (!leftDetected && rightDetected) {
            turnRight(currentDriveSpeed);
        } else {
            stopMotor();
        }
    }
}

void handleIncomingCommand() {
    if (hasNewCommand()) {
        String json = getBLECommand();
        ParsedCommand parsed = parseCommand(json);

        if (parsed.type == CMD_SET_SPEED) {
            currentDriveSpeed = clampDriveSpeed(parsed.speed);
            return;
        }

        executeCommand(parsed.type);
    }
}