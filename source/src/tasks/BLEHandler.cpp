#include <Arduino.h>
#include <ArduinoBLE.h>
#include "tasks/BLEHandler.h"
#include "globals.h"
#include "utils/initializeBLE.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-000000000001"

QueueHandle_t hostMessageQueue;
static bool localKeyPressed = false; // Track if the key was from local matrix

BLEService psychoService(SERVICE_UUID);
BLECharacteristic psychoCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 20);
BLECharacteristic remotePsychoCharacteristic;

enum class BLEConnectionState
{
    DISCONNECTED,
    CONNECTING,
    DISCOVERING_SERVICES,
    CONNECTED
};

struct BLEConnection
{
    BLEDevice peripheral;
    BLECharacteristic characteristic;
    BLEConnectionState state = BLEConnectionState::DISCONNECTED;
    bool isConnected = false;
};

static BLEConnection connection;

struct BLEMessage
{
    enum Type
    {
        KEY_EVENT,
        CAPS_LOCK_STATUS
    } type;
    union Data
    {
        uint8_t rawData[20];
        struct
        {
            uint8_t keyCode;
            uint8_t isPressed;
        } keyEvent;
    } data;
    int length;
};

// Function declarations
void handleReceivedKeypress(const uint8_t *data, int length, bool fromMatrix);
bool handleConnection(BLEConnection &conn);

void handleReceivedKeypress(const uint8_t *data, int length, bool fromMatrix)
{
    if (length < 2)
        return;

    uint8_t keyCode = data[0];
    bool isPressed = data[1] == 1;

    if (fromMatrix)
    {
        // Matrix key - send directly to connected device
        if (connection.isConnected)
        {
            const uint8_t eventData[2] = {keyCode, static_cast<uint8_t>(isPressed ? 1 : 0)};
            connection.characteristic.writeValue(eventData, 2);
            Serial.print("BLE: Sending matrix key ");
            Serial.print(isPressed ? "press: " : "release: ");
            Serial.println(keyCode, HEX);
        }
    }
    // Ignore BLE received keys - we don't need to echo them back
    else
    {
        Serial.print("BLE: Received key ");
        Serial.print(isPressed ? "press: " : "release: ");
        Serial.println(keyCode, HEX);
    }
}

bool handleConnection(BLEConnection &conn)
{
    switch (conn.state)
    {
    case BLEConnectionState::DISCONNECTED:
        Serial.println("BLE: Scanning for Kibodo One...");
        BLE.scanForName("Kibodo One");
        conn.peripheral = BLE.available();
        if (conn.peripheral)
        {
            Serial.println("BLE: Found Kibodo One, attempting connection...");
            BLE.stopScan();
            conn.state = BLEConnectionState::CONNECTING;
        }
        break;

    case BLEConnectionState::CONNECTING:
        if (conn.peripheral.connect())
        {
            Serial.println("BLE: Connected, discovering services...");
            conn.state = BLEConnectionState::DISCOVERING_SERVICES;
        }
        else
        {
            Serial.println("BLE: Connection failed");
            conn.state = BLEConnectionState::DISCONNECTED;
        }
        break;

    case BLEConnectionState::DISCOVERING_SERVICES:
        if (conn.peripheral.discoverAttributes())
        {
            BLEService service = conn.peripheral.service(SERVICE_UUID);
            if (service)
            {
                conn.characteristic = service.characteristic(CHARACTERISTIC_UUID);
                if (conn.characteristic)
                {
                    // Subscribe to notifications
                    if (conn.characteristic.subscribe())
                    {
                        conn.state = BLEConnectionState::CONNECTED;
                        conn.isConnected = true;
                        moduleConnectionStatus = true;
                        Serial.println("BLE: Successfully connected to Kibodo One and subscribed to notifications");
                        return true;
                    }
                    else
                    {
                        Serial.println("BLE: Failed to subscribe to notifications");
                    }
                }
            }
            Serial.println("BLE: Failed to setup BLE characteristic");
        }
        conn.peripheral.disconnect();
        conn.state = BLEConnectionState::DISCONNECTED;
        break;

    case BLEConnectionState::CONNECTED:
        if (!conn.peripheral.connected())
        {
            Serial.println("BLE: Connection lost");
            moduleConnectionStatus = false;
            conn.isConnected = false;
            conn.state = BLEConnectionState::DISCONNECTED;
            return false;
        }
        return true;
    }
    return false;
}

void BLEHandler(void *parameter)
{
    // Create message queue for key events
    hostMessageQueue = xQueueCreate(32, sizeof(HostMessage));
    if (hostMessageQueue == NULL)
    {
        Serial.println("BLE: Failed to create key event queue!");
        return;
    }

    initializeBLE();
    BLE.setLocalName("Tenki one");

    // Set up peripheral role
    BLE.setAdvertisedService(psychoService);
    psychoService.addCharacteristic(psychoCharacteristic);
    BLE.addService(psychoService);
    BLE.advertise();

    Serial.println("BLE: Started advertising as 'Tenki one'");

    for (;;)
    {
        // Handle any central connections
        BLEDevice central = BLE.central();
        if (central)
        {
            Serial.print("BLE: Connected to central: ");
            Serial.println(central.address());
            while (central.connected())
            {
                if (psychoCharacteristic.written())
                {
                    uint8_t data[20];
                    int length = psychoCharacteristic.readValue(data, sizeof(data));
                    handleReceivedKeypress(data, length);
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }
            Serial.println("BLE: Disconnected from central");
        }

        // Handle connection to Kibodo One
        if (!connection.isConnected)
        {
            handleConnection(connection);
        }
        else
        {
            // We're connected, check if we have data to read
            if (connection.characteristic.valueUpdated())
            {
                uint8_t data[20];
                int length = connection.characteristic.readValue(data, sizeof(data));
                handleReceivedKeypress(data, length);
            }

            // Process any messages in the queue
            HostMessage msg;
            while (xQueueReceive(hostMessageQueue, &msg, 0) == pdTRUE)
            {
                // Send key event to connected device with proper uint8_t initialization
                const uint8_t pressed = uint8_t{1};
                const uint8_t released = uint8_t{0};
                const uint8_t keyState = (msg.type == KEY_PRESS) ? pressed : released;
                const uint8_t eventData[2] = {msg.data, keyState};
                connection.characteristic.writeValue(eventData, 2);
                Serial.print("BLE: Sending key ");
                Serial.print(msg.type == KEY_PRESS ? "press: " : "release: ");
                Serial.println(msg.data);
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