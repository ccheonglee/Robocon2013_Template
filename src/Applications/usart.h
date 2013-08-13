/**
  ******************************************************************************
  * @file    usart.h
  * @author  Eugene
  * @date    Dec 4, 2012
  * @brief   This file provides usart header file
  ******************************************************************************
  */

#ifndef USART_H_
#define USART_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#define MAX_STRLEN 12 // this is the maximum string length of our string in characters

volatile char received_string[MAX_STRLEN+1]; // this will hold the recieved string
char message[100];

/** Functions prototype
  * @{
  */
void Usart1_init(uint32_t baudrate);
void Usart1_puts(char *s);

#endif /* USART_H_ */
