#include "project_config.h"

#include <stdio.h>
#include <string.h>
#include "storage_manager.h"
#include "button.h"
#include "ble_server.h" // Bluetooth (Konfiguracja)

#include "esp_log.h"   // logowanie wiadomosci
#include "nvs_flash.h" // pamiec flash

#include "driver/i2c_master.h" // Required for i2c_config_t, I2C_MODE_MASTER, i2c_driver_install
#include "driver/gpio.h"       // Required for GPIO_PULLUP_ENABLE
#include "driver/spi_master.h" // <--- Add this include at the top

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c_master_bus.h"
#include "spi_master_bus.h"

#include "storage_manager.h"
#include "bmp280.h"
#include "veml7700.h"
#include "max6675.h"
#include "adxl345.h"
#include "hcsr04.h"

#include "bmp280_task.h"
#include "max6675_task.h"
#include "veml7700_task.h"
#include "adxl345_task.h"

#include "wifi_station.h"
#include "status_led.h"
#include "http_client.h"

#include "buzzer.h"

// stałe do zastąpienia funkcjami i zmiennymi
#define BLE_PAIRED_SUCCESS true
#define MEMORY_USAGE_PERCENT 75

// Console tag
static const char *TAG = "app_main";

typedef struct
{
  float temperature;
  float illuminance;
  float engine_temp;
  float distance;
  float acceleration;
} sensor_data_t;

void initialize_master_buses(i2c_master_bus_handle_t *i2c_bus_0, i2c_master_bus_handle_t *i2c_bus_1)
{
  i2c_initialize_master(I2C_NUM_0, I2C_PORT_0_SDA_PIN, I2C_PORT_0_SCL_PIN, 100000, i2c_bus_0); // BMP280 & ADXL345
  i2c_initialize_master(I2C_NUM_1, I2C_PORT_1_SDA_PIN, I2C_PORT_1_SCL_PIN, 100000, i2c_bus_1); // VEML7700
  ESP_LOGI(TAG, "I2C initialized.");
  spi_initialize_master(SPI2_HOST, SPI_PORT_0_MISO_PIN, SPI_PORT_0_MOSI_PIN, SPI_PORT_0_SCK_PIN); // MAX6675
  ESP_LOGI(TAG, "SPI initialized.");
}

void initialize_devices(spi_device_handle_t *max6675_handle, i2c_master_bus_handle_t *i2c_handle_0, i2c_master_bus_handle_t *i2c_handle_1)
{
  veml7700_init(*i2c_handle_1);
  bmp280_init(*i2c_handle_0);
  adxl345_init(*i2c_handle_0);
  max6675_init(max6675_handle);
  ESP_LOGI(TAG, "Devices initialized.");
}

void configure_device_defaults(void)
{
  bmp280_configure();
  adxl345_configure();
  ESP_LOGI(TAG, "Devices configured with default settings.");
}

static void init_nvs(void)
{
  esp_err_t ret = nvs_flash_init();

  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);
}

static sensor_data_t collect_sensor_data(float bmp, float lux, float eng, float dist, float accel)
{
  sensor_data_t data = {
      .temperature = bmp,
      .illuminance = lux,
      .engine_temp = eng,
      .distance = dist,
      .acceleration = accel};
  return data;
}

static void print_sensor_data(const sensor_data_t *data)
{
  printf("BMP280:    %.2f C\n", data->temperature);
  printf("VEML7700:  %.2f Lux\n", data->illuminance);
  printf("MAX6675:   %.2f C\n", data->engine_temp);
  printf("HC-SR04:   %.2f cm\n", data->distance);
  printf("ADXL345:   %.2f m/s2\n", data->acceleration);
}

static void format_sensor_line(char *out, size_t len, const sensor_data_t *d)
{
  snprintf(out, len,
           "BMP280: %.2f C\n"
           "VEML7700: %.2f Lux\n"
           "MAX6675: %.2f C\n"
           "HC-SR04: %.2f cm\n"
           "ADXL345: %.2f m/s2\n",
           d->temperature, d->illuminance, d->engine_temp, d->distance, d->acceleration);
}

void get_line_from_console(char *buffer, size_t max_len)
{
  size_t index = 0;
  int c;
  while (1)
  {
    c = fgetc(stdin);
    if (c == EOF)
    {
      vTaskDelay(10 / portTICK_PERIOD_MS);
      continue;
    }
    if (c == '\n' || c == '\r')
    {
      if (index > 0)
      {
        buffer[index] = 0;
        printf("\n");
        return;
      }
    }
    else
    {
      if (index < max_len - 1)
      {
        buffer[index++] = (char)c;
        printf("%c", c);
      }
    }
  }
}

void app_main(void)
{
  i2c_master_bus_handle_t i2c_bus_0;
  i2c_master_bus_handle_t i2c_bus_1;
  initialize_master_buses(&i2c_bus_0, &i2c_bus_1);
  spi_device_handle_t max6675_handle;
  initialize_devices(&max6675_handle, &i2c_bus_0, &i2c_bus_1);
  configure_device_defaults();

  init_nvs();

  storage_init();
  vTaskDelay(pdMS_TO_TICKS(STARTUP_DELAY_MS));

  float bmp280_temp = 0.0f, veml7700_illuminance = 0.0f, max6675_engine_temp = 0.0f, adxl345_acceleration = 0.0f, hcsr04_distance = 0.0f;

  bmp280_start_task(&bmp280_temp);
  veml7700_start_task(&veml7700_illuminance);
  max6675_start_task(&max6675_engine_temp);
  adxl345_start_task(&adxl345_acceleration);
  hcsr04_start_task(&hcsr04_distance);

  buzzer_init(GPIO_NUM_18);
  buzzer_beep(500);
  // button_init();
  // bool have_wifi_credentials = wifi_check_credentials();

  // // ble_server_init();

  // if(have_wifi_credentials)
  // {
  //   ESP_LOGI(TAG, "Dane WiFi dostępne. Inicjalizacja stacji WiFi...");
  //   wifi_station_init();
  // }
  // else
  // {
  //   ESP_LOGW(TAG, "Brak danych WiFi. Uruchom tryb konfiguracji BLE.");
  // }

  int c;
  while ((c = fgetc(stdin)) != EOF)
  {
    // Ignore input during sensor startup
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }

  char input_line[128];

  while (1)
  {
    get_line_from_console(input_line, sizeof(input_line));

    if (strcmp(input_line, "read") == 0)
    {
      // Używamy funkcji z modułu
      char *content = storage_read_all();
      if (content)
      {
        printf("\n--- NOTATKI ---\n%s\n---------------\n", content);
        free(content);
      }
      else
      {
        printf(">> Pusto.\n");
      }
    }
    else if (strcmp(input_line, "free") == 0)
    {
      printf(">> Wolne: %zu bajtów\n", storage_get_free_space());
    }
    else if (strcmp(input_line, "clear") == 0)
    {
      storage_clear_all();
      printf(">> Wyczyszczono.\n");
    }
    else if (strcmp(input_line, "measurement") == 0)
    {

      bool valid = true;

      char line[128];
      snprintf(line, sizeof(line), "BMP280 LM: %.2f C\nVEML7700 LM: %.2f Lux\nMAX6675 LM: %.2f C\nHC-SR04 LM: %.2f cm\nADXL345 LM: %.2f m/s2\n",
               bmp280_temp, veml7700_illuminance, max6675_engine_temp, hcsr04_distance, adxl345_acceleration);
      printf("BMP280 LM: %.2f C \nVEML7700 LM: %.2f Lux\nMAX6675 LM: %.2f C\nHC-SR04 LM: %.2f cm\nADXL345 LM: %.2f m/s2\n",
             bmp280_temp, veml7700_illuminance, max6675_engine_temp, hcsr04_distance, adxl345_acceleration);
      if (storage_write_line(line))
      {
        printf(">> Zapisano.\n");
      }
    }
    else
    {
      if (storage_write_line(input_line))
      {
        printf(">> Zapisano.\n");
      }
      else
      {
        printf(">> Błąd zapisu/brak miejsca!\n");
      }
    }
  }
}