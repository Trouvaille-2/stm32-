#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "beep.h"



void app_main(void)
{
    beep_init(); 
    while(1)
    {
        // 先获取当前电平，然后取反，再设置回去 → 等于翻转
    gpio_set_level(GPIO_NUM_41, !gpio_get_level(GPIO_NUM_41)); // 切换 GPIO 41 的电平状态
        vTaskDelay(500 / portTICK_PERIOD_MS); // 延迟 500 ms
    }
}