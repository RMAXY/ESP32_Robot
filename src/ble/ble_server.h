#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <Arduino.h>

void initBLE();
void updateBLE();
bool hasNewCommand();
String getBLECommand();

#endif