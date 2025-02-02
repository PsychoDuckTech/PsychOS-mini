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

    for (;;)
    {
        BLEDevice central = BLE.central();
        if (central)
        {
            while (central.connected())
            {
                // Send matrix data
                uint8_t keyData[2] = {currentKeyCode, isPressed};
                psychoCharacteristic.writeValue(keyData, 2);
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            moduleConnectionStatus = false;
        }
    }
}

void handleSlaveBLE()
{
    static bool autoConnected = false;

    if (!autoConnected)
    {
        BLEDevice central = BLE.central();
        if (central && central.localName() == "PsychoMaster")
        {
            Serial.println("Connected to master!");
            moduleConnectionStatus = true;
            autoConnected = true;
        }
    }

    if (moduleConnectionStatus)
    {
        // Keypress data will be sent through matrixScan
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