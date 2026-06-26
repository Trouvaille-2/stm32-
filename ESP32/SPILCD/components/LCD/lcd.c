#include "lcd.h"
#include "myspi.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

uint8_t lcd_buf[115200];

void lcd_write_cmd(uint8_t cmd)
{
    LCD_DC(0);
    spi2_write_data(&cmd,1);
}

void lcd_write_data(uint8_t data)
{
    LCD_DC(1);
    spi2_write_data(&data,1);
}

void lcd_write_data16(uint16_t data)
{
    uint8_t databuf[2] = {0,0};
    databuf[0] = data >>8;
    databuf[1] = data & 0xFF;
    LCD_DC(1);
    spi2_write_data(databuf,2);
}

void lcd_write_datan(uint8_t *data,uint16_t length)
{
    LCD_DC(1);
    spi2_write_data(data,length);
}

void lcd_hard_reset(void)
{
    LCD_RST(0);
    vTaskDelay(100);
    LCD_RST(1);
    vTaskDelay(100);
}

void lcd_on(void)
{
    LCD_BLK(1);
    vTaskDelay(10);
}

void lcd_off(void)
{
    LCD_BLK(0);
    vTaskDelay(10);
}

void lcd_init(void)
{
    spi2_init();

    gpio_config_t gpio_structure = {0};
    gpio_structure.intr_type = GPIO_INTR_DISABLE;
    gpio_structure.mode = GPIO_MODE_OUTPUT;
    gpio_structure.pin_bit_mask = 1ULL << GPIO_NUM_21;
    gpio_structure.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_structure.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&gpio_structure);

    
    gpio_structure.intr_type = GPIO_INTR_DISABLE;
    gpio_structure.mode = GPIO_MODE_OUTPUT;
    gpio_structure.pin_bit_mask = 1ULL << GPIO_NUM_47;
    gpio_structure.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_structure.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&gpio_structure);

    
    gpio_structure.intr_type = GPIO_INTR_DISABLE;
    gpio_structure.mode = GPIO_MODE_OUTPUT;
    gpio_structure.pin_bit_mask = 1ULL << GPIO_NUM_40;
    gpio_structure.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_structure.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&gpio_structure);

    lcd_hard_reset();
    lcd_on();
    vTaskDelay(100);

    lcd_write_cmd(0x11);
    vTaskDelay(120);

    lcd_write_cmd(0xB2);
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    lcd_write_cmd(0x35);
    lcd_write_data(0x00);

    lcd_write_cmd(0x36);
    lcd_write_data(0x70);
    lcd_write_data(0xA0);

    lcd_write_cmd(0x3A);
    lcd_write_data(0x05);

    lcd_write_cmd(0xB7);
    lcd_write_data(0x35);

    lcd_write_cmd(0xBB);
    lcd_write_data(0x2D);

    lcd_write_cmd(0xC0);
    lcd_write_data(0x2C);

    lcd_write_cmd(0xC2);
    lcd_write_data(0x01);

    lcd_write_cmd(0xC3);
    lcd_write_data(0x15);

    lcd_write_cmd(0xC4);
    lcd_write_data(0x20);

    lcd_write_cmd(0xC6);
    lcd_write_data(0x0F);

    lcd_write_cmd(0xD0);
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xD6);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xE0);
    lcd_write_data(0x70);
    lcd_write_data(0x05);
    lcd_write_data(0x0A);
    lcd_write_data(0x0B);
    lcd_write_data(0x0A);
    lcd_write_data(0x27);
    lcd_write_data(0x2F);
    lcd_write_data(0x44);
    lcd_write_data(0x47);
    lcd_write_data(0x37);
    lcd_write_data(0x14);
    lcd_write_data(0x14);
    lcd_write_data(0x29);
    lcd_write_data(0x2F);
    
    lcd_write_cmd(0xE1);
    lcd_write_data(0x70);
    lcd_write_data(0x07);
    lcd_write_data(0x0C);
    lcd_write_data(0x08);
    lcd_write_data(0x08);
    lcd_write_data(0x04);
    lcd_write_data(0x2F);
    lcd_write_data(0x33);
    lcd_write_data(0x46);
    lcd_write_data(0x18);
    lcd_write_data(0x15);
    lcd_write_data(0x15);
    lcd_write_data(0x2B);
    lcd_write_data(0x2D);

    lcd_write_cmd(0x21);
    lcd_write_cmd(0x29);
    lcd_write_cmd(0x2C);

    lcd_clear(WHITE);
}

void lcd_set_window(uint16_t xstar,uint16_t ystar,uint16_t xend,uint16_t yend)
{
    lcd_write_cmd(0x2a);
    lcd_write_data16(xstar);
    lcd_write_data16(xend);

    lcd_write_cmd(0x2b);
    lcd_write_data16(ystar);
    lcd_write_data16(yend);
}

void lcd_show_dot(uint16_t color)
{
    lcd_set_window(100,100,100,100);
    lcd_write_data16(color);
}

void lcd_show_line(uint16_t color)
{
    uint8_t data[2]={0,0};
    data[0]=color>>8;
    data[1]=color & 0xFF;
    lcd_set_window(0,99,10,10);

    for(uint16_t i=0;i<100;i++)
    {
        lcd_buf[2*i] = data[0];
        lcd_buf[2*i+1] = data[i];
    }
    lcd_write_datan(lcd_buf,100);
}

void lcd_clear(uint16_t color)
{
    uint8_t data[2]={0,0};
    data[0]=color>>8;
    data[1]=color & 0xFF;
    lcd_set_window(0,0,239,239);

    for(uint16_t i=0;i<240*240/10;i++)
    {
        lcd_buf[2*i] = data[0];
        lcd_buf[2*i+1] = data[i];
    }
    for(uint16_t j=0;j<10;j++)
    {
        lcd_write_datan(lcd_buf,11520);
    }
    
}

void lcd_show_picture(uint8_t *img)
{
    unsigned long i=0;
    unsigned long j=0;
    lcd_set_window(0,0,239,239);
    for( i=0;i<240*240/10;i++)
    {
        lcd_buf[2*i] = img[2*i];
        lcd_buf[2*i+1] =img[2*i+1];
    }
    for( j=0;j<(240*240*2/11520);j++)
    {
        lcd_write_datan(&lcd_buf[j*11520],11520);
    }
}