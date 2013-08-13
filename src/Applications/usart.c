/**
  ******************************************************************************
  * @file    usart.c
  * @author  Eugene
  * @date    Dec 4, 2012
  * @brief   This file provides usart algorithm
  ******************************************************************************
  */

/*
 * This file provides usart1 configurations
 * Have to initialize it by writing Usart1_init();
 * Write string through usart1 by writing sprintf(message, "Good morning, a = %d.", a);
 * then Usart1_puts(message);
 */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/**
  * @brief  USART1 peripheral initialization
  * @param  baudrate --> the baudrate at which the USART is
 * 						   supposed to operate
  * @retval void
  */
void Usart1_init(uint32_t baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* alternate function configuration */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function  */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect PB6 to COMM_PORT_Tx*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);

	/* Connect PB7 to COMM_PORT_Rx*/
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);

	/* Enable USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USARTx */
	USART_Init(USART1, &USART_InitStructure);
	/* Enable the USARTx */
	USART_Cmd(USART1, ENABLE);
}

/**
  * @brief  Send string through usart, have to use sprintf(message, "Good morning, a = %d.", a);
  * 		to store the string in a buffer "message" before sending through usart.
  * @param  *s: string to be sent, it should be always char string "message".
  * @retval void
  */
void Usart1_puts(char *string)
{
	uint16_t i;
	for (i=0; string[i] !=0; i++){
		USART_SendData(USART1, string[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}


