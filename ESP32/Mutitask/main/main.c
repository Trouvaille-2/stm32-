#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "key.h"
#include "led.h"

    void led_task(void *pvParameters)
    {
        while (1) {
            gpio_toggle(GPIO_NUM_38);
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
    
    void key_task(void *pvParameters)
    {
        uint8_t key_num = 0;
        while (1) {
            key_num = key_scan();
            if (key_num == 1) {
                gpio_toggle(GPIO_NUM_39);
            }
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

void app_main(void)
{

    
    key_init();
    led_init();
    xTaskCreate(led_task, "led_task", 2048, NULL, 5, NULL);
    xTaskCreate(key_task, "key_task", 2048, NULL, 5, NULL);
}