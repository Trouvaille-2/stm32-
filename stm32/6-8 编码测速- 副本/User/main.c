#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "encod.h"

uint16_t Num;
int16_t Speed;

int main(void)
{
    OLED_Init();       // 初始化OLED
	  //Timer_Init();
		Encod_Init();
	  OLED_ShowString(1, 1, "CNT:");

    while(1)
    {
			OLED_ShowSignedNum(1,5,Encod_Get(),5);
			Delay_ms(1000);
    }
}