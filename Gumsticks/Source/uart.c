#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "uart.h"

/*******************************************************************************
* Function Name  : USART_SendByte
* Description    : Write a byte to USART
* Input          : USART_TypeDef* USARTx, the USART port used to write
				 : u16 Data, the data
* Output         : None
* Return         : None
*******************************************************************************/
void USART_SendByte(USART_TypeDef* USARTx, u8 Data)
{ 
	/*
	while (!(USARTx->SR & USART_FLAG_TC));
   	//USARTx->DR = (Data & (uint16_t)0x01FF);	 
 	//while (*s != '\0')  
	USART_SendData(USARTx, Data); */
	USART_SendData(USARTx, Data);
    while( USART_GetFlagStatus(USARTx,USART_FLAG_TC)!= SET);
    
}	

/*******************************************************************************
* Function Name  : USART_Send_Str
* Description    : Write a string to USART
* Input          : USART_TypeDef* USARTx, the USART port used to write
				 : unsigned char *s, the string data
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Send_Str(USART_TypeDef* USARTx, u8* str)
{
	u8 i=0;

	while(str[i]!='\0')
	{
		USART_SendData(USARTx,str[i]);
		while( USART_GetFlagStatus(USARTx,USART_FLAG_TC)!= SET);  
		i++;
	}  
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure the USART
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_Configuration(void)
{
	USART_InitTypeDef USART1_InitStructure;
	USART_InitTypeDef USART2_InitStructure;

  /* USART1 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to the SCLK pin
   */

	USART1_InitStructure.USART_BaudRate            = 9600  ;
	USART1_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART1_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART1_InitStructure.USART_Parity              = USART_Parity_No ;
	USART1_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART1_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART1_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	
  /* USART2 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
        - USART Clock disabled
        - USART CPOL: Clock is active low
        - USART CPHA: Data is captured on the middle 
        - USART LastBit: The clock pulse of the last data bit is not output to the SCLK pin
   */

	USART2_InitStructure.USART_BaudRate            = 9600  ;
	USART2_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART2_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART2_InitStructure.USART_Parity              = USART_Parity_No ;
	USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART2_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART2_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);
	     
/*	DIR485_L;  */
}
