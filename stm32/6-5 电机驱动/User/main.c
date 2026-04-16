#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Moter.h"
#include "Key.h"

int8_t Speed;
uint16_t Num;
uint16_t KeyNum;



int main(void)
{
    OLED_Init();       // 初始化OLED
		Moter_Init();
		Key_Init();
	
		OLED_ShowString(1,1,"Speed:");
	   while(1)
    {
			KeyNum = Key_GetNum();
			if(KeyNum == 1)
			{
				Speed += 20;
				if(Speed>100)
				{
					Speed = -100;
				}
			}
			Moter_SetSpeed(Speed);
			OLED_ShowSignedNum(1,7,Speed,3);
    }
}