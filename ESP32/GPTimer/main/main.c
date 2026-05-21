#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "lcd.h"



void app_main(void)
{
    lcd_init(); 
    lcd_clear(BLACK);
    lcd_show_char(1,1,'A',RED,BLACK);
    lcd_show_string(2,1,"Hello, ESP32!",GREEN,BLACK);
    lcd_show_num(3,1,12345678,8,YELLOW,BLACK);
    lcd_show_hexnum(4,1,0x1234,4,BROWN,BLACK);
    lcd_show_float(5,1,3.14159,2,CYAN,BLACK);
    while(1)
    {
        vTaskDelay(500);
    }
}