#include "adxl345.h"
#include "driver/i2c.h"
#include "esp_log.h"

// I2C Configuration
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_SCL_IO 22      // ESP32 GPIO for SCL
#define I2C_SDA_IO 21      // ESP32 GPIO for SDA
#define I2C_FREQ_HZ 100000 // I2C Speed (100kHz)

// ADXL345 Registers
#define REG_POWER_CTL 0x2D   // Power Control Register
#define REG_DATA_FORMAT 0x31 // Data Format Register
#define REG_DATAX0 0x32      // Start of Data Registers (X-Low)

#define ADXL345_LSB_TO_MS2 (0.004f * 9.80665f) // Conversion factor from LSB to m/s² (assuming ±2g range)
static const char *TAG = "ADXL345";

esp_err_t adxl345_init(void)
{
    // 3. Wake up the ADXL345
    // We write to the POWER_CTL register (0x2D)
    // Bit 3 (0x08) is the Measure bit. Setting it to 1 enables measurement.
    uint8_t data_enable[2] = {REG_POWER_CTL, 0x08};
    esp_err_t err = i2c_master_write_to_device(I2C_MASTER_NUM, ADXL345_ADDR, data_enable, 2, 1000 / portTICK_PERIOD_MS);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "ADXL345 Initialized successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize ADXL345");
    }

    return err;
}

esp_err_t adxl345_read_data(adxl345_data_t *data)
{
    uint8_t raw_data[6]; // Buffer for X0, X1, Y0, Y1, Z0, Z1
    uint8_t reg_addr = REG_DATAX0;

    // Read 6 bytes of data starting from REG_DATAX0
    esp_err_t err = i2c_master_write_read_device(
        I2C_MASTER_NUM,
        ADXL345_ADDR,
        &reg_addr,
        1,
        raw_data,
        6,
        1000 / portTICK_PERIOD_MS);

    if (err == ESP_OK)
    {
        // Combine low and high bytes into 16-bit signed integers
        // ADXL345 sends Least Significant Byte first
        int16_t raw_x = (int16_t)((raw_data[1] << 8) | raw_data[0]);
        int16_t raw_y = (int16_t)((raw_data[3] << 8) | raw_data[2]);
        int16_t raw_z = (int16_t)((raw_data[5] << 8) | raw_data[4]);
        data->x = raw_x * ADXL345_LSB_TO_MS2;
        data->y = raw_y * ADXL345_LSB_TO_MS2;
        data->z = raw_z * ADXL345_LSB_TO_MS2;
    }
    else
    {
        ESP_LOGE(TAG, "Failed to read data");
    }

    return err;
}

void adxl345_task(void *arg)
{
    float combined_acceleration = 0.0f;
    if (adxl345_init() != ESP_OK)
    {
        ESP_LOGE(TAG, "Sensor init failed! Restarting...");
    }

    adxl345_data_t acc_data;

    while (1)
    {
        // Read data from sensor
        if (adxl345_read_data(&acc_data) == ESP_OK)
        {
            ESP_LOGI(TAG, "X: %.3f m/s² | Y: %.3f m/s² | Z: %.3f m/s²",
                     acc_data.x, acc_data.y, acc_data.z);
        }

        // Wait for 500ms before next read
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void adxl345_start_task()
{
    xTaskCreate(adxl345_task, "ADXL345_Task", 4096, NULL, 10, NULL);
}