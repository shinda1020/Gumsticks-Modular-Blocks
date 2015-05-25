#ifndef __SPI_H
#define __SPI_H
#include "stm32f10x_lib.h"


void SPIx_Init(void);//初始化SPI口
u8 SPIx_ReadWriteByte(u8 TxData);
u8 SPIx_ReadByte(void);
void SPIx_WriteByte(u8 TxData);//SPI总线读写一个字节

#endif