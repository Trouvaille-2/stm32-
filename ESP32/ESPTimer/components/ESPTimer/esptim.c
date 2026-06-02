#include "esptim.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "led.h"
#include "lcd.h"

uint8_t lcd_cnt = 0;

void esptimer_callback(void* arg)
{
    lcd_cnt++;
    lcd_show_string(1,1,"CNT:",GREEN,BLACK);
    lcd_show_num(1,5,lcd_cnt,3,GREEN,BLACK);
    gpio_toggle(GPIO_NUM_38); 
    
}

void esptim_init()
{
    esp_timer_handle_t esptim;

    esp_timer_create_args_t esptim_cfg = {
        .arg = NULL,
        .callback = &esptimer_callback, // 定时器回调函数
        .dispatch_method = ESP_TIMER_TASK, // 定时器调度方法
        .name = "mytim",
        .skip_unhandled_events = true 
};

    esp_timer_create(&esptim_cfg, &esptim); // 创建定时器

    esp_timer_start_periodic(esptim, 500000); // 启动定时器，周期为1秒（1000000微秒）
}