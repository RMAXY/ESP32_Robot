#include <Arduino.h>
#include "tracking.h"
#include "../config/pins.h"

static bool isDetected(int pin) {
    return digitalRead(pin) == LOW;
}

void initTracking() {
    pinMode(TRACKING_LEFT, INPUT_PULLUP);
    pinMode(TRACKING_RIGHT, INPUT_PULLUP);
}

bool isLeftDetected() {
    return isDetected(TRACKING_LEFT);
}

bool isRightDetected() {
    return isDetected(TRACKING_RIGHT);
}