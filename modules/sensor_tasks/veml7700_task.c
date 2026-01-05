#include "veml7700_task.h"

void veml7700_task(void *arg)
{
    while (1)
    {
        float lux = veml7700_read_lux();
        if (lux >= 0.0f)
        {
            *(float *)arg = lux;
            vTaskDelay(pdMS_TO_TICKS(3600000));
        }
        else
        {
            printf("Failed to read sensor");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void veml7700_start_task(float *parameter)
{
    xTaskCreatePinnedToCore(veml7700_task, "VEML7700_Task", 2048, parameter, 5, NULL, 1);
}