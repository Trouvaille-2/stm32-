#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "LED.h"
#include "exti.h"



void app_main(void)
{
    led_init(); 
    exti_init();
    while(1)
    {
    vTaskDelay(500);
    }
}