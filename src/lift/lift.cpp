#include <Arduino.h>
#include "../config/pins.h"
#include "lift.h"

static String currentLiftState = "stop";

String getLiftState() {
    return currentLiftState;
}

void initLift() {
    pinMode(LIFT_IN1, OUTPUT);
    pinMode(LIFT_IN2, OUTPUT);
    pinMode(LIMIT_TOP, INPUT_PULLUP);
    pinMode(LIMIT_BOTTOM, INPUT_PULLUP);
    stopLift();
}

void liftUp() {
    if (digitalRead(LIMIT_TOP) == LOW) {   // 触发上限位
        stopLift();
        return;
    }
    currentLiftState = "up";
    digitalWrite(LIFT_IN1, HIGH);
    digitalWrite(LIFT_IN2, LOW);
}

void liftDown() {
    if (digitalRead(LIMIT_BOTTOM) == LOW) { // 触发下限位
        stopLift();
        return;
    }
    currentLiftState = "down";
    digitalWrite(LIFT_IN1, LOW);
    digitalWrite(LIFT_IN2, HIGH);
}

void stopLift() {
    currentLiftState = "stop";
    digitalWrite(LIFT_IN1, LOW);
    digitalWrite(LIFT_IN2, LOW);
}

void updateLift() {
    if (currentLiftState == "up" && digitalRead(LIMIT_TOP) == LOW) {
        stopLift();
        return;
    }

    if (currentLiftState == "down" && digitalRead(LIMIT_BOTTOM) == LOW) {
        stopLift();
        return;
    }
}