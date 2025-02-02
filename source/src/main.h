#pragma once

#include <Arduino.h>
#include "usbHIDKeyCodes.h" // by MightyPork on GitHub, customized by GabiBrawl
#include "translations/enUS.h"

// Global Constants
extern const char *OS_version;
extern const char *byCompany;
#define PRODUCT_NAME "Tenki one" // In Japanese, the numeric keypad is called "テンキー" (tenkī), a term derived from the English "ten key."

// Matrix Configuration
extern const int totalRows;
extern const int totalCols;
extern const int usesMultiplexer;
extern const int rowPins[];
extern const int colPins[]; // Add this if using non-multiplexer mode

// Key Maps & Names
extern const uint8_t keyMapL0[][4];
extern const char *keyNameL0[][4];

// ===== Task Includes (AFTER declarations) =====
#include "tasks.h"

// Task Functions

// Utility Functions
void initializeMatrix();
void submitKeyPress(uint8_t keyCode, bool isPressed);