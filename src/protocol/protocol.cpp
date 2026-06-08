#include <ArduinoJson.h>
#include "protocol.h"
#include "../config/config.h"

ParsedCommand parseCommand(const String &json) {
    ParsedCommand result = {CMD_NONE, DEFAULT_SPEED};
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        return result;
    }

    String cmd = doc["cmd"];
    if (cmd == "F")      { result.type = CMD_FORWARD; return result; }
    if (cmd == "B")      { result.type = CMD_BACKWARD; return result; }
    if (cmd == "L")      { result.type = CMD_LEFT; return result; }
    if (cmd == "R")      { result.type = CMD_RIGHT; return result; }
    if (cmd == "S")      { result.type = CMD_STOP; return result; }
    if (cmd == "UP")     { result.type = CMD_LIFT_UP; return result; }
    if (cmd == "DOWN")   { result.type = CMD_LIFT_DOWN; return result; }
    if (cmd == "REMOTE" || cmd == "MODE_REMOTE") { result.type = CMD_SET_REMOTE_MODE; return result; }
    if (cmd == "TRACK" || cmd == "TRACKING" || cmd == "MODE_TRACK") { result.type = CMD_SET_TRACKING_MODE; return result; }
    if (cmd == "SET_SPEED" || cmd == "SPEED") {
        result.type = CMD_SET_SPEED;
        if (doc["speed"].is<int>()) {
            result.speed = doc["speed"].as<int>();
        } else if (doc["value"].is<int>()) {
            result.speed = doc["value"].as<int>();
        } else if (doc["pwm"].is<int>()) {
            result.speed = doc["pwm"].as<int>();
        }
        return result;
    }
    return result;
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