#ifndef __MYI2C_H
#define __MYI2C_H


void MyI2C_Init();
void MyI2C_start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendACK(uint8_t ACKBit);
uint8_t MyI2C_ReceiveACK(void);
#endif