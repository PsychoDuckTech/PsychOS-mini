#pragma once

#include <Arduino.h>
#include "main.h"
#include "utils/initializeMatrix.h"
#include "globals.h"
#include "BLEHandler.h"

// External Variables from main.cpp
extern const int rowPins[];
extern const int colPins[];
extern const uint8_t keyMapL0[6][4];
extern const char *keyNameL0[6][4];
extern const int totalRows;
extern const int totalCols;

void startMatrixScanTask(UBaseType_t core = 0, uint32_t stackDepth = 8192, UBaseType_t priority = 3);