#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Serve_Init(void)
{
	PWM_Init();
	
}



void Serve_SetAngle(float Angle)
{
	PWM_SetCompare2(Angle / 180*2000+500);
	
}