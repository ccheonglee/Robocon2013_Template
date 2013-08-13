/**
  ******************************************************************************
  * @file    DAC124S085.h
  * @author  Eugene
  * @date    11/26/2012
  * @brief   This file provides 12bits  DAC124S085 header file
  ******************************************************************************
  */

#ifndef DAC124S085_H_
#define DAC124S085_H_

#include "stm32f4xx.h"

#define SET_DAC_CLK 		GPIO_SetBits (GPIOB, GPIO_Pin_2)
#define CLR_DAC_CLK 		GPIO_ResetBits (GPIOB, GPIO_Pin_2)
#define SET_DAC_DI 			GPIO_SetBits (GPIOD, GPIO_Pin_1)
#define CLR_DAC_DI 			GPIO_ResetBits (GPIOD, GPIO_Pin_1)
#define SET_DAC1_SYNC 		GPIO_SetBits (GPIOD, GPIO_Pin_2)
#define CLR_DAC1_SYNC 		GPIO_ResetBits (GPIOD, GPIO_Pin_2)
#define SET_DAC2_SYNC 		GPIO_SetBits (GPIOE, GPIO_Pin_10)
#define CLR_DAC2_SYNC 		GPIO_ResetBits (GPIOE, GPIO_Pin_10)

#define DAC_VEXTA3 ((uint16_t)0x1000)
#define DAC_VEXTA2 ((uint16_t)0x5000)
#define DAC_VEXTA1 ((uint16_t)0x9000)
#define DAC_VEXTA6 ((uint16_t)0x1001)
#define DAC_VEXTA5 ((uint16_t)0x5001)
#define DAC_VEXTA4 ((uint16_t)0x9001)

/** Functions prototype
  * @{
  */
void DAC_init (void);
void DAC_send(uint16_t vextaChannel, uint16_t step);

#endif /* DAC124S085_H_ */
