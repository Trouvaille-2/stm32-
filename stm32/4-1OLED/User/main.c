#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "OLED_DATA.h"

int main(void)
{
	
	OLED_Init();
	OLED_ShowImage(0,0,92,64,Lenna);
	
	OLED_Update();
		while(1)
		{
		  
		}
	
		
		
}
	
