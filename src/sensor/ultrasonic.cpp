#include <Arduino.h>
#include "../config/pins.h"
#include "ultrasonic.h"

void initUltrasonic() {
    pinMode(ULTRASONIC_TRIG, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);
    digitalWrite(ULTRASONIC_TRIG, LOW);
    Serial.println("Ultrasonic sensor initialized.");
}

float getDistance() {
    const unsigned long timeoutUs = 30000UL;
    unsigned long pulseDuration;
    static float lastDistance = -1.0f;
    static bool lastEchoValid = true;
    static unsigned long lastDebugMs = 0;

    digitalWrite(ULTRASONIC_TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(ULTRASONIC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG, LOW);

    pulseDuration = pulseIn(ULTRASONIC_ECHO, HIGH, timeoutUs);
    if (pulseDuration == 0) {
        if (lastEchoValid) {
            Serial.println("Ultrasonic no echo received.");
            lastEchoValid = false;
        }
        lastDistance = 0.0f;
        return 0.0;
    }

    float distance = (pulseDuration * 0.0343f) / 2.0f;
    lastEchoValid = true;

    if (abs(distance - lastDistance) >= 1.0f || millis() - lastDebugMs >= 1000UL || lastDistance < 0.0f) {
        Serial.print("Ultrasonic distance: ");
        Serial.print(distance);
        Serial.println(" cm");
        lastDistance = distance;
        lastDebugMs = millis();
    }

    // 距离：声波往返时间 / 2, 速度 0.0343 cm/us
    return distance;
}