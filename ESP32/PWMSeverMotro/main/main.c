#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "motor.h"
#include "pwm.h"
#include "key.h"

uint8_t current_angle = 0;
uint8_t key_num=0;

void app_main(void)
{
    lcd_init(); 
    key_init();
    pwm_init();
    
    lcd_show_string(1,1,"Angle: 0",GREEN,BLACK);
    set_angle(0);
    while(1)
    {
        key_num=key_scan();
        if(key_num==1)
        {
            if(current_angle<180) current_angle+=45;
            set_angle(current_angle);
        }
        else if(key_num==2)
        {
           if(current_angle>0) current_angle-=45;
            set_angle(current_angle);
        }
        lcd_show_num(1,7,current_angle,3,YELLOW,BLACK);
        vTaskDelay(5);
    }
}