#include "tasks/matrixScan.h"
#include <Arduino.h>
#include "tasks/BLEHandler.h"
#include "globals.h"

// Function declaration from BLEHandler.cpp
void handleReceivedKeypress(const uint8_t *data, int length, bool fromMatrix);

void matrixScan(void *parameters)
{
    initializeMatrix();

    bool lastKeyStates[totalRows][totalCols] = {false};

    Serial.begin(115200);

    for (;;)
    {
        bool anyKeyPressed = false;
        for (int row = 0; row < totalRows; row++)
        {
            digitalWrite(rowPins[row], LOW);
            delayMicroseconds(10);

            for (int col = 0; col < totalCols; col++)
            {
                bool currentState = digitalRead(colPins[col]) == LOW;

                if (currentState != lastKeyStates[row][col])
                {
                    lastKeyStates[row][col] = currentState;
                    if (currentState)
                    {
                        anyKeyPressed = true;
                    }

                    if (moduleConnectionStatus && keyMapL0[row][col] != 0)
                    {
                        uint8_t data[2] = {
                            keyMapL0[row][col],
                            static_cast<uint8_t>(currentState ? 1 : 0)};

                        Serial.print("Matrix: Key ");
                        Serial.print(currentState ? "pressed: " : "released: ");
                        Serial.println(data[0], HEX);

                        // Send to BLE handler with fromMatrix=true
                        handleReceivedKeypress(data, 2, true);
                    }

                    digitalWrite(ledPin, currentState ? HIGH : LOW);
                }
            }

            digitalWrite(rowPins[row], HIGH);
        }

        vTaskDelay((anyKeyPressed ? 10 : 50) / portTICK_PERIOD_MS);
    }
}

void startMatrixScanTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    xTaskCreatePinnedToCore(
        matrixScan, "MatrixScan", stackDepth, NULL, priority, NULL, core);
}