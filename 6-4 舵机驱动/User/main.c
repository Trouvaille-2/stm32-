#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "SERVE.h"
#include "KEY.h"


uint16_t i;
uint16_t Num;
uint16_t KeyNum;
float Angle;


int main(void)
{
    OLED_Init();       // 初始化OLED
		Serve_Init();
	  Key_Init();
	
	  OLED_ShowString(1,1,"Angle:");
		
		Serve_SetAngle(90);
    while(1)
    {
				KeyNum = Key_GetNum();
				if(KeyNum == 1)
				{
					Angle +=30;
					if(Angle>180)
					{
						Angle = 0;
					}
				}
				Serve_SetAngle(Angle);
				OLED_ShowNum(1,7,Angle,3);
    }
}