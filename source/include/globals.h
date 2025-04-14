#pragma once
#include <Arduino.h>
#include "translations/enUS.h"

// Message types for key events
enum MessageType
{
    KEY_PRESS,
    KEY_RELEASE
};

// Host message structure
struct HostMessage
{
    MessageType type;
    uint8_t data;
};

// Message queue for key events
extern QueueHandle_t hostMessageQueue;

extern const int totalRows;
extern const int totalCols;
extern const int usesMultiplexer;
extern const int rowPins[];
extern const int colPins[];

extern const int ledPin;

extern bool connectionStatus;
extern bool moduleConnectionStatus;
extern uint8_t capsLockStatus;

#include "tasks.h"