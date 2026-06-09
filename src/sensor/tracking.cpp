#include <Arduino.h>
#include "tracking.h"
#include "../config/pins.h"

static bool isDetected(int pin) {
    return digitalRead(pin) == LOW;
}

void initTracking() {
    pinMode(TRACKING_LEFT, INPUT_PULLUP);
    pinMode(TRACKING_RIGHT, INPUT_PULLUP);
    Serial.println("Tracking sensor initialized.");
}

bool isLeftDetected() {
    static bool lastDetected = false;
    static unsigned long lastDebugMs = 0;
    bool detected = isDetected(TRACKING_LEFT);

    if (detected != lastDetected || (millis() - lastDebugMs) >= 500UL) {
        Serial.print("Tracking left: ");
        Serial.println(detected ? "detected" : "clear");
        lastDetected = detected;
        lastDebugMs = millis();
    }

    return detected;
}

bool isRightDetected() {
    static bool lastDetected = false;
    static unsigned long lastDebugMs = 0;
    bool detected = isDetected(TRACKING_RIGHT);

    if (detected != lastDetected || (millis() - lastDebugMs) >= 500UL) {
        Serial.print("Tracking right: ");
        Serial.println(detected ? "detected" : "clear");
        lastDetected = detected;
        lastDebugMs = millis();
    }

    return detected;
}