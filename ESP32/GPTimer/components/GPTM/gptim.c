#include "gptim.h"
#include "driver/gptimer.h"
#include "esp_attr.h"

gptimer_handle_t gptim;
uint8_t flag_timer = 0;

bool TimerCallback (gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    flag_timer = 1;
    return 0;
}

void gptim_init(void)
{
    gptimer_config_t gptimer_cfg = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .flags = {0}, // 只改这一句！！！
        .intr_priority = 0,
        .resolution_hz = 1000000,
    };
    gptimer_new_timer(&gptimer_cfg, &gptim);

    gptimer_alarm_config_t gptimer_alarm_cfg = {
        .alarm_count = 500000,
        .reload_count = 0,
        .flags = {0}, // 只改这一句！！！
    };
    gptimer_set_alarm_action(gptim, &gptimer_alarm_cfg);

    gptimer_event_callbacks_t event_cfg = {
        .on_alarm = TimerCallback,
    };
    gptimer_register_event_callbacks(gptim, &event_cfg, NULL);

    gptimer_enable(gptim);
    gptimer_start(gptim);
}