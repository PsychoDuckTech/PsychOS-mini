#include <Arduino.h>
#include <ArduinoBLE.h>
#include "tasks/BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

BLECharacteristic remotePsychoCharacteristic;

void BLEHandler(void *parameter)
{
    initializeBLE();
    BLE.setLocalName("Tenki one");

    BLEDevice peripheral;
    bool scanning_message_printed = false;

    for (;;)
    {
        if (!moduleConnectionStatus)
        {
            if (!scanning_message_printed)
            {
                Serial.println("Scanning for Kibodo One...");
                scanning_message_printed = true;
            }
            BLE.scanForName("Kibodo One");
            peripheral = BLE.available();

            if (peripheral)
            {
                BLE.stopScan();
                if (peripheral.connect() && peripheral.discoverAttributes())
                {
                    remotePsychoCharacteristic = peripheral.characteristic(CHARACTERISTIC_UUID);
                    if (remotePsychoCharacteristic)
                    {
                        moduleConnectionStatus = true;
                        Serial.print("Connected to device: ");
                        Serial.println(peripheral.localName());
                        Serial.print("Address: ");
                        Serial.println(peripheral.address());
                    }
                }
            }
        }
        else if (!peripheral.connected())
        {
            moduleConnectionStatus = false;
            scanning_message_printed = false;
            Serial.println("Disconnected from Kibodo One");
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

void sendKeyEvent(uint8_t keyCode, bool isPressed)
{
    if (moduleConnectionStatus && keyCode != 0)
    {
        uint8_t data[2] = {
            keyCode,
            static_cast<uint8_t>(isPressed ? 1 : 0)};
        remotePsychoCharacteristic.writeValue(data, 2);
    }
}