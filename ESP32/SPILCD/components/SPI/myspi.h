#ifndef __MYSPI_H
#define __MYSPI_H

#include "driver/spi_master.h"
#include <stdint.h>

extern spi_device_handle_t spi2_handle;

void spi2_init(void);

uint8_t spi2_transfer_byte(uint8_t data);

void spi2_write_data(uint8_t *data,uint32_t len);

#endif