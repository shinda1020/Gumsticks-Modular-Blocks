#ifndef __UART_H
#define __UART_H

extern void USART_SendByte(USART_TypeDef* USARTx, u8 Data);
void USART_Send_Str(USART_TypeDef* USARTx, unsigned char *s);
void USART_Configuration(void);

#define DIR485_H  GPIOC->BSRR=1<<1
#define DIR485_L  GPIOC->BRR=1<<1 

#endif
