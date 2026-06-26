#include "lcd.h"
#include "lcdfont.h"
#include "spi.h"
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
    databuf[0] = data >> 8 ;
    databuf[1] = data & 0xFF ;
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

void lcd_set_window(uint16_t xstar, uint16_t ystar,uint16_t xend,uint16_t yend)
{	
    lcd_write_cmd(0x2a);
    lcd_write_data16(xstar);
    lcd_write_data16(xend);
    lcd_write_cmd(0x2b);
    lcd_write_data16(ystar);
    lcd_write_data16(yend);
    lcd_write_cmd(0x2c);
} 

void lcd_clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;
    
    lcd_set_window(0, 0, 239, 239);

    for(j = 0; j < 115200/10/2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    for(i = 0; i < 10; i++)
    {
        lcd_write_datan(lcd_buf, 11520);
    }
}

void lcd_init(void)
{
    spi2_init();

    spi_device_interface_config_t   spidevice_structure = {0};
    spidevice_structure.clock_source = SPI_CLK_SRC_DEFAULT;
    spidevice_structure.clock_speed_hz = 60000000;
    spidevice_structure.mode = 0;
    spidevice_structure.queue_size = 7;
    spidevice_structure.spics_io_num = GPIO_NUM_48;
    spi_bus_add_device(SPI2_HOST, &spidevice_structure, &spi2_handle);

    gpio_config_t gpio_init_struct;
        /* WR管脚 */
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;                       /* 配置输出模式 */
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_NUM_47 ;           /* 配置引脚位掩码 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* 失能下拉 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;               /* 使能上拉 */
    gpio_config(&gpio_init_struct);                                 /* 引脚配置 */
    /* BL管脚 */
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;                       /* 配置输出模式 */
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_NUM_40;            /* 配置引脚位掩码 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_ENABLE;           /* 使能下拉 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_DISABLE;              /* 失能上拉 */
    gpio_config(&gpio_init_struct);                                 /* 引脚配置 */
    /* RST管脚 */
    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* 失能引脚中断 */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;                       /* 配置输出模式 */
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_NUM_21;           /* 配置引脚位掩码 */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* 失能下拉 */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;               /* 使能上拉 */
    gpio_config(&gpio_init_struct);                                 /* 引脚配置 */

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

    lcd_clear(BLACK);
}

void lcd_set_cursor(uint16_t xpos, uint16_t ypos)
{
    lcd_set_window(xpos,ypos,xpos,ypos);	
} 

void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_cursor(x, y);
    lcd_write_data16(color);
}

void lcd_show_char(uint8_t line,uint8_t column,uint8_t chr,uint16_t fontcolor,uint16_t backgroundcolor)
{
    uint8_t i , j = 0 ;
    uint8_t chr_index = 0 ;
    uint8_t chr_temp = 0 ;
    lcd_set_window( (column - 1) * 16 , (line - 1) * 32 + 8 ,column * 16 - 1 , line * 32 + 7 );
    for( i = 0 ; i < 64 ; i++ )
    {
        chr_temp = ascii_3216[chr - ' '][i];
        for( j = 0 ; j < 8 ; j++ )
        {
            if( chr_temp & ( 0x01 << j ) )
            {
                lcd_write_data16(fontcolor);
            }
            else
            {
                lcd_write_data16(backgroundcolor);
            }
            chr_index++;
            if( chr_index == 16 )
            {
                chr_index = 0 ;
                break;
            }
        }
    }
}

void lcd_show_string(uint8_t line,uint8_t column,char *string,uint16_t fontcolor,uint16_t backgroundcolor)
{
    uint8_t i = 0 ;
    for( i = 0 ; string[i] != '\0' ; i++ )
    {
        lcd_show_char( line , column + i , string[i] , fontcolor , backgroundcolor);
    }
}

uint32_t lcd_pow(uint32_t x, uint32_t y)
{
	uint32_t Result = 1;
	while (y--)
	{
		Result *= x;
	}
	return Result;
}

void lcd_show_num(uint8_t line,uint8_t column,uint32_t number,uint8_t length,uint16_t fontcolor,uint16_t backgroundcolor)
{
	uint8_t i;
	for (i = 0; i < length; i++)							
	{
		lcd_show_char(line, column + i, number / lcd_pow(10, length - i - 1) % 10 + '0',fontcolor,backgroundcolor);
	}    
}

void lcd_show_hexnum(uint8_t line, uint8_t column, uint32_t number, uint8_t length,uint16_t fontcolor,uint16_t backgroundcolor)
{
	uint8_t i, singlenumber;
	for (i = 0; i < length; i++)							
	{
		singlenumber = number / lcd_pow(16, length - i - 1) % 16;
		if (singlenumber < 10)
		{
			lcd_show_char(line, column + i, singlenumber + '0',fontcolor,backgroundcolor);
		}
		else
		{
			lcd_show_char(line, column + i, singlenumber - 10 + 'A',fontcolor,backgroundcolor);
		}
	}
}

void lcd_show_float(uint8_t line, uint8_t column, float number, uint8_t length,uint16_t fontcolor,uint16_t backgroundcolor)
{
	uint8_t i;
    uint32_t temp;
    uint32_t number1 = number * 100;
    for( i = 0 ; i < length ; i ++ )
    {
        temp = ( number1 / lcd_pow( 10 , length - i - 1) ) % 10 ;
        if( i == ( length - 2 ) )
        {
            lcd_show_char( line , column + length - 2 , '.' , fontcolor , backgroundcolor );
            i++;
            length += 1;
        }
        lcd_show_num( line , column + i , temp , 1 , fontcolor , backgroundcolor);
    }
}

void lcd_show_picture(uint8_t *img)
{
    unsigned long i = 0;
    unsigned long j = 0;
    lcd_set_window(0, 0, 239, 239);
    for (j = 0; j < 240 * 240; j++)
    {
        lcd_buf[2 * j] = img[2 * i] ;
        lcd_buf[2 * j + 1] =  img[2 * i + 1];
        i ++;
    }
    
    for(j = 0; j < (240 * 240 * 2 / 11520); j++)
    {
        lcd_write_datan(&lcd_buf[j * 11520] , 11520);
    }   
}