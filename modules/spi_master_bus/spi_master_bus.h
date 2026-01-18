#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define SPI_PORT_0_MISO_PIN 12
#define SPI_PORT_0_MOSI_PIN -1
#define SPI_PORT_0_SCK_PIN 14
#define MAX_TRANSFER_SIZE 0

esp_err_t spi_initialize_master();