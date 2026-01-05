#include "bmp280.h"

void bmp280_task(void *arg)
{
    i2c_master_dev_handle_t dev_handle = *((i2c_master_dev_handle_t *)arg);
    float temp;
    while (1)
    {
        bmp280_trigger_forced_mode(dev_handle);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        temp = bmp280_read_temp(dev_handle);
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