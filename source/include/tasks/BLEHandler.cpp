#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);

void BLEHandler(void *parameter)
{
    initializeBLE();
    bool wasConnected = false;

    BLE.setLocalName("Tenki one");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();

    for (;;)
    {
        BLEDevice central = BLE.central();
        if (central)
        {
            if (central.connected())
            {
                Serial1.println("Connected to main keyboard");
                moduleConnectionStatus = true;
                wasConnected = true;
                BLE.stopAdvertise();

                while (central.connected())
                {
                    BLE.poll();
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
            }

            if (wasConnected)
            {
                Serial1.println("Main keyboard disconnected");
                moduleConnectionStatus = false;
                wasConnected = false;
                BLE.advertise();
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler, "BLE Handler", stackDepth, NULL, priority, &bleTaskHandle, core);
}