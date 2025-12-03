#ifndef MAX6675_H
#define MAX6675_H

#include "driver/spi_master.h"
#include "esp_err.h"

// Configuration structure
typedef struct
{
    int sck_io_num;             // Serial Clock Pin
    int miso_io_num;            // Serial Data Out (SO) Pin
    int cs_io_num;              // Chip Select (CS) Pin
    spi_host_device_t spi_host; // e.g., SPI2_HOST or SPI3_HOST
} max6675_config_t;

// Handle for the device
typedef struct
{
    spi_device_handle_t spi_dev;
} max6675_handle_t;

typedef struct {
    float temperature;
    uint32_t seconds;
} max6675_sample_t;

/**
 * @brief Initialize the MAX6675 sensor
 *
 * @param cfg Configuration struct
 * @param out_handle Pointer to store the resulting device handle
 * @return esp_err_t ESP_OK on success
 */
esp_err_t max6675_init(const max6675_config_t *cfg, max6675_handle_t *out_handle);

/**
 * @brief Read temperature in Celsius
 *
 * @param handle Device handle
 * @return float Temperature in Celsius, or -1.0 if reading failed/thermocouple open
 */
float max6675_read_celsius(max6675_handle_t *handle);

void max6675_task(void *arg);
void max6675_profile_task(void *arg);
void max6675_start_task(float *parameter);
void max6675_start_profile_task(max6675_sample_t *result_array);

#endif // MAX6675_H