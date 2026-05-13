#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "key.h"
#include "led.h"

void app_main(void)
{
    uint8_t key_num = 0;
    key_init();
    led_init();

    while (1) {
        key_num = key_scan();

        if (key_num == 1) {
            gpio_toggle(GPIO_NUM_38);
        } else if (key_num == 2) {
            gpio_toggle(GPIO_NUM_39);
        } else if (key_num == 3) {
            gpio_toggle(GPIO_NUM_41);
        } else if (key_num == 4) {
            gpio_toggle(GPIO_NUM_42);
        }

        vTaskDelay(50);

        printf("KEY8:%d KEY9:%d KEY11:%d KEY12:%d\n", 
       gpio_get_level(GPIO_NUM_8),
       gpio_get_level(GPIO_NUM_9),
       gpio_get_level(GPIO_NUM_11),
       gpio_get_level(GPIO_NUM_12));
vTaskDelay(pdMS_TO_TICKS(200));
    }
}