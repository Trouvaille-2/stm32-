#include "myuart.h"
#include "driver/uart.h"
#include "driver/gpio.h"

void myuart_init()
{
    uart_config_t uart_config = {
        .baud_rate = 9600,//波特率
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
        .rx_flow_ctrl_thresh = 100
    };
    uart_param_config(UART_NUM_1, &uart_config);

    uart_set_pin(UART_NUM_1, GPIO_NUM_17, GPIO_NUM_18, -1, -1);

    uart_driver_install(UART_NUM_1,1024,1024,0,NULL,0);
}