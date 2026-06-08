#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>

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
    CMD_SET_TRACKING_MODE
};

CommandType parseCommand(const String &json);
String buildStatusJson(float speed, bool running, float distance, const String &liftState, bool limitTop, bool limitBottom, bool connected);

#endif