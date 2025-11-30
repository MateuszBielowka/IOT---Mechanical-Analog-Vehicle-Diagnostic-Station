#include "nvs_flash.h" // pamiec flash
#include "esp_log.h"   // logowanie wiadomosci

// importy
#include "wifi_station.h"
#include "status_led.h"
#include "http_client.h"

#include "hcsr04.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bmp280.h"

// tag do logowania w konsoli
static const char *TAG = "app_main";

//stałe do zastąpienia funkcjami i zmiennymi
#define BLE_PAIRED_SUCCESS true
#define MEMORY_USAGE_PERCENT 75
#define WIFI_STATION_CHECK_CREDETIALS true
#define WIFI_CONNECTED_SUCCESS true


void app_main(void)
{
  //Init storage test if all sensors work
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


  //Try BLE Pairing


  if (BLE_PAIRED_SUCCESS == true)
  {
    ESP_LOGI(TAG, "BLE Paired Success");
    if (MEMORY_USAGE_PERCENT < 90) {
      ESP_LOGI(TAG, "Memory Usage is acceptable: %d%%", MEMORY_USAGE_PERCENT);
    }
    else {
      ESP_LOGW(TAG, "Memory Usage is too high: %d%%", MEMORY_USAGE_PERCENT);
      //Check WIFI Credentials
      if(WIFI_STATION_CHECK_CREDETIALS == false) {
        ESP_LOGE(TAG, "Wi-Fi credentials are invalid. Please reconfigure via BLE.");
        //Send BLE Alert about high memory usage
        //Wait for new credentials via BLE
      }
      //Attempt to connect with credetials

      if(WIFI_CONNECTED_SUCCESS == true) {
        ESP_LOGI(TAG, "Wi-Fi connected successfully.");
      }
      else {
        ESP_LOGE(TAG, "Wi-Fi connection failed. Please reconfigure via BLE.");
        //Send BLE Alert about high memory usage
        //Wait for new credentials via BLE
      }
      
    }
  }
  else
  {
    ESP_LOGI(TAG, "BLE Paired Failed");
  }

  
  
  //----Test HC-SR04----
  // hcsr04_regular_measurments();
  //--------------------
  //----Test BMP280----
  xTaskCreate(bmp280_task, "BMP280_Task", 4096, NULL, 10, NULL);


  wifi_station_init();     // watek obslugujacy wi-fi

  ESP_LOGI(TAG, "Pierwsze połączenie Wi-Fi nawiązane. Uruchamiam klienta HTTP...");

  http_client_start_task(); // watek klienta HTTP
}