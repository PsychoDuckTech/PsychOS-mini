#include <Arduino.h>
#include <ArduinoBLE.h>
#include "BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"
#include "BLEProtocol.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);

// Module state tracking
static ModuleInfo moduleInfo = {
    .moduleId = 0xFF,  // Unassigned
    .capabilities = CAP_EXTRA_KEYS,  // Update based on actual capabilities
    .totalKeys = 16,   // Update based on actual number of keys
    .version = 1       // Current protocol version
};

static uint8_t missedHeartbeats = 0;
static unsigned long lastHeartbeatTime = 0;

// Helper functions
static void sendHandshakeResponse() {
    uint8_t response[5];
    response[0] = HANDSHAKE_RESPONSE;
    memcpy(&response[1], MAGIC_RESPONSE, 4);
    psychoCharacteristic.writeValue(response, 5);
}

static void sendModuleInfo() {
    uint8_t response[sizeof(ModuleInfo) + 1];
    response[0] = MODULE_INFO_RESPONSE;
    memcpy(&response[1], &moduleInfo, sizeof(ModuleInfo));
    psychoCharacteristic.writeValue(response, sizeof(ModuleInfo) + 1);
}

static void sendHeartbeatResponse() {
    uint8_t response[1] = {HEARTBEAT_RESPONSE};
    psychoCharacteristic.writeValue(response, 1);
}

static void handleReceivedMessage(const uint8_t* data, size_t length) {
    if (length < 1) return;

    BLEMessageType messageType = (BLEMessageType)data[0];
    switch (messageType) {
        case HANDSHAKE_REQUEST:
            if (length >= 9 && memcmp(&data[1], MAGIC_PACKET, 8) == 0) {
                sendHandshakeResponse();
            }
            break;

        case MODULE_INFO_REQUEST:
            if (length >= 2) {
                moduleInfo.moduleId = data[1];  // Save assigned module ID
                sendModuleInfo();
            }
            break;

        case CAPS_LOCK_STATUS:
            if (length >= 2) {
                capsLockStatus = data[1];
                // Update LED state based on caps lock if needed
            }
            break;

        case HEARTBEAT:
            missedHeartbeats = 0;
            lastHeartbeatTime = millis();
            sendHeartbeatResponse();
            break;

        default:
            break;
    }
}

void sendKeyEvent(uint8_t keyCode, bool isPressed) {
    if (moduleInfo.moduleId == 0xFF) return; // Not properly initialized

    uint8_t message[sizeof(KeyEvent) + 1];
    message[0] = KEY_EVENT;
    
    KeyEvent event = {
        .keyCode = keyCode,
        .isPressed = isPressed,
        .moduleId = moduleInfo.moduleId
    };
    
    memcpy(&message[1], &event, sizeof(KeyEvent));
    psychoCharacteristic.writeValue(message, sizeof(KeyEvent) + 1);
}

void BLEHandler(void *parameter) {
    initializeBLE();
    bool wasConnected = false;
    BLEDevice mainKeyboard;  // Main keyboard peripheral

    // Start scanning for the main keyboard
    BLE.scan();
    Serial1.println("Scanning for Kibodo one...");    for (;;) {
        BLEDevice peripheral = BLE.available();
        
        if (peripheral) {
            // Check if this is the main keyboard
            if (String(peripheral.localName()) == "Kibodo one") {
                BLE.stopScan();
                Serial1.println("Found Kibodo one keyboard!");

                if (peripheral.connect()) {
                    Serial1.println("Connected to main keyboard");
                    moduleConnectionStatus = true;
                    wasConnected = true;
                    mainKeyboard = peripheral;
                    missedHeartbeats = 0;
                    lastHeartbeatTime = millis();

                    // Discover the service and characteristic
                    if (peripheral.discoverAttributes()) {
                        BLECharacteristic keyboardChar = peripheral.characteristic(CHARACTERISTIC_UUID);
                        
                        if (keyboardChar) {
                            // Subscribe to notifications
                            if (keyboardChar.subscribe()) {
                                // Monitor connection and handle messages
                                while (peripheral.connected()) {
                                    if (keyboardChar.valueUpdated()) {
                                        const uint8_t* data = keyboardChar.value();
                                        size_t length = keyboardChar.valueLength();
                                        handleReceivedMessage(data, length);
                                    }

                                    // Monitor heartbeat timeout
                                    if (millis() - lastHeartbeatTime > MODULE_CHECK_INTERVAL) {
                                        missedHeartbeats++;
                                        lastHeartbeatTime = millis();
                                        
                                        if (missedHeartbeats >= MAX_MISSED_CHECKS) {
                                            // Connection considered lost
                                            peripheral.disconnect();
                                            break;
                                        }
                                    }

                                    vTaskDelay(10 / portTICK_PERIOD_MS);
                                }
                            }
                        }
                    }
                }

                if (wasConnected) {
                    Serial1.println("Main keyboard disconnected");
                    moduleConnectionStatus = false;
                    wasConnected = false;
                    moduleInfo.moduleId = 0xFF;  // Reset module ID
                    BLE.scan(); // Resume scanning
                    Serial1.println("Scanning for Kibodo one...");
                }
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void startBleTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority) {
    TaskHandle_t bleTaskHandle;
    xTaskCreatePinnedToCore(
        BLEHandler, "BLE Handler", stackDepth, NULL, priority, &bleTaskHandle, core);
}