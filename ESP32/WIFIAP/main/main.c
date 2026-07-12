#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "wifiap.h"
#include "nvs_flash.h"



void app_main(void)
{
    lcd_init(); 
    nvs_flash_init();
    wifi_init_ap();
    while(1)
    {
        vTaskDelay(500);
    }
}