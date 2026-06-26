#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "LED.h"
#include "LCD.h"
#include "myspi.h"



void app_main(void)
{
    lcd_init(); 
    lcd_show_line(RED);

    while(1)
    {
        vTaskDelay(500);
    }
}