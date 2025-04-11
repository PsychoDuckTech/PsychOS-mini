#pragma once
#include <stdint.h>

// Magic packets for handshake
#define MAGIC_PACKET "PSYCHO_KB"
#define MAGIC_RESPONSE "PMOD"

// Protocol constants
#define MODULE_CHECK_INTERVAL 60    // ms
#define MAX_MISSED_CHECKS 3

enum BLEMessageType {
    HANDSHAKE_REQUEST = 0x01,
    HANDSHAKE_RESPONSE = 0x02,
    MODULE_INFO_REQUEST = 0x03,
    MODULE_INFO_RESPONSE = 0x04,
    KEY_EVENT = 0x05,
    CAPS_LOCK_STATUS = 0x06,
    HEARTBEAT = 0x07,
    HEARTBEAT_RESPONSE = 0x08
};

// Module capabilities bitmap
#define CAP_ROTARY_ENCODER 0x01
#define CAP_DISPLAY 0x02
#define CAP_RGB_LEDS 0x04
#define CAP_EXTRA_KEYS 0x08
#define CAP_ANALOG_INPUTS 0x10

struct ModuleInfo {
    uint8_t moduleId;           // Assigned by main keyboard (0-2)
    uint8_t capabilities;       // Bitmap of supported features
    uint8_t totalKeys;         // Number of keys on the module
    uint8_t version;           // Protocol version
} __attribute__((packed));

struct KeyEvent {
    uint8_t keyCode;           // HID key code
    bool isPressed;            // true = pressed, false = released
    uint8_t moduleId;          // Source module ID
} __attribute__((packed));
