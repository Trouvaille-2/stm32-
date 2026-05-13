#include "beep.h"
#include "driver/gpio.h"

void beep_init(void)
{   
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = (1ULL << GPIO_NUM_41), // 蜂鸣器连接到 GPIO 5
        .mode =  GPIO_MODE_INPUT_OUTPUT, // 设置为输入输出模式
        .pull_up_en = GPIO_PULLUP_ENABLE, // 开启上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // 禁止下拉
        .intr_type = GPIO_INTR_DISABLE // 禁止中断
    };

    gpio_config(&gpio_cfg); // 配置 GPIO

    gpio_set_level(GPIO_NUM_41, 1); // 初始化为低电平，蜂鸣器关闭

    printf("Beep构建完成！\n");

}

