#pragma once
#include <Arduino.h>
#include <ArduinoBLE.h>
#include "main.h"

void initializeBLE()
{
    if (!BLE.begin())
    {
        Serial1.println(BLE_initialization_failed);
        ESP.restart();
    }
    Serial1.println(BLE_initialized_successfully);
}