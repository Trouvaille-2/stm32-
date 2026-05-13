#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "LED.h"



void app_main(void)
{
    led_init(); 

    while(1)
    {
        gpio_toggle(GPIO_NUM_4); // 切换LED状态
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}