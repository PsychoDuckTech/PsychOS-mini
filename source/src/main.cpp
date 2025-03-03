#include "main.h"

const int totalRows = 6, totalCols = 4;
const int rowPins[totalRows] = {0, 1, 2, 3, 4, 5},
          colPins[totalCols] = {6, 7, 8, 9},
          usesMultiplexer = false;
const int ledPin = 10;

bool moduleConnectionStatus = false;
uint8_t capsLockStatus = 0;

// Key Mapping (HID codes as hex values)
const uint8_t keyMapL0[totalRows][totalCols] = {
    {0x3F, 0x27, 0x06, 0x07}, // KEY_F5, KEY_B, KEY_C, KEY_D (example codes)
    {0x53, 0x54, 0x55, 0x56}, // KEY_NUMLOCK, KEY_KPSLASH, etc.
    {0x31, 0x32, 0x33, 0x57}, // KEY_KP1, KEY_KP2, KEY_KP3, KEY_KPPLUS
    {0x34, 0x35, 0x36, 0x00}, // KEY_KP4, KEY_KP5, KEY_KP6, 0x00 (unused)
    {0x37, 0x38, 0x39, 0x58}, // KEY_KP7, KEY_KP8, KEY_KP9, KEY_KPENTER
    {0x40, 0x00, 0x63, 0x00}  // KEY_KP0, 0x00, KEY_KPDOT, 0x00
};

void setup()
{
  startBleTask();
  startMatrixScanTask();
  pinMode(ledPin, OUTPUT);
}

void loop() {}