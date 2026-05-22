#include "ble_server.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// BLE 服务与特性 UUID (通用ID，用于手机识别)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// 收件箱：用于存放从手机收到的字符串命令
static String receivedData = "";

// 标志位：告诉主程序“有新命令到达”
static bool newDataFlag = false;

// 接收数据回调
class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            // 1. 存入全局变量（这里必须转换成 String，因为整个工程基于 String）
            receivedData = String(value.c_str());
            // 2. 设置标志位，通知 loop() 有新数据
            newDataFlag = true;
        }
    }
};

void initBLE() {
    // 1. 初始化BLE设备，这里的名称 "ESP32_Robot" 会出现在手机搜索列表里
    BLEDevice::init("ESP32_Robot");
    
    // 2. 创建BLE服务器
    BLEServer *pServer = BLEDevice::createServer();
    
    // 3. 创建服务
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // 4. 创建特性（这是手机端进行读写的对象）
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE |  // 允许手机向ESP32写数据
        BLECharacteristic::PROPERTY_READ     // 允许手机读取ESP32的数据
    );
    
    // 5. 绑定回调（当手机向这个特性写数据时，会触发 MyCallbacks 里的 onWrite 函数）
    pCharacteristic->setCallbacks(new MyCallbacks());
    
    // 6. 启动服务
    pService->start();
    
    // 7. 开始广播（让手机能搜到）
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    
    Serial.println("BLE server is ready. Waiting for connection...");
}

void updateBLE() {
    // 非阻塞处理 BLE 事件（通常不需要额外操作）
}

bool hasNewCommand() {
    return newDataFlag;
}

String getBLECommand() {
    newDataFlag = false;  // 读取后立刻清零标志位
    return receivedData;
}