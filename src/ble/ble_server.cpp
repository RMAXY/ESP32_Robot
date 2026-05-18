#include "ble_server.h"

// 简易收件箱
static String currentCommand = "";
static bool newCommandFlag = false;

void initBLE() {
    // TODO: 参考 ESP32 BLE Arduino 示例 BLE_write
    // 1. 创建 BLE 服务器
    // 2. 创建特征值，并在 onWrite 回调中设置 currentCommand 和 newCommandFlag
}

void updateBLE() {
    // 非阻塞处理 BLE 事件（通常不需要额外操作）
}

bool hasNewCommand() {
    return newCommandFlag;
}

String getBLECommand() {
    newCommandFlag = false;
    return currentCommand;
}