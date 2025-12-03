/* veml7700.h */
#ifndef VEML7700_H
#define VEML7700_H

#include <esp_err.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief Initialize the I2C bus and the VEML7700 sensor.
     *
     * @param sda_pin GPIO pin for Data
     * @param scl_pin GPIO pin for Clock
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t veml7700_init(int sda_pin, int scl_pin);

    /**
     * @brief Reads the Ambient Light in Lux.
     *
     * Includes automatic correction for high-brightness non-linearity.
     *
     * @param lux_val Pointer to a float where the result will be stored.
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t veml7700_read_lux(float *lux_val);

    void veml7700_task(void *arg);

    void veml7700_start_task(float *parameter);

#ifdef __cplusplus
}
#endif

#endif // VEML7700_H