/**
  ******************************************************************************
  * @file    delay.h
  * @author  Eugene
  * @date    Dec 5, 2012
  * @brief   This file provides timer7 and systemtick delay header file
  ******************************************************************************
  */

#ifndef DELAY_H_
#define DELAY_H_
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/** Functions prototype
  * @{
  */
void delay_ms(uint32_t miliseconds);
void Delaynus(vu32 nus);
#endif /* DELAY_H_ */
