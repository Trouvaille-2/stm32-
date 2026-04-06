#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"

uint16_t Num;

int main(void)
{
    OLED_Init();       // 初始化OLED

		OLED_ShowString(1,1,"SYSCLK:");
		OLED_ShowNum(1,8,SystemCoreClock,8);

    while(1)
    {
		OLED_ShowString(2,1,"Running");
		Delay_ms(500);
		OLED_ShowString(2,1,"         ");
		Delay_ms(500);
    }
}