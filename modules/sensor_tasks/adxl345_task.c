#include "adxl345_task.h"

void adxl345_task(void *arg)
{
    while (1)
    {
        float acceleration = adxl345_read_data();
        if (acceleration != -1.0f)
        {
            *(float *)arg = acceleration;
            vTaskDelay(1000);
        }
        else
        {
            printf("Failed to read ADXL345 data");
            vTaskDelay(1000);
        }
    }
}

void adxl345_start_task(float *parameter)
{
    xTaskCreate(adxl345_task, "adxl345_task", 2048, parameter, 5, NULL);
}