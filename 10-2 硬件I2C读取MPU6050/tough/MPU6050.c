#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"

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
	return Data;
}

void MPU6050_Init(void)
{
	MyI2C_Init();
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);

}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
	
}


void MPU6050_GetData(int16_t *ACCX,int16_t *ACCY,int16_t *ACCZ,
	int16_t *GYROX,int16_t *GYROY,int16_t *GYROZ)
{
	uint8_t DataH,DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*ACCX=(DataH<<8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*ACCY=(DataH<<8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*ACCZ=(DataH<<8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*ACCX=(DataH<<8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*ACCY=(DataH<<8) | DataL;
	
	DataH=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*ACCZ=(DataH<<8) | DataL;
}