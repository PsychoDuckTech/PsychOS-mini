#pragma once

#include <Arduino.h>
#include <ArduinoBLE.h>
#include "globals.h"
#include "utils/initializeBLE.h"
#include "tasks/matrixScan.h"

// Function declarations
void BLEHandler(void *parameter);
void handleMasterBLE();
void handleSlaveBLE();
void handleReceivedKeypress(const uint8_t *data, int length, bool fromMatrix = false);
void sendKeyEvent(uint8_t keyCode, bool isPressed);
void startBleTask(UBaseType_t core = 1, uint32_t stackDepth = 16384, UBaseType_t priority = 1);

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

// BLE Service and Characteristic
extern BLEService psychoService;
extern BLECharacteristic psychoCharacteristic;