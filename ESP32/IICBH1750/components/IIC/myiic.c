#include "myiic.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

void iic_init()
{
    i2c_config_t conf = {
        .clk_flags = 0,
        .master.clk_speed = 100000,
        .mode = I2C_MODE_MASTER,
        .sda_io_num = GPIO_NUM_5,
        .scl_io_num = GPIO_NUM_4,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE
    };
    i2c_param_config(I2C_NUM_0,&conf);

    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}