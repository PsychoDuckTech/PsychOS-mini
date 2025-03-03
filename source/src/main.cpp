#include "main.h"

const int totalRows = 6, totalCols = 4;
const int rowPins[totalRows] = {0, 1, 2, 3, 4, 5},
          colPins[totalCols] = {6, 7, 9, 10},
          usesMultiplexer = false;
const int ledPin = 8;

bool moduleConnectionStatus = false;
uint8_t capsLockStatus = 0;

// Key Mapping (Layer 0)
const uint8_t keyMapL0[totalRows][totalCols] = {
    {KEY_F5, KEY_B, KEY_C, KEY_D},
    {KEY_NUMLOCK, KEY_KPSLASH, KEY_KPASTERISK, KEY_KPMINUS},
    {KEY_KP1, KEY_KP2, KEY_KP3, KEY_KPPLUS},
    {KEY_KP4, KEY_KP5, KEY_KP6, 0},
    {KEY_KP7, KEY_KP8, KEY_KP9, KEY_KPENTER},
    {KEY_KP0, 0, KEY_KPDOT, 0}};

void setup()
{
  startBleTask();
  startMatrixScanTask();
  pinMode(ledPin, OUTPUT);
}

void loop() {}