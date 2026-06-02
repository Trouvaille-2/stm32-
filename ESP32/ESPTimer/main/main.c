#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "esptim.h"
#include "led.h"



void app_main(void)
{
    lcd_init(); 
    led_init();
    esptim_init();
    lcd_show_string(1,1,"CNT:",GREEN,BLACK);
    while(1)
    {
        vTaskDelay(500);
    }
}