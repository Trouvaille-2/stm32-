#include "stm32f10x.h"                  // Device header
#include "Delay.h"                       // 增加延时头文件，用于防抖

uint16_t CountSensor_Count = 0;         // 显式初始化为0，避免未知值

void CountSensor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;        // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
    
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 下降沿触发
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 响应优先级1
    NVIC_Init(&NVIC_InitStructure);
}

uint16_t CountSensor_Get(void)
{
    return CountSensor_Count;
}

// 中断服务函数（核心优化：增加防抖）
void EXTI15_10_IRQHandler(void)
{
    // 1. 检查中断标志位
    if(EXTI_GetITStatus(EXTI_Line14) == SET)
    {
        // 2. 防抖延时：等待电平稳定（10ms，可根据传感器调整）
        Delay_ms(10);
        
        // 3. 二次确认电平：确保确实是有效下降沿
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) == 0)
        {
            CountSensor_Count++; // 计数+1
        }
        
        // 4. 清除中断标志位（必须做，否则会重复触发中断）
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}