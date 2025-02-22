#include "matrixScan.h"

extern BLECharacteristic psychoCharacteristic;

void matrixScan(void *parameters)
{
    initializeMatrix();

    bool lastKeyStates[totalRows][totalCols] = {false};

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

                    if (moduleConnectionStatus && keyMapL0[row][col] != 0)
                    {
                        uint8_t data[2] = {
                            keyMapL0[row][col],
                            static_cast<uint8_t>(currentState ? 1 : 0)};
                        psychoCharacteristic.writeValue(data, sizeof(data));
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