#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"

#define MPU6050_ADDRESS    0XD0

void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	MyI2C_start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveACK();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveACK();
	MyI2C_SendByte(Data);
	MyI2C_ReceiveACK();
	MyI2C_Stop();
	
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_start();
	MyI2C_SendByte(MPU6050_ADDRESS);
	MyI2C_ReceiveACK();
	MyI2C_SendByte(RegAddress);
	MyI2C_ReceiveACK();

	MyI2C_start();
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
	MyI2C_ReceiveACK();
	Data=MyI2C_ReceiveByte();
	MyI2C_SendACK(1);
	MyI2C_Stop();
}

void MPU6050_Init()
{
	MyI2C_Init();
}