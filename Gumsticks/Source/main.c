/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : Main program body
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "stdio.h"
#include "platform_config.h"
#include "uart.h"
#include "MPU6050.h"
#include "message.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USE_USART1		// Used for sending and receiving ID
// #define USE_USART2		// Used for Wi-Fi transmission
// #define USE_I2C		// Used for IMU
#define USE_GPIO		// Used for capacitive sensing

// #define USE_TIM2		// Used for sending SELF_ID
// #define USE_TIM3		// Used for checking Neighbor_ID

#define ABS(x)		(x > 0) ? x : -x

#define SEND_PERIOD			(4000 - 1)		// The sending period of SELF_ID, roughly 2 seconds
#define CHECK_PERIOD		(10000 - 1)		// The checking period of Neighbor_ID, roughly 5 seconds

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

bool Receive_Flag = FALSE;	// Flag variable for checking if there's timeout in receiving Neighbor_ID
u8 Neighbor_ID = 0;		// Neighbor module's ID

s16 AccelArray[3];		// Acceleration data array
	
u8 PrevStatus = 0x00;	// Touch module status

/* Private function prototypes -----------------------------------------------*/

// RCC Configuration
void RCC_Configuration(void);

// GPIO Configurations
void GPIO_Configuration(void);
void USART_GPIO_Configuration(void);
void Regular_GPIO_Configuration(void);

// Timer Configurations
void TIMER_Configuration(void);

// NVIC Configuration
void NVIC_Configuration(void);

// Main Loop
void Loop(void);

/* Private local functions ---------------------------------------------------*/
char* str_concat(const char *str1,const char *str2);
void delay_ms(u16 time);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
  
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
#ifdef DEBUG
	debug();
#endif

	/* Configure the default Neighbor_ID to NEIGHBOR_ID_NONE */
	Neighbor_ID = NEIGHBOR_ID_NONE;
	   
	/* System Clocks Configuration */
	RCC_Configuration();
       
	/* NVIC configuration */
	NVIC_Configuration();

	/* Configure the GPIO ports */	                                                                                                                                     
	GPIO_Configuration();

	/* Configure the USART ports */
	USART_Configuration();						

	/* Timer Configuration */
#if (defined USE_TIM2) || (defined USE_TIM3)
	TIMER_Configuration();
#endif

	/* Configure the I2C ports */
#ifdef USE_I2C
	MPU6050_I2C_Init();
	MPU6050_Initialize();
#endif
																												  
	// Hang the program to receive interrupt request
	while(1) {
		Loop();
	}
}

/*******************************************************************************
* Function Name  : Loop
* Description    : Main loop of the program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Loop(void)
{
#ifdef USE_I2C		
	if(MPU6050_TestConnection()) {
		u8 str[50];

		//USART_Send_Str(USART2, "Connected!\n");
		//USART_SendByte(USART2, MPU6050_GetDeviceID());
		MPU6050_GetRawAccel(AccelArray);

		//if ( ABS(AccelArray[0]) > 16384 || ABS(AccelArray[1]) > 16384 || ABS(AccelArray[2]) > 16384) {
			sprintf(str, "%d %d %d\n", AccelArray[0], AccelArray[1], AccelArray[2]);
			USART_Send_Str(USART2, str);
		//}

	} else {
		USART_Send_Str(USART2, "Not connected!\n");
	}

	/* Delay for 100ms */
	delay_ms(100);
#endif

#ifdef USE_GPIO
	u8 pinData = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
	
	if (PrevStatus != pinData) {
		USART_Send_Str(USART1, ((pinData == 0x00) ? (u8*)"Released\n" : (u8*)"Touched\n"));
		PrevStatus = pinData;
	}

	/* Delay for 1ms */
	delay_ms(1);
#endif			 
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;

	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    	/* Flash 2 wait state */
    	FLASH_SetLatency(FLASH_Latency_2);
 
    	/* HCLK = SYSCLK */
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    	/* PCLK2 = HCLK */
    	RCC_PCLK2Config(RCC_HCLK_Div1); 

    	/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);

    	/* PLLCLK = 8MHz * 9 = 72 MHz */
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    	/* Enable PLL */ 
    	RCC_PLLCmd(ENABLE);

    	/* Wait till PLL is ready */
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    	{
    	}

    	/* Select PLL as system clock source */
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    	/* Wait till PLL is used as system clock source */
    	while(RCC_GetSYSCLKSource() != 0x08)
    	{
    	}
	}
    
	/* Enable GPIOx clock */
#if (defined USE_USART1) || (defined USE_USART2)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// For USART1 and USART2
#endif

#ifdef USE_TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
#endif

#ifdef USE_TIM3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
#endif

#ifdef USE_GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#endif

	/* Enable USARTx clocks */
#ifdef USE_USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// For USART1
#endif

#ifdef USE_USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	// For USART2
#endif

}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
	/* Configure GPIO ports using USART */
#if (defined USE_USART1) || (defined USE_USART2)
	USART_GPIO_Configuration();
#endif
    
	/* Configure regular GPIO ports */
#ifdef USE_GPIO
	Regular_GPIO_Configuration();
#endif
}

/*******************************************************************************
* Function Name  : Regular_GPIO_Configuration
* Description    : Configures the regular GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Regular_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure PB.06 as Input pull-up */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //…œ¿≠ ‰»Î
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : USART_GPIO_Configuration
* Description    : Configures the different GPIO ports using USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#ifdef USE_USART1
	/**
	 * Configure USART1
	 */ 

	// Configure USART1_Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin			 = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed		 = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode		 = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure USART1_Rx as input floating
	GPIO_InitStructure.GPIO_Pin			 = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode		 = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

#ifdef USE_USART2
	/**
	 * Configure USART2
	 */ 

	// Configure USART2_Tx as alternate function push-pull
	GPIO_InitStructure.GPIO_Pin			 = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed 	  	 = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode		 = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure USART2_Rx as input floating
	GPIO_InitStructure.GPIO_Pin			 = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode		 = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : TIMER_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIMER_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

#ifdef USE_TIM2	
	TIM_DeInit(TIM2);

	/* TIM2 configuration */
	TIM_InternalClockConfig(TIM2);
	TIM_TimeBaseStructure.TIM_Prescaler			 = (36000 - 1);
	TIM_TimeBaseStructure.TIM_ClockDivision		 = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode 		 = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period			 = SEND_PERIOD;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);

	/* Clear TIM2 update pending flag */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	// TIM_ARRPreloadConfig(TIM2, DISABLE);

 	/* Enable TIM2 Update interrupt */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	/* TIM2 enable counter */
    TIM_Cmd(TIM2, ENABLE);
#endif

#ifdef USE_TIM3
	TIM_DeInit(TIM3);

	/* TIM2 configuration */
	TIM_InternalClockConfig(TIM3);
	TIM_TimeBaseStructure.TIM_Prescaler			 = (36000 - 1);
	TIM_TimeBaseStructure.TIM_ClockDivision		 = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode 		 = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period			 = CHECK_PERIOD;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

	/* Clear TIM3 update pending flag */
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

 	/* Enable TIM3 Update interrupt */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);
#endif
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{ 
	NVIC_InitTypeDef NVIC_InitStructure; 

#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

	/* Configure the NVIC Preemption Priority Bits */ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

#ifdef USE_USART1
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel 						= USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_USART2
	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel 						= USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_TIM2
	/* Enable the TIM2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel						= TIM2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

#ifdef USE_TIM3
	/* Enable the TIM3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel						= TIM3_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay for given time in microseconds.
* Input          : u16 time: time in microseconds
* Output         : None
* Return         : None
*******************************************************************************/
void delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=12000;
      while(i--) ;    
   }
}

/*******************************************************************************
* Function Name  : str_concat
* Description    : Concatenate two char arrays.
* Input          : str1, str2
* Output         : None
* Return         : result
*******************************************************************************/
/*
char* str_concat(const char *str1,const char *str2)
{
  	char* result;
    result = (char*)malloc(strlen(str1) + strlen(str2) + 1);
    if(!result){
         #ifdef DEBUG
	 	printf("Error: malloc failed in concat! \n");
	 #endif
         exit(1);
    }
    strcpy(result,str1);
    strcat(result,str2);
    return result;
}*/ 

/*******************************************************************************
* Function Name  : PUTCHAR_PROTOTYPE
* Description    : Retargets the C library printf function to the USART.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
PUTCHAR_PROTOTYPE
{										  
  	/* Write a character to the USART */
    USART_SendData(USART2, (u8) ch);

    /* Loop until the end of transmission */
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {
    }

    return ch;
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  	/* User can add his own implementation to report the file name and line number,
  	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
	{
	}
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
