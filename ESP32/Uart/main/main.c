#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "LED.h"
#include "myuart.h"
#include "driver/uart.h"



void app_main(void)
{
    int length =0;
    uint8_t receive_data[1024]={0};
    led_init(); 
    myuart_init();
    while(1)
    {
        length = uart_read_bytes(UART_NUM_1, receive_data, 1024, 100 );
        if(length > 0)
        {
            uart_write_bytes(UART_NUM_1, receive_data,length);
            uart_flush(UART_NUM_1);
        }
        vTaskDelay(5);
    }
}