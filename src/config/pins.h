#ifndef PINS_H
#define PINS_H

// ========== 电机 ==========
#define MOTOR_LEFT_IN1  19
#define MOTOR_LEFT_IN2  18
#define MOTOR_RIGHT_IN1 21
#define MOTOR_RIGHT_IN2 22

// 电机使能（PWM调速）
#define MOTOR_LEFT_EN   25
#define MOTOR_RIGHT_EN  26

// ========== 吊装 ==========
#define LIFT_IN1        23
#define LIFT_IN2        5
#define LIMIT_TOP       32   // 上限位开关
#define LIMIT_BOTTOM    33   // 下限位开关

// ========== 超声波 ==========
#define ULTRASONIC_TRIG 13
#define ULTRASONIC_ECHO 14

// ========== 循迹（预留）==========
#define TRACKING_LEFT   34
#define TRACKING_RIGHT  35

#endif