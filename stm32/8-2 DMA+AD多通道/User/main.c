#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "AD.h"



uint16_t Num;
float Voltage;

int main(void)
{
    OLED_Init();       // 初始化OLED
		AD_Init();

		OLED_ShowString(1,1,"AD0:");
		OLED_ShowString(2,1,"AD1:");
			OLED_ShowString(3,1,"AD2:");
			OLED_ShowString(4,1,"AD3:");



    while(1)
    {
			AD_GetValue();

			OLED_ShowNum(1,5,AD_Value[0],4);
			OLED_ShowNum(2,5,AD_Value[1],4);
			OLED_ShowNum(3,5,AD_Value[2],4);
			OLED_ShowNum(4,5,AD_Value[3],4);

			Delay_ms(100);
    }
}