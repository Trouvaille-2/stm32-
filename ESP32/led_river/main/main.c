#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "LED.h"



void app_main(void)
{
    uint8_t i=0;
    led_init(); 

    while(1)
    {
        i++;
        if(i==1)
        {
            gpio_set_level(GPIO_NUM_38, 0);
            gpio_set_level(GPIO_NUM_39, 1);
            gpio_set_level(GPIO_NUM_41, 1);
            gpio_set_level(GPIO_NUM_42, 1);
        }
        else if(i==2)
        {
            gpio_set_level(GPIO_NUM_38, 1);
            gpio_set_level(GPIO_NUM_39, 0);
            gpio_set_level(GPIO_NUM_41, 1);
            gpio_set_level(GPIO_NUM_42, 1);
        }
        else if(i==3)
        {
            gpio_set_level(GPIO_NUM_38, 1);
            gpio_set_level(GPIO_NUM_39, 1);
            gpio_set_level(GPIO_NUM_41, 0);
            gpio_set_level(GPIO_NUM_42, 1);
        }
        else if(i==4)
        {
            gpio_set_level(GPIO_NUM_38, 1);
            gpio_set_level(GPIO_NUM_39, 1);
            gpio_set_level(GPIO_NUM_41, 1);
            gpio_set_level(GPIO_NUM_42, 0);
        }
           
        else if(i==5)
        {
            i=0;
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}