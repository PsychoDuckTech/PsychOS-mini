#include "tasks/matrixScan.h"
#include <Arduino.h>
#include "tasks/BLEHandler.h"
#include "globals.h"

void matrixScan(void *parameters)
{
    initializeMatrix();
    bool lastKeyStates[totalRows][totalCols] = {false};
    Serial.begin(115200);

    for (;;)
    {
        if (!moduleConnectionStatus)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
            continue;
        }

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

                    if (keyMapL0[row][col] != 0)
                    {
                        sendKeyEvent(keyMapL0[row][col], currentState);
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