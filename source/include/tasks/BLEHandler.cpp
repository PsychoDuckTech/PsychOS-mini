#include "BLEHandler.h"

/*
Target data flow: Slave Matrix -> BLE -> Master BLE -> Host Bridge -> USB HID
*/

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);

void BLEHandler(void *parameter)
{
    initializeBLE();

    BLE.setLocalName("Tenki one");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();
    Serial1.println("Advertising started...");

    psychoCharacteristic.setEventHandler(BLEWritten, [](BLEDevice central, BLECharacteristic characteristic)
                                         {
                                             uint8_t data[1];
                                             characteristic.readValue(data, 1);
                                             Serial1.print("Received CapsLock: ");
                                             Serial1.println(data[0]);
                                             capsLockStatus = data[0]; });

    for (;;)
    {
        BLEDevice central = BLE.central();
        if (central && central.connected())
        {
            Serial1.print("Connected to master: ");
            Serial1.println(central.localName());
            moduleConnectionStatus = true;
            while (central.connected())
            {
                BLE.poll();
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            Serial1.println("Disconnected");
            moduleConnectionStatus = false;
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler,     // Function to be called
        "BLE Handler",  // Name of the task
        stackDepth,     // Stack size in words
        NULL,           // Task input parameter
        priority,       // Priority of the task
        &bleTaskHandle, // Task handle
        core            // Core where the task should run
    );
}