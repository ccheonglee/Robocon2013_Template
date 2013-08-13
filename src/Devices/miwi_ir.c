/*
 * miwi_ir.c
 *
 *  Created on: Feb 26, 2013
 *      Author: Chee Cheong Lee
 */

/*
 * This file provides miwi_ir configurations and usable methods
 *
 *
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "miwi_ir.h"
#include <string.h>
#include <stdio.h>

static void MiwiIR_setUsart (uint32_t baudrate);
static void Usart3_puts(char *string);

/**
  * @brief  USART1 peripheral initialization
  * @param  baudrate --> the baudrate at which the USART is
 * 						   supposed to operate
  * @retval void
  */
void MiwiIR_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* alternate function configuration */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/* Configure USART Tx as alternate function  */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Connect PD8 to COMM_PORT_Tx*/
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_USART3);

	/* Connect PD9 to COMM_PORT_Rx*/
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_USART3);

	MiwiIR_setUsart(115200);
}

/**
  * @brief  setup USART for Miwi/IR transceiver
  * @param 	void
  * @retval void
  */
static void MiwiIR_setUsart (uint32_t baudrate)
{
	USART_InitTypeDef USART_InitStructure;

	/* Enable USART3 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USARTx */
	USART_Init(USART3, &USART_InitStructure);
		/* Enable the USARTx */
	USART_Cmd(USART3, ENABLE);
}

/**
  * @brief  Send string through usart, have to use sprintf(message, "Good morning, a = %d.", a);
  * 		to store the string in a buffer "message" before sending through usart.
  * @param  *s: string to be sent, it should be always char string "message".
  * @retval void
  */
static void Usart3_puts(char *string)
{
	uint16_t i;
	for (i=0; string[i] !=0; i++){
		USART_SendData(USART1, string[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	}
}

void MiwiIR_printMiwi (char *string)
{
	Usart3_puts(string);
}
