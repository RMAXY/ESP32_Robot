#include <Arduino.h>
#include "../config/pins.h"
#include "motor.h"

// LEDC 通道配置
#define LEDC_FREQ       5000      // 频率 5kHz
#define LEDC_RESOLUTION 8         // 分辨率 8 位（0-255）

static const int leftChannel  = 0;   // 左电机 PWM 通道
static const int rightChannel = 1;   // 右电机 PWM 通道
static int currentSpeed = 0;

void initMotor() {
    // 方向引脚
    pinMode(MOTOR_LEFT_IN1, OUTPUT);
    pinMode(MOTOR_LEFT_IN2, OUTPUT);
    pinMode(MOTOR_RIGHT_IN1, OUTPUT);
    pinMode(MOTOR_RIGHT_IN2, OUTPUT);

    // PWM 使能引脚初始化
    ledcSetup(leftChannel, LEDC_FREQ, LEDC_RESOLUTION);
    ledcAttachPin(MOTOR_LEFT_EN, leftChannel);

    ledcSetup(rightChannel, LEDC_FREQ, LEDC_RESOLUTION);
    ledcAttachPin(MOTOR_RIGHT_EN, rightChannel);

    // 初始停止
    stopMotor();
}

void moveForward(int speed) {
    currentSpeed = speed;
    digitalWrite(MOTOR_LEFT_IN1, HIGH);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
    digitalWrite(MOTOR_RIGHT_IN1, HIGH);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);

    ledcWrite(leftChannel, speed);
    ledcWrite(rightChannel, speed);
}

void moveBackward(int speed) {
    currentSpeed = speed;
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, HIGH);
    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, HIGH);

    ledcWrite(leftChannel, speed);
    ledcWrite(rightChannel, speed);
}

void turnLeft(int speed) {
    currentSpeed = speed;
    // 左转：左轮后退，右轮前进
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, HIGH);
    digitalWrite(MOTOR_RIGHT_IN1, HIGH);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);

    ledcWrite(leftChannel, speed);
    ledcWrite(rightChannel, speed);
}

void turnRight(int speed) {
    currentSpeed = speed;
    // 右转：左轮前进，右轮后退
    digitalWrite(MOTOR_LEFT_IN1, HIGH);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, HIGH);

    ledcWrite(leftChannel, speed);
    ledcWrite(rightChannel, speed);
}

void stopMotor() {
    currentSpeed = 0;
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);

    ledcWrite(leftChannel, 0);
    ledcWrite(rightChannel, 0);
}

int getMotorSpeed() {
    return currentSpeed;
}

bool isMotorRunning() {
    return currentSpeed > 0;
}