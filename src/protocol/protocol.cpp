#include <ArduinoJson.h>
#include "protocol.h"

CommandType parseCommand(const String &json) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        return CMD_NONE;
    }

    String cmd = doc["cmd"];
    if (cmd == "F")      return CMD_FORWARD;//前进
    if (cmd == "B")      return CMD_BACKWARD;//后退
    if (cmd == "L")      return CMD_LEFT;//左转
    if (cmd == "R")      return CMD_RIGHT;//右转
    if (cmd == "S")      return CMD_STOP;//停车
    if (cmd == "UP")     return CMD_LIFT_UP;//抬起
    if (cmd == "DOWN")   return CMD_LIFT_DOWN;//放下
    if (cmd == "REMOTE" || cmd == "MODE_REMOTE") return CMD_SET_REMOTE_MODE;//遥控模式
    if (cmd == "TRACK" || cmd == "TRACKING" || cmd == "MODE_TRACK") return CMD_SET_TRACKING_MODE;
    //循迹模式
    return CMD_NONE;
}

String buildStatusJson(float speed, bool running, float distance, const String &liftState, bool limitTop, bool limitBottom, bool connected) {
    JsonDocument doc;
    doc["type"] = "status";
    doc["speed"] = speed;
    doc["running"] = running;
    doc["distance"] = distance;
    doc["lift"] = liftState;
    doc["limit_top"] = limitTop;
    doc["limit_bottom"] = limitBottom;
    doc["connected"] = connected;

    String json;
    serializeJson(doc, json);
    return json;
}