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

    // 持续执行当前模式的控制逻辑：
    // 1) 遥控模式：动作由蓝牙命令驱动
    // 2) 循迹模式：传感器持续驱动移动
    // 3) 无论哪种模式，前方避障始终启用
    updateRobotControl();
}