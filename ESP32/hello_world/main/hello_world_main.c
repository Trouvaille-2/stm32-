

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


void app_main(void)
{
    while(1)
    {
        printf("Hello world!\n");
        vTaskDelay(500);
    }
}
