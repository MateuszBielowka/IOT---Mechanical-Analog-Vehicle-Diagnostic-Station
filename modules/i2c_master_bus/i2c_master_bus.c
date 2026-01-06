#include "i2c_master_bus.h"

const char *TAG = "I2C_MASTER_BUS";

void i2c_initialize_master(i2c_port_t port, int sda, int scl, uint32_t freq, i2c_master_bus_handle_t *bus_handle)
{
    i2c_master_bus_config_t i2c_bus_config = {
        .i2c_port = -1,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, bus_handle));
    ESP_LOGI(TAG, "I2C master bus created");
}