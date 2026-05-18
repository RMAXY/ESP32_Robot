#include <Arduino.h>
#include "../config/pins.h"
#include "ultrasonic.h"

void initUltrasonic() {
    pinMode(ULTRASONIC_TRIG, OUTPUT);
    pinMode(ULTRASONIC_ECHO, INPUT);
}

float getDistance() {
    // TODO: 实现超声波测距
    return 0.0;
}