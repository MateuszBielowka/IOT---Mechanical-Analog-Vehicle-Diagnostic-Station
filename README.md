# ESP32 – WiFi Station + HTTP Client

Projekt realizuje następujące funkcje:
- Łączy się z wybraną siecią WiFi.
- Reaguje na utratę połączenia (migająca dioda LED).
- Próbuje ponownie połączyć się z WiFi.
- Po uzyskaniu połączenia pobiera dane z serwera HTTP.

---

## Struktura folderów

```plaintext
iot_project/
│
├── CMakeLists.txt
├── sdkconfig
├── README.md
│
├── include/
│   └── project_config.h
│   
├── main/
│   ├── main.c
│   ├── app_main.c            → logika główna modułu diagnostycznego
│   ├── app_main.h
│   └── CMakeLists.txt
│
├── modules/
│   │
│   ├── wifi_station/
│   │   ├── wifi_station.c
│   │   ├── wifi_station.h
│   │   └── CMakeLists.txt
│   │
│   ├── http_client/          → wysyłanie danych REST (jeśli używasz)
│   │   ├── http_client.c
│   │   ├── http_client.h
│   │   └── CMakeLists.txt
│   │
│   ├── mqtt_client/          → MQTT do wysyłania buforów
│   │   ├── mqtt_client_app.c
│   │   ├── mqtt_client_app.h
│   │   └── CMakeLists.txt
│   │
│   ├── sensors/
│   │   ├── max6675.c
│   │   ├── max6675.h
│   │   ├── bme280.c
│   │   ├── bme280.h
│   │   ├── hcsr04.c
│   │   ├── hcsr04.h
│   │   ├── adxl345.c
│   │   ├── adxl345.h
│   │   ├── light_veml7700.c
│   │   ├── light_veml7700.h
│   │   └── CMakeLists.txt
│   │
│   ├── storage/              → buforowanie danych: NVS / FATFS / SPIFFS
│   │   ├── storage.c
│   │   ├── storage.h
│   │   └── CMakeLists.txt
│   │
│   ├── ble_service/
│   │   ├── ble_service.c
│   │   ├── ble_service.h
│   │   └── CMakeLists.txt
│   │
│   ├── ui_led/
│   │   ├── status_led.c
│   │   ├── status_led.h
│   │   └── CMakeLists.txt
│
├── *components/
├── *managed_components/
└── *build/

---

### Uwagi

1. Każdy moduł w `modules/` powinien mieć własny `CMakeLists.txt` z poprawnie zadeklarowanymi zależnościami (`REQUIRES` lub `PRIV_REQUIRES`).
2. `sdkconfig` i `include/project_config.h` przechowują ustawienia konfiguracyjne projektu (domyślne SSID, hasło WiFi, porty HTTP, GPIO dla LED).
3. Foldery `build/`, `components/` i `managed_components/` są tworzone przez system budowania ESP-IDF i nie powinny być modyfikowane ręcznie.
4. Działanie całego systemu jak i pojedynczych komponentów będą przedstawiać diagramy czynności w poniższym linku:
[Draw.io](https://drive.google.com/file/d/1hRFMKYafspgwK1co53UuYx3lU7xpVcHV/view?usp=sharing)

---



---
