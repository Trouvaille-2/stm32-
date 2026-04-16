#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "CountSensor.h"


int main(void)
{
    OLED_Init();       // 初始化OLED
    CountSensor_Init();// 初始化计数传感器
	

    OLED_ShowString(1, 1, "Count:");

    while(1)
    {
       
      OLED_ShowNum(1, 7,CountSensor_Get(), 5);
        
    }
}