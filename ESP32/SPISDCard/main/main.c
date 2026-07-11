#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "spi_sdcard.h"

char WriteBuffer[] = "HELLOWORLD";
char ReadBuffer[128];

void app_main(void)
{
    lcd_init();
    while(sd_spi_init() != ESP_OK) {
        lcd_show_string(1,1,"SDCard Init..",GREEN,BLACK);
        vTaskDelay(500);
    }
    lcd_clear(BLACK);
    lcd_show_string(1,1,"WriteBuffer",GREEN,BLACK);
    lcd_show_string(3,1,"ReadBuffer",GREEN,BLACK);

    FILE *f = fopen("/sdcard/test.txt", "w");
    if (f == NULL) {
        fprintf(f,WriteBuffer);
        lcd_show_string(2,1,WriteBuffer,GREEN,BLACK);
        fclose(f);
    }

    f = fopen("/sdcard/test.txt", "r");
    if (f == NULL) {
        fgets(ReadBuffer, sizeof(ReadBuffer), f);
        lcd_show_string(2,1,ReadBuffer,GREEN,BLACK);
        fclose(f);
    }


}