#include "pwm.h"
#include "driver/ledc.h"

void pwm_init()
{
    ledc_timer_config_t ledctimer_cfg={
        .clk_cfg=LEDC_AUTO_CLK,//配置定时器的时钟源
        .duty_resolution=LEDC_TIMER_10_BIT,//设置分辨率
        .freq_hz=1000,//脉冲频率
        .speed_mode=LEDC_LOW_SPEED_MODE,
        .timer_num=LEDC_TIMER_1
    };
    ledc_timer_config(&ledctimer_cfg);


    ledc_channel_config_t ledcchannel_cfg={
        .channel=LEDC_CHANNEL_1,//pwm通道
        .duty=512,//高电平所占用的分割数量
        .flags={.output_invert=0},
        .gpio_num=GPIO_NUM_38,//pwm输出引脚
        .hpoint=0,
        .intr_type=LEDC_INTR_DISABLE,//不使用中断
        .speed_mode=LEDC_LOW_SPEED_MODE,//选择低速模式
        .timer_sel=LEDC_TIMER_1//选择定时器1
    };

    ledc_channel_config(&ledcchannel_cfg);
}

void duty_set(uint16_t duty)
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1,duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE,LEDC_CHANNEL_1);
}
