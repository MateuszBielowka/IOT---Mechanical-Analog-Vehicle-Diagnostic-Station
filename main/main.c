#include "nvs_flash.h" // pamiec flash
#include "esp_log.h"   // logowanie wiadomosci

// importy
#include "wifi_station.h"
#include "status_led.h"
#include "http_client.h"

#include "hcsr04.h"
#include "bmp280.h"
#include "veml7700.h"
#include "max6675.h"
#include "adxl345.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"        // Required for i2c_config_t, I2C_MODE_MASTER, i2c_driver_install
#include "driver/gpio.h"       // Required for GPIO_PULLUP_ENABLE
#include "driver/spi_master.h" // <--- Add this include at the top

#define I2C_MASTER_NUM I2C_NUM_0
#define SDA_PIN 21
#define SCL_PIN 22

#define PIN_CLK 14
#define PIN_SO 12 // MISO
// MOSI is not needed for MAX6675, but SPI driver might need a pin number or -1
#define PIN_MOSI -1

// tag do logowania w konsoli
static const char *TAG = "app_main";

void init_i2c_global(void)
{
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = SDA_PIN,
      .scl_io_num = SCL_PIN,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = 100000,
  };
  // Install the driver ONCE.
  // If this errors, nothing else will work.
  ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
  ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));
  printf(">>> I2C Driver Installed Globally <<<\n");
}

void init_spi_global(void)
{
  printf("Initializing SPI Bus...\n");

  spi_bus_config_t buscfg = {
      .miso_io_num = PIN_SO,
      .mosi_io_num = PIN_MOSI,
      .sclk_io_num = PIN_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 0, // Default
  };

  // Initialize SPI2_HOST (FSPI)
  // SPI_DMA_CH_AUTO lets ESP32 pick the best DMA settings
  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

  printf(">>> SPI Bus Initialized <<<\n");
}

void app_main(void)
{
  //----Test HC-SR04----
  // hcsr04_start_task();
  //--------------------

  init_i2c_global();
  init_spi_global();
  vTaskDelay(pdMS_TO_TICKS(500));

  bmp280_start_task();
  veml7700_start_task();
  max6675_start_task();
  adxl345_start_task();

  esp_err_t ret = nvs_flash_init(); // inicjalizacja NVS
  // sprawdzanie czy NVS jest pelny lub ma niekompatybilna wersje
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  ESP_LOGI(TAG, "Start aplikacji...");

  status_led_start_task(); // watek obslugujacy diode LED
  wifi_station_init();     // watek obslugujacy wi-fi

  ESP_LOGI(TAG, "Pierwsze połączenie Wi-Fi nawiązane. Uruchamiam klienta HTTP...");

  http_client_start_task(); // watek klienta HTTP
}