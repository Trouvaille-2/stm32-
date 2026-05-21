#include "spi.h"
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

spi_device_handle_t spi2_handle;

void spi2_init(void)
{
    spi_bus_config_t  spibus_structure = {
        .flags = SPICOMMON_BUSFLAG_MASTER,
        //.isr_cpu_id = INTR_CPU_ID_AUTO,
        .max_transfer_sz = 240 * 240 * 2,
        .miso_io_num = GPIO_NUM_13,
        .mosi_io_num = GPIO_NUM_11,
        .sclk_io_num = GPIO_NUM_12,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
    };
    spi_bus_initialize(SPI2_HOST,& spibus_structure, SPI_DMA_CH_AUTO);
}

uint8_t spi2_transfer_byte(uint8_t data)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));

    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    spi_device_polling_transmit(spi2_handle, &t);

    return t.rx_data[0];
}

void spi2_write_data(uint8_t *data, int len)
{
    spi_transaction_t t = {0};

    t.length = len * 8;                            
    t.tx_buffer = data;                            
    spi_device_polling_transmit(spi2_handle, &t);  
}