#ifndef LIFT_H
#define LIFT_H

#include <Arduino.h>

String getLiftState();

void initLift();
void liftUp();
void liftDown();
void stopLift();
void updateLift();   // 周期调用，处理限位等

#endif