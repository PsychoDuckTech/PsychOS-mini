#include "main.h"

const int totalRows = 6;
const int totalCols = 4;
const int usesMultiplexer = false;
const int rowPins[totalRows] = {0, 1, 2, 3, 4, 5};
const int colPins[totalCols] = {6, 7, 9, 10};
const int ledPin = 8;

bool moduleConnectionStatus = false;
uint8_t capsLockStatus = 0;

// Key Mapping (Layer 0)
const uint8_t keyMapL0[totalRows][totalCols] = {
    {KEY_F5, KEY_B, KEY_C, KEY_D},
    {KEY_NUMLOCK, KEY_SLASH, KEY_KPASTERISK, KEY_MINUS},
    {KEY_1, KEY_2, KEY_3, KEY_KPPLUS},
    {KEY_4, KEY_5, KEY_6, 0},
    {KEY_7, KEY_8, KEY_9, KEY_ENTER},
    {KEY_0, 0, KEY_DELETE, 0}};

void setup()
{
  startBleTask();
  startMatrixScanTask();
  pinMode(ledPin, OUTPUT);
}

void loop() {}