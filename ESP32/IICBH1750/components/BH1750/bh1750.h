#ifndef __BH1750_H
#define __BH1750_H

#include <stdint.h>

#define bh1750_write_addr 0x46
#define bh1750_read_addr 0x47

#define BH1750_POWER_DOWN 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_HResolutionMode 0x10

void bh1750_send_cmd(uint8_t cmd_data);

uint16_t bh1750_read_data();

void bh1750_init(void);

#endif