#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../protocol/protocol.h"

void executeCommand(CommandType cmd);
void handleIncomingCommand();   // 一次处理一条蓝牙命令

#endif