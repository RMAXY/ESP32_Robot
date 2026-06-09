#include "ble_server.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// BLE 服务与特性 UUID (通用ID，用于手机识别)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define NOTIFY_CHARACTERISTIC_UUID "c1bc4f23-6d4f-48b9-9f26-4f9ae1d4a001"

// 收件箱：用于存放从手机收到的字符串命令
static String receivedData = "";

// 标志位：告诉主程序“有新命令到达”
static bool newDataFlag = false;

static BLEServer *gServer = nullptr;
static BLECharacteristic *gNotifyCharacteristic = nullptr;
static bool gConnected = false;

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

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        gConnected = true;
        Serial.println("BLE client connected.");
    }

    void onDisconnect(BLEServer *pServer) {
        gConnected = false;
        Serial.println("BLE client disconnected.");
        pServer->getAdvertising()->start();
    }
};

void initBLE() {
    // 1. 初始化BLE设备，这里的名称 "ESP32_Robot" 会出现在手机搜索列表里
    BLEDevice::init("ESP32_Robot");
    BLEDevice::setMTU(512); // 将最大传输单元扩展到 512 字节
    
    // 2. 创建BLE服务器
    gServer = BLEDevice::createServer();
    gServer->setCallbacks(new ServerCallbacks());
    
    // 3. 创建服务
    BLEService *pService = gServer->createService(SERVICE_UUID);
    
    // 4. 创建写入特性（这是手机端进行读写的对象）
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE |  // 允许手机向ESP32写数据
        BLECharacteristic::PROPERTY_READ     // 允许手机读取ESP32的数据
    );

    gNotifyCharacteristic = pService->createCharacteristic(
        NOTIFY_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    gNotifyCharacteristic->addDescriptor(new BLE2902());
    
    // 5. 绑定回调（当手机向这个特性写数据时，会触发 MyCallbacks 里的 onWrite 函数）
    pCharacteristic->setCallbacks(new MyCallbacks());
    
    // 6. 启动服务
    pService->start();
    
    // 7. 开始广播（让手机能搜到）
    BLEAdvertising *pAdvertising = gServer->getAdvertising();
    pAdvertising->start();
    
    Serial.println("BLE server is ready. Waiting for connection...");
}

void updateBLE() {
    // 非阻塞处理 BLE 事件（通常不需要额外操作）
}

void sendStatus(const String &json) {
    if (gNotifyCharacteristic == nullptr || !gConnected) {
        return;
    }

    gNotifyCharacteristic->setValue(json.c_str());
    gNotifyCharacteristic->notify();
}

bool isBLEConnected() {
    return gConnected;
}

bool hasNewCommand() {
    return newDataFlag;
}

String getBLECommand() {
    newDataFlag = false;  // 读取后立刻清零标志位
    return receivedData;
}