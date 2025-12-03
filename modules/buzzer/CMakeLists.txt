idf_component_register(
    SRCS "buzzer.c"
    INCLUDE_DIRS
        "."
    REQUIRES driver freertos esp_timer esp_driver_i2c
)