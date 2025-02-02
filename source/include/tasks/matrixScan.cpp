#include "matrixScan.h"
#include "BLEHandler.h"

extern BLECharacteristic psychoCharacteristic;

#define keyMap keyMapL0
#define keyName keyNameL0

#define benchmark false
#define DEBOUNCE_DELAY_MS 10

void matrixScan(void *parameters)
{
    initializeMatrix();

    Serial.println(task_keyScanning_started);
    Serial.println("\n");

    unsigned long lastTime = millis();
    unsigned long pollCount[totalRows][totalCols] = {0};

    // Arrays to store key states and debounce information.
    bool keyStates[totalRows][totalCols];
    bool lastReading[totalRows][totalCols];
    unsigned long lastDebounceTime[totalRows][totalCols];

    for (int row = 0; row < totalRows; row++)
    {
        gpio_set_level((gpio_num_t)rowPins[row], 0); // Activate current row pin
        for (int col = 0; col < totalCols; col++)    // Scan the column combination with the current row
        {
            bool reading = (digitalRead(colPins[col]) == LOW);
            keyStates[row][col] = reading;
            lastReading[row][col] = reading;
            lastDebounceTime[row][col] = millis();
        }
        gpio_set_level((gpio_num_t)rowPins[row], 1); // Reset the row pin
    }

    for (;;)
    {
        for (int row = 0; row < totalRows; row++)
        {
            gpio_set_level((gpio_num_t)rowPins[row], 0); // Activate current row pin

            for (int col = 0; col < totalCols; col++)
            {
                bool reading = (digitalRead(colPins[col]) == LOW);

                // If the reading has changed from the last reading, reset the debounce timer.
                if (reading != lastReading[row][col])
                {
                    lastDebounceTime[row][col] = millis();
                }

                // If the reading is stable for longer than the debounce delay, update the confirmed state.
                if ((millis() - lastDebounceTime[row][col]) > DEBOUNCE_DELAY_MS)
                {
                    if (reading != keyStates[row][col])
                    {
                        switch (keyMap[row][col])
                        {
                        case 0:
                            break;
                        default:
                            if (moduleConnectionStatus)
                            {
                                uint8_t data[2] = {keyMap[row][col], (reading ? 1 : 0)};
                                psychoCharacteristic.writeValue(data, 2);
                            }
                            break;
                        }
                    }
                }

                lastReading[row][col] = reading;
                pollCount[row][col]++;
            }
            gpio_set_level((gpio_num_t)rowPins[row], 1); // Reset the row pin
        }

        if (benchmark)
        {
            unsigned long currentTime = millis();
            if (currentTime - lastTime >= 1000)
            {
                Serial.println("Individual key polling rates (polls/sec):");
                for (int row = 0; row < totalRows; row++)
                {
                    for (int col = 0; col < totalCols; col++)
                    {
                        Serial.printf("Key [%d][%d]: %lu\n", row, col, pollCount[row][col]);
                        pollCount[row][col] = 0;
                    }
                }
                lastTime = currentTime;
            }
        }
        vTaskDelay(1); // allow other tasks to run, 1ms delay
    }
}

void startMatrixScanTask(UBaseType_t core, uint32_t stackDepth, UBaseType_t priority)
{
    xTaskCreatePinnedToCore(
        matrixScan, "MatrixScan", stackDepth, NULL, priority, NULL, core);
}