idf_component_register(
    SRCS "button.c"
    INCLUDE_DIRS "."
    REQUIRES bt ble_service driver esp_timer log freertos
)