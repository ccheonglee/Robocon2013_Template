/*
  ******************************************************************************
  * @file    ps2.h
  * @author  Eugene
  * @date    Dec 4, 2012
  * @brief   This file provides ps2 header file
  ******************************************************************************
 */

#ifndef PS2_H_
#define PS2_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


// PSX command/data control
#define SET_PS2_COMM 		GPIO_SetBits (GPIOE, GPIO_Pin_3) //orange output
#define CLR_PS2_COMM 		GPIO_ResetBits (GPIOE, GPIO_Pin_3)
#define SET_PS2_ATT 		GPIO_SetBits (GPIOE, GPIO_Pin_4) //yellow output
#define CLR_PS2_ATT 		GPIO_ResetBits (GPIOE, GPIO_Pin_4)
#define SET_PS2_CLOCK 		GPIO_SetBits (GPIOE, GPIO_Pin_5) //blue output
#define CLR_PS2_CLOCK 		GPIO_ResetBits (GPIOE, GPIO_Pin_5)

//GPIOE, GPIO_Pin_2 = data(brown)
//GPIOE, GPIO_Pin_6 = ack (green)

typedef struct {
	uint8_t	 COMBO1;
	uint8_t	 COMBO2;
	uint8_t	 RJOY_X;
	uint8_t	 RJOY_Y;
	uint8_t	 LJOY_X;
	uint8_t	 LJOY_Y;
	uint8_t	 Response;
} PSX_DATA;

extern PSX_DATA psxData;

/** Functions prototype
  * @{
  */

void psx_init (void);
void psx_status (void);
uint8_t	get_psx (void);


#endif /* PS2_H_ */
