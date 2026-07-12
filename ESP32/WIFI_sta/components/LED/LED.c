#include "led.h"
#include "driver/gpio.h"

void led_init(void)
{   
    esp_err_t err;
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = (1ULL << GPIO_NUM_4),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    err = gpio_config(&gpio_cfg);
    if (err != ESP_OK) {
        printf("LED GPIO配置失败: %s\n", esp_err_to_name(err));
    } else {
        printf("LED GPIO%d 初始化完成\n", GPIO_NUM_4);
    }

}

void gpio_toggle(gpio_num_t gpio_num)
{
    if(gpio_get_level(gpio_num) == 0)
        gpio_set_level(gpio_num, 1); // 输出高电平，点亮LED
    else
        gpio_set_level(gpio_num, 0); // 输出低电平，熄灭LED
    
}