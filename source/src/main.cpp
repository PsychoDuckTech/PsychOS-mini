#include "main.h"

const int totalRows = 6, totalCols = 4;
const int rowPins[totalRows] = {1, 2, 3, 4, 5, 6},
          colPins[totalCols] = {7, 8, 9, 10},
          usesMultiplexer = false;
const int ledPin = 11;

bool moduleConnectionStatus = false;
uint8_t capsLockStatus = 0;

// Key Mapping (HID codes as hex values)
const uint8_t keyMapL0[totalRows][totalCols] = {
    {0x00, 0x00, 0x00, 0x00}, // no keys here for now
    {0x53, 0x54, 0x55, 0x56}, // HID_KEY_NUM_LOCK, HID_KEY_KEYPAD_DIVIDE, HID_KEY_KEYPAD_MULTIPLY, HID_KEY_KEYPAD_SUBTRACT
    {0x59, 0x5A, 0x5B, 0x57}, // HID_KEY_KEYPAD_1, HID_KEY_KEYPAD_2, HID_KEY_KEYPAD_3, HID_KEY_KEYPAD_ADD
    {0x5C, 0x5D, 0x5E, 0x00}, // HID_KEY_KEYPAD_4, HID_KEY_KEYPAD_5, HID_KEY_KEYPAD_6, 0x00 (unused)
    {0x5F, 0x60, 0x61, 0x58}, // HID_KEY_KEYPAD_7, HID_KEY_KEYPAD_8, HID_KEY_KEYPAD_9, HID_KEY_KEYPAD_ENTER
    {0x62, 0x00, 0x63, 0x00}  // HID_KEY_KEYPAD_0, 0x00, HID_KEY_KEYPAD_DECIMAL, 0x00
};

void setup()
{
  startBleTask();
  startMatrixScanTask();
  pinMode(ledPin, OUTPUT);
}

void loop() {}