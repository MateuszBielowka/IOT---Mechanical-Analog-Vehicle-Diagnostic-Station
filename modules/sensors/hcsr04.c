
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hcsr04_driver.h"

static const char *TAG = "HCSR04";

void hcsr04_task(void *pvParameters)
{
    esp_err_t return_value = ESP_OK;
    (void)UltrasonicInit();

    // create variable which stores the measured distance
    static uint32_t afstand = 0;

    while (1)
    {
        return_value = UltrasonicMeasure(100, &afstand);
        UltrasonicAssert(return_value);
        if (return_value == ESP_OK)
        {
            // ESP_LOGI(TAG, "Distance: %ldcm", afstand);
            *(double *)pvParameters = (double)afstand;
        }

        // 0,5 second delay before starting new measurement
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void hcsr04_start_task(double *parameter)
{
    // Create measurement task
    xTaskCreate(hcsr04_task, "HSRC04 task", 2048, parameter, 4, NULL);
}
