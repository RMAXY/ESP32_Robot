#include <Arduino.h>
#include "../config/pins.h"
#include "lift.h"

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
    digitalWrite(LIFT_IN1, HIGH);
    digitalWrite(LIFT_IN2, LOW);
}

void liftDown() {
    if (digitalRead(LIMIT_BOTTOM) == LOW) { // 触发下限位
        stopLift();
        return;
    }
    digitalWrite(LIFT_IN1, LOW);
    digitalWrite(LIFT_IN2, HIGH);
}

void stopLift() {
    digitalWrite(LIFT_IN1, LOW);
    digitalWrite(LIFT_IN2, LOW);
}

void updateLift() {
    // 可在此增加额外的安全检查，暂时留空
}