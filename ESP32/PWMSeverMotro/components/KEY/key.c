#include "key.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdio.h>

void key_init(void)
{
    gpio_config_t gpio_cfg = {
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << GPIO_NUM_9) | (1ULL << GPIO_NUM_10) | (1ULL << GPIO_NUM_11) | (1ULL << GPIO_NUM_12)
    };
    gpio_config(&gpio_cfg);
}

uint8_t key_scan(void)
{
    uint8_t key_num = 0;

    if (gpio_get_level(GPIO_NUM_9) == 0) {
        vTaskDelay(pdMS_TO_TICKS(20));
        while (gpio_get_level(GPIO_NUM_9) == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(20));
        key_num = 1;
    }

    if (gpio_get_level(GPIO_NUM_8) == 0) {
        vTaskDelay(pdMS_TO_TICKS(20));
        while (gpio_get_level(GPIO_NUM_8) == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(20));
        key_num = 2;
    }

    if (gpio_get_level(GPIO_NUM_11) == 0) {
        vTaskDelay(pdMS_TO_TICKS(20));
        while (gpio_get_level(GPIO_NUM_11) == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(20));
        key_num = 3;
    }

    if (gpio_get_level(GPIO_NUM_12) == 0) {
        vTaskDelay(pdMS_TO_TICKS(20));
        while (gpio_get_level(GPIO_NUM_12) == 0) {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        vTaskDelay(pdMS_TO_TICKS(20));
        key_num = 4;
    }

    return key_num;
}