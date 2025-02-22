#include "matrixScan.h"
#include <Arduino.h>

extern BLECharacteristic psychoCharacteristic;

void matrixScan(void *parameters)
{
    initializeMatrix();

    bool lastKeyStates[totalRows][totalCols] = {false};

    Serial.begin(115200); // Initialize serial communication for debugging

    for (;;)
    {
        for (int row = 0; row < totalRows; row++)
        {
            digitalWrite(rowPins[row], LOW);

            for (int col = 0; col < totalCols; col++)
            {
                bool currentState = digitalRead(colPins[col]) == LOW;

                if (currentState != lastKeyStates[row][col])
                {
                    lastKeyStates[row][col] = currentState;

                    Serial.print("Row: ");
                    Serial.print(row);
                    Serial.print(" Col: ");
                    Serial.print(col);
                    Serial.print(" State: ");
                    Serial.println(currentState);

                    Serial.print("moduleConnectionStatus: ");
                    Serial.println(moduleConnectionStatus);
                    Serial.print("keyMapL0[row][col]: ");
                    Serial.println(keyMapL0[row][col]);

                    if (moduleConnectionStatus && keyMapL0[row][col] != 0)
                    {
                        uint8_t data[2] = {
                            keyMapL0[row][col],
                            static_cast<uint8_t>(currentState ? 1 : 0)};
                        psychoCharacteristic.writeValue(data, sizeof(data));
                        Serial.print("Sent data: ");
                        Serial.print(data[0]);
                        Serial.print(", ");
                        Serial.println(data[1]);
                    }
                    if (currentState)
                    {
                        digitalWrite(ledPin, HIGH);
                    }
                    else
                    {
                        digitalWrite(ledPin, LOW);
                    }
                }
            }

            digitalWrite(rowPins[row], HIGH);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void startMatrixScanTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    xTaskCreatePinnedToCore(
        matrixScan, "MatrixScan", stackDepth, NULL, priority, NULL, core);
}