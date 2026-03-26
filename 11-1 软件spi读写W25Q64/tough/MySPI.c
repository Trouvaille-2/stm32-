#include "stm32f10x.h"                  // Device header

void MySPI_W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)BitValue);
}

void MySPI_W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,(BitAction)BitValue);
}

void MySPI_W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(GPIOA,GPIO_Pin_7,(BitAction)BitValue);
}

uint8_t MySPI_R_MISO(void)
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
}

void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=  GPIO_Pin_4 | GPIO_Pin_5|  GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin=  GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	MySPI_W_SS(1);
	MySPI_W_SCK(0);
	
}

void MySPI_Start(void)
{
	MySPI_W_SS(0);
}

void MySPI_Stop(void)
{
	MySPI_W_SS(1);
}

uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
    uint8_t i, ByteReceive = 0x00;
    
    for(i = 0; i < 8; i++)
    {
        // 1. 准备 MOSI 数据
        // 修正写法：先判断位是否为1，再写入 1 或 0
        if(ByteSend & (0x80 >> i))
            GPIO_SetBits(GPIOA, GPIO_Pin_7);
        else
            GPIO_ResetBits(GPIOA, GPIO_Pin_7);
            
        // 2. 产生时钟脉冲
        GPIO_SetBits(GPIOA, GPIO_Pin_5); // SCK = 1
        
        // --- 关键修改：增加一点延时让从机响应 ---
        // 简单的空循环，具体次数视主频而定，通常几次就够了
        // 或者调用你的 Delay_us(1); 如果有这个函数
        volatile int delay = 10; while(delay--); 
        // -------------------------------------

        // 3. 读取 MISO
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) == 1)
        {
            ByteReceive |= (0x80 >> i);
        }
        
        GPIO_ResetBits(GPIOA, GPIO_Pin_5); // SCK = 0
    }
    
    return ByteReceive;
}
