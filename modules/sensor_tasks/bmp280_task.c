#include "bmp280_task.h"

void bmp280_task(void *arg)
{
    float temp;
    while (1)
    {
        bmp280_trigger_forced_mode();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        temp = bmp280_read_temp();
        if (temp != -1.0f)
        {
            *(float *)arg = temp;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        else
        {
            printf("Failed to read temperature");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}

void bmp280_start_task(float *temperature)
{
    xTaskCreate(bmp280_task, "bmp280_task", 2048, temperature, 5, NULL);
}