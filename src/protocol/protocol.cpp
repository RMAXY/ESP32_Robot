#include <ArduinoJson.h>
#include "protocol.h"

CommandType parseCommand(const String &json) {
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        return CMD_NONE;
    }

    String cmd = doc["cmd"];
    if (cmd == "F")      return CMD_FORWARD;
    if (cmd == "B")      return CMD_BACKWARD;
    if (cmd == "L")      return CMD_LEFT;
    if (cmd == "R")      return CMD_RIGHT;
    if (cmd == "S")      return CMD_STOP;
    if (cmd == "UP")     return CMD_LIFT_UP;
    if (cmd == "DOWN")   return CMD_LIFT_DOWN;

    return CMD_NONE;
}