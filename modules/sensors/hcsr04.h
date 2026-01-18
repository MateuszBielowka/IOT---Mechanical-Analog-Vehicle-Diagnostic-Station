#ifndef HCSR04
#define HCSR04

#include <stdbool.h>
#include <stdio.h>
#include "project_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "hcsr04_driver.h"
#include "esp_timer.h"
#include "buzzer.h"

void hcsr04_task(void *arg);

void hcsr04_start_task(float *parameter);

#endif // HCSR04