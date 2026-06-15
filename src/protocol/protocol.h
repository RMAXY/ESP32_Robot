#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

// 1. 新增了 CMD_SET_ROUTE 枚举
enum CommandType {
    CMD_NONE,
    CMD_FORWARD,
    CMD_BACKWARD,
    CMD_LEFT,
    CMD_RIGHT,
    CMD_STOP,
    CMD_LIFT_UP,
    CMD_LIFT_DOWN,
    CMD_SET_REMOTE_MODE,
    CMD_SET_TRACKING_MODE,
    CMD_SET_SPEED,
    CMD_SET_ROUTE  // 👈 新增：全局网格路由指令
};

// 2. 结构体里增加了 String route 用来承载路径
struct ParsedCommand {
    CommandType type;
    int speed;
    String route;  // 👈 新增：专门存放 "LFRLF" 这样的路径字符串
};

ParsedCommand parseCommand(const String &json);
String buildStatusJson(float speed, bool running, float distance, const String &liftState, bool limitTop, bool limitBottom, bool connected);

#endif