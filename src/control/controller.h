#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../protocol/protocol.h"

typedef enum {
    MODE_REMOTE,
    MODE_TRACKING
} RobotMode;

void executeCommand(CommandType cmd);
void updateRobotControl();
void handleIncomingCommand();   // 一次处理一条蓝牙命令

#endif