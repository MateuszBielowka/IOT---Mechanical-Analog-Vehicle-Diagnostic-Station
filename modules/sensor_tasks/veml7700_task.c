#include "veml7700_task.h"

void veml7700_task(void *arg)
{
    while (1)
    {
        float lux = veml7700_read_lux();
        if (lux >= 0.0f)
        {
            *(float *)arg = lux;
            vTaskDelay(3600000);
        }
        else
        {
            printf("Failed to read sensor");
            vTaskDelay(1000);
        }
    }
}

void veml7700_start_task(float *parameter)
{
    xTaskCreate(veml7700_task, "VEML7700_Task", 2048, parameter, 5, NULL);
}