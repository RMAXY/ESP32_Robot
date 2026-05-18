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
    CMD_LIFT_DOWN
};

CommandType parseCommand(const String &json);

#endif