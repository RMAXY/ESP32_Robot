#include <Arduino.h>
#include "../config/pins.h"
#include "ultrasonic.h"

void initUltrasonic() {
    pinMode(ULTRASONIC_TRIG, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);
    digitalWrite(ULTRASONIC_TRIG, LOW);
}

float getDistance() {
    const unsigned long timeoutUs = 30000UL;
    unsigned long pulseDuration;

    digitalWrite(ULTRASONIC_TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(ULTRASONIC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG, LOW);

    pulseDuration = pulseIn(ULTRASONIC_ECHO, HIGH, timeoutUs);
    if (pulseDuration == 0) {
        return 0.0;
    }

    // 距离：声波往返时间 / 2, 速度 0.0343 cm/us
    return (pulseDuration * 0.0343f) / 2.0f;
}