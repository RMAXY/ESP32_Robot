#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <Arduino.h>

void initBLE();
void updateBLE();
void sendStatus(const String &json);
bool isBLEConnected();
bool hasNewCommand();
String getBLECommand();

#endif