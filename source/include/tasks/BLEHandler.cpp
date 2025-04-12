#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "esp_sleep.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);

void BLEHandler(void *parameter)
{
    // Enable light sleep
    esp_sleep_enable_timer_wakeup(1000); // Wake up after 1ms

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
                    // Shorter delay when connected since we need to be responsive
                    vTaskDelay(20 / portTICK_PERIOD_MS);
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
        // Much longer delay when not connected to save power
        vTaskDelay(500 / portTICK_PERIOD_MS);
        esp_light_sleep_start();
    }
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler, "BLE Handler", stackDepth, NULL, priority, &bleTaskHandle, core);
}