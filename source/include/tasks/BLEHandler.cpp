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

    BLE.setLocalName("PsychoNumpad");
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();

    psychoCharacteristic.setEventHandler(BLEWritten, [](BLEDevice central, BLECharacteristic characteristic)
                                         {
                                             uint8_t data[1];
                                             characteristic.readValue(data, 1);
                                             capsLockStatus = data[0]; // Update from main keyboard
                                         });

    for (;;)
    {
        BLEDevice central = BLE.central();
        if (central.connected())
        {
            Serial.println("Connected to master!");
            moduleConnectionStatus = true;
        }
        while (central.connected())
        {
            BLE.poll(); // Important for callback handling
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
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