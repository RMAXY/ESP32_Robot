/*
// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y + 1;
}
*/
#include <Arduino.h>
#include "config/pins.h"
#include "config/config.h"
#include "motor/motor.h"
#include "lift/lift.h"
#include "sensor/ultrasonic.h"
#include "sensor/tracking.h"
#include "ble/ble_server.h"
#include "control/controller.h"

void setup() {
    Serial.begin(115200);

    initMotor();
    initLift();
    initUltrasonic();
    initTracking();
    initBLE();

    Serial.println("Robot ready.");
}

void loop() {
    updateBLE();
    updateLift();

    // 核心：收到蓝牙命令 → 解析 → 执行
    handleIncomingCommand();

    // 之后可在这里加入自动避障、循迹等
}