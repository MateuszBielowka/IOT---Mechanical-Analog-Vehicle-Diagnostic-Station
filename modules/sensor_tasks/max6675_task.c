#include "max6675.h"

void max6675_task(void *arg)
{
    spi_device_handle_t dev_handle = *((spi_device_handle_t *)arg);
    while (1)
    {
        float engine_temp = max6675_read_celsius(dev_handle);
        if (engine_temp != -1.0f)
        {
            *(float *)arg = engine_temp;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        else
        {
            printf("Error reading MAX6675 temperature.\n");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}

void max6675_start_task(float *engine_temp)
{
    xTaskCreate(max6675_task, "max6675_task", 2048, engine_temp, 5, NULL);
}