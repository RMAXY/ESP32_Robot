#include "controller.h"
#include "../motor/motor.h"
#include "../lift/lift.h"
#include "../ble/ble_server.h"

void executeCommand(CommandType cmd) {
    switch (cmd) {
        case CMD_FORWARD:
            moveForward(150);
            break;
        case CMD_BACKWARD:
            moveBackward(150);
            break;
        case CMD_LEFT:
            turnLeft(150);
            break;
        case CMD_RIGHT:
            turnRight(150);
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

void handleIncomingCommand() {
    if (hasNewCommand()) {
        String json = getBLECommand();
        CommandType cmd = parseCommand(json);
        executeCommand(cmd);
    }
}