#include "main.h"

const int totalRows = 6;
const int totalCols = 4;
const int usesMultiplexer = false;
const int rowPins[totalRows] = {21, 4, 3, 8, 11, 12};
const int colPins[totalCols] = {13, 14, 15, 16};

// Key Mapping (Layer 0)
const uint8_t keyMapL0[totalRows][totalCols] = {
    {0, 0, 0, 0},
    {KEY_NUMLOCK, KEY_SLASH, KEY_KPASTERISK, KEY_MINUS},
    {KEY_1, KEY_2, KEY_3, KEY_KPPLUS},
    {KEY_4, KEY_5, KEY_6, 0},
    {KEY_7, KEY_8, KEY_9, KEY_ENTER},
    {KEY_0, 0, KEY_DELETE, 0}};

const char *keyNameL0[totalRows][totalCols] = {
    {0, 0, 0, 0},
    {"NUMLOCK", "SLASH", "ASTERISK", "MINUS"},
    {"1", "2", "3", "PLUS"},
    {"4", "5", "6", 0},
    {"7", "8", "9", "ENTER"},
    {"0", 0, "DELETE", 0}};

void setup()
{
  initializeMatrix();
  startBleTask();
  startMatrixScanTask();
}
void loop() {}