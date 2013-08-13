/**
  ******************************************************************************
  * @file    io_exp_vexta.h
  * @author  Eugene
  * @date    Dec 21, 2012
  * @brief   This file provides vexta io expander header file
  ******************************************************************************
  */

#ifndef IO_EXP_VEXTA_H_
#define IO_EXP_VEXTA_H_

#include "stm32f4xx.h"

/*************************************************************
MCP23S18
#defines are with IOCON.BANK = 0
**************************************************************/
#ifndef IO_EXP_IODIRA
	#define IO_EXP_IODIRA           0x00
	#define IO_EXP_IODIRB           0x01
	#define IO_EXP_IPOLA            0x02
	#define IO_EXP_IPOLB            0x03
	#define IO_EXP_GPINTENA         0x04
	#define IO_EXP_GPINTENB         0x05
	#define IO_EXP_DEFVALA          0x06
	#define IO_EXP_DEFVALB          0x07
	#define IO_EXP_INTCONA          0x08
	#define IO_EXP_INTCONB          0x09
	#define IO_EXP_IOCONA           0x0A
	#define IO_EXP_IOCONB           0x0B
	#define IO_EXP_GPPUA            0x0C
	#define IO_EXP_GPPUB            0x0D
	#define IO_EXP_INTFA            0x0E
	#define IO_EXP_INTFB            0x0F
	#define IO_EXP_INTCAPA          0x10
	#define IO_EXP_INTCAPB          0x11
	#define IO_EXP_GPIOA            0x12
	#define IO_EXP_GPIOB            0x13
	#define IO_EXP_OLATA            0x14
	#define IO_EXP_OLATB            0x15
#endif

// IO_C0
#define ALARM_OUT1           0
#define ALARM_OUT2           1
#define ALARM_OUT3           2
#define ALARM_RST3           3
#define ALARM_RST2           4
#define ALARM_RST1           5
#define DIRECTION3           6
#define DIRECTION2           7

#define STARTSTOP3           8
#define STARTSTOP2           9
#define STARTSTOP1           10
#define RUNBRAKE2		  	 11
#define RUNBRAKE1            12
#define RUNBRAKE3            13
#define DIRECTION1           14
#define IOVEXTA_GPB7   		 15

// IO_E14
#define ALARM_OUT4         16
#define ALARM_OUT5         17
#define ALARM_OUT6         18
#define ALARM_RST6         19
#define ALARM_RST5         20
#define ALARM_RST4         21
#define DIRECTION6         22
#define DIRECTION5         23

#define STARTSTOP6         24
#define STARTSTOP5         25
#define STARTSTOP4         26
#define RUNBRAKE5          27
#define RUNBRAKE4          28
#define RUNBRAKE6          29
#define DIRECTION4         30
#define IOVEXTA_GPB7_2     31

#define SET_IOE_RESET1 		GPIO_SetBits (GPIOC, GPIO_Pin_15)
#define CLR_IOE_RESET1	 	GPIO_ResetBits (GPIOC, GPIO_Pin_15)
#define SET_IOE_RESET2 		GPIO_SetBits (GPIOD, GPIO_Pin_3)
#define CLR_IOE_RESET2	 	GPIO_ResetBits (GPIOD, GPIO_Pin_3)

#define SET_IOE_CS1 		GPIO_SetBits (GPIOC, GPIO_Pin_14)
#define CLR_IOE_CS1	 		GPIO_ResetBits (GPIOC, GPIO_Pin_14)
#define SET_IOE_CS2 		GPIO_SetBits (GPIOD, GPIO_Pin_6)
#define CLR_IOE_CS2	 		GPIO_ResetBits (GPIOD, GPIO_Pin_6)

#define SET_IOE_SCK 		GPIO_SetBits (GPIOB, GPIO_Pin_2)
#define CLR_IOE_SCK 		GPIO_ResetBits (GPIOB, GPIO_Pin_2)
#define SET_IOE_SI 			GPIO_SetBits (GPIOD, GPIO_Pin_1)
#define CLR_IOE_SI 			GPIO_ResetBits (GPIOD, GPIO_Pin_1)

#define READ_MISO			GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);

/*  function prototypes */
void IoExpVexta_init (void);
void VextaGPIOWrite(uint8_t gpio, uint8_t bit);
uint8_t VextaGPIORead (uint8_t gpio);

#endif /* IO_EXP_VEXTA_H_ */
