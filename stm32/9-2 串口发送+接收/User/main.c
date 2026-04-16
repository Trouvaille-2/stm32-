#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

uint16_t RxData;
uint16_t Num;


int main(void)
{
    OLED_Init();       // 初始化OLED

		Serial_Init();
	
	

    while(1)
    {
			if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)== SET)
			{
				RxData=USART_ReceiveData(USART1);
				OLED_ShowHexNum(1,1,RxData,2);
			}
    }
}