#include "bh1750.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "myiic.h"

void bh1750_send_cmd(uint8_t cmd_data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, bh1750_write_addr, true);
    i2c_master_write_byte(cmd, cmd_data, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000);
    i2c_cmd_link_delete(cmd);
}

uint16_t bh1750_read_data(void)
{
    uint8_t light_high = 0,light_low = 0;
    uint16_t light_value = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, bh1750_read_addr, true);
    i2c_master_read(cmd, &light_high, 1, I2C_MASTER_ACK);
    i2c_master_read(cmd, &light_low, 1, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000);
    i2c_cmd_link_delete(cmd);
    light_value = ((uint16_t)light_high << 8) | light_low;
    return light_value;
};

void bh1750_init(void)
{
    iic_init();
    bh1750_send_cmd(BH1750_POWER_ON);
    bh1750_send_cmd(BH1750_HResolutionMode);
    vTaskDelay(200);
}