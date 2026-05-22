#include <Arduino.h>
#include "tracking.h"
#include "../config/pins.h"

void initTracking() {
    pinMode(TRACKING_LEFT, INPUT);
    pinMode(TRACKING_RIGHT, INPUT);
}

bool isLeftDetected() {
    // TODO: 读取左循迹传感器
    return false;
}

bool isRightDetected() {
    // TODO: 读取右循迹传感器
    return false;
}