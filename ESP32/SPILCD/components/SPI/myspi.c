#include "myspi.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

spi_device_handle_t spi2_handle;

void spi2_init(void)
{
    spi_bus_config_t spibus_structure = {
        .flags = SPICOMMON_BUSFLAG_MASTER,
        .intr_flags = 0,
        .isr_cpu_id = ESP_INTR_CPU_AFFINITY_AUTO,
        .max_transfer_sz = 240*240*2,
        .miso_io_num = GPIO_NUM_13,
        .mosi_io_num = GPIO_NUM_11,
        .quadhd_io_num = -1,
        .quadwp_io_num = -1,
        .sclk_io_num = GPIO_NUM_12,
        
        
    };
    spi_bus_initialize(SPI2_HOST,&spibus_structure,SPI_DMA_CH_AUTO);

    spi_device_interface_config_t spidevice_structure = {
        .clock_source = SPI_CLK_SRC_DEFAULT,
        .clock_speed_hz = 60000000,
        .mode = 0,
        .queue_size = 7,
        .spics_io_num = GPIO_NUM_48,
    };
    spi_bus_add_device(SPI2_HOST,&spidevice_structure,&spi2_handle);
}

uint8_t spi2_transfer_byte(uint8_t data)
{
    spi_transaction_t t = {0};
    t.length = data*8;
    t.tx_data[0] = data;
    spi_device_polling_transmit(spi2_handle,&t);
    return t.rx_data[0];
}

uint8_t array[100];

void spi2_write_data(uint8_t *data,uint32_t len)
{
    spi_transaction_t t = {0};
    t.length = len*8;
    t.tx_buffer = data;
    spi_device_polling_transmit(spi2_handle,&t);
}