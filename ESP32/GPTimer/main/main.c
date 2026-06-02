#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "GPTM/gptim.h"
#include "led.h"



void app_main(void)
{
    uint8_t lcd_cnt = 0;
    lcd_init(); 
    led_init();
    gptim_init();
    lcd_show_string(1,1,"CNT:",GREEN,BLACK);
    while(1)
    {
        if(flag_timer == 1)
        {
            lcd_cnt++;
            lcd_show_num(1,5,lcd_cnt,3,GREEN,BLACK);
            gpio_toggle(GPIO_NUM_38);
            flag_timer = 0;
        }
        vTaskDelay(1);
    }
}