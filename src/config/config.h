#ifndef CONFIG_H
#define CONFIG_H

// 机器人尺寸参数（毫米）
#define WHEEL_DIAMETER     65.0
#define ROBOT_TRACK_WIDTH  150.0

// 默认运动速度 (0-255)
#define DEFAULT_SPEED      60

// TB6612 驱动电压与 TT 减速电机额定电压
#define MOTOR_SUPPLY_VOLTAGE 12.1f
#define TT_MOTOR_MAX_VOLTAGE 6.0f
#define MAX_MOTOR_PWM        (int)(255.0f * TT_MOTOR_MAX_VOLTAGE / MOTOR_SUPPLY_VOLTAGE)

// BLE 设备名称
#define BLE_DEVICE_NAME    "ESP32_Robot"

#endif