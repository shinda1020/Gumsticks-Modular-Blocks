#include "spi.h"

void SPIx_Init(void) {
 SPI_InitTypeDef SPI_InitStructure;
 GPIO_InitTypeDef GPIO_InitStructure; 
  
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructure);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//SPI CS 
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//ͨ���������  GPIO_Init(GPIOA, &GPIO_InitStructure);  GPIO_SetBits(GPIOA,GPIO_Pin_2); 
 RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1, ENABLE );//(6) 
 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫�� 
 
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  //����SPI����ģʽ:����Ϊ��SPI 
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;  //ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; //���ݲ����ڵڶ���ʱ���� 
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;  //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ���� 
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;  //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256 
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ 
 SPI_InitStructure.SPI_CRCPolynomial = 7; //CRCֵ����Ķ���ʽ 
 SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���    //(7) 
 SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����   
 SPIx_ReadWriteByte(0xff);//��������    
 
 }      

u8 SPIx_ReadByte(void) {   
 u8 retry=0;      
 while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ  
 {
    retry++;
	if(retry>200) return 0;  
 }     
              
 return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����         
 
}

void SPIx_WriteByte(u8 TxData) {   
 u8 retry=0;      

 SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������  retry=0; 
 while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ  
 {   
  retry++; 
  if(retry>200) return;  
 }														     
}

u8 SPIx_ReadWriteByte(u8 TxData) {
 u8 retry=0;      
 while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
 {   retry++;   if(retry>200)    return 0;  }     
 SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������ retry=0; 
 while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET); //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
 {   retry++; 
  if(retry>200) return 0;  }              
 return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����         
 }