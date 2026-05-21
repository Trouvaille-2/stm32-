#include "exti.h"
#include "LED.h"
#include "driver/gpio.h"

void  exti_isr_handler(void* arg)
{
    gpio_toggle(GPIO_NUM_38);
}

void exti_init(void)
{
    gpio_config_t io_conf={
        .intr_type=GPIO_INTR_NEGEDGE,
        .mode=GPIO_MODE_INPUT,
        .pin_bit_mask=(1ULL<<GPIO_NUM_9),
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_ENABLE
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);

    gpio_isr_handler_add(GPIO_NUM_9, exti_isr_handler, NULL);

    gpio_intr_enable(GPIO_NUM_9);
}