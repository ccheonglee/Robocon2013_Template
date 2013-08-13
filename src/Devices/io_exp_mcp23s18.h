/*
  * @file    io_exp_mcp23s18.h
  * @author  Boon
  * @date    Nov 28, 2012
  * @brief   This file provides io expander header file.
 */

#ifndef __MCP23S18_H_
#define __MCP23S18_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/*************************************************************
MCP23S18
#defines are with IOCON.BANK = 0
**************************************************************/
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
#define IO_EXP_INTCAPA          0x010
#define IO_EXP_INTCAPB          0x011
#define IO_EXP_GPIOA            0x012
#define IO_EXP_GPIOB            0x013
#define IO_EXP_OLATA            0x014
#define IO_EXP_OLATB            0x015

// IO_C0
#define GPA0           0
#define GPA1           1
#define GPA2           2
#define GPA3           3
#define GPA4           4
#define GPA5           5
#define GPA6           6
#define GPA7           7

#define GPB7           8
#define GPB6           9
#define GPB5           10
#define GPB4           11
#define GPB3           12
#define GPB2           13
#define GPB1           14
#define GPB0           15

// IO_E14
#define GPA0_2         16
#define GPA1_2         17
#define GPA2_2         18
#define GPA3_2         19
#define GPA4_2         20
#define GPA5_2         21
#define GPA6_2         22
#define GPA7_2         23

#define GPB7_2         24
#define GPB6_2         25
#define GPB5_2         26
#define GPB4_2         27
#define GPB3_2         28
#define GPB2_2         29
#define GPB1_2         30
#define GPB0_2         31

// IO_E15
#define GPIO_PWM2         32	//PWM2
#define GPIO_PWM9         33	//PWM9
#define GPIO_PWM5         34	//PWM5
#define GPIO_PWM1         35	//PWM1
#define GPIO_LED5         36	//LED5
#define GPIO_LED6         37	//LED6
#define GPIO_LED7         38	//LED7
#define GPIO_LED8         39	//LED8

#define GPIO_PWM6         40	//PWM6
#define GPIO_PWM10        41	//PWM10
#define GPIO_PWM3         42	//PWM3
#define GPIO_PWM8         43	//PWM8
#define GPIO_PWM11        44	//PWM11
#define GPIO_PWM4         45	//PWM4
#define GPIO_PWM7         46	//PWM7
#define GPIO_PWM12        47	//PWM12

#define U22 0
#define U25 1
#define U26 2

#define IO_OUTPUT 0
#define IO_INPUT 1

#define CS_HIGH_U22			GPIO_SetBits (GPIOC, GPIO_Pin_0)
#define CS_HIGH_U25			GPIO_SetBits (GPIOE, GPIO_Pin_14)
#define CS_HIGH_U26			GPIO_SetBits (GPIOE, GPIO_Pin_15)
#define CS_LOW_U22			GPIO_ResetBits (GPIOC, GPIO_Pin_0)
#define CS_LOW_U25			GPIO_ResetBits (GPIOE, GPIO_Pin_14)
#define CS_LOW_U26			GPIO_ResetBits (GPIOE, GPIO_Pin_15)

#define RESET_HIGH_U22		GPIO_SetBits (GPIOC, GPIO_Pin_1)
#define RESET_HIGH_U25		GPIO_SetBits (GPIOE, GPIO_Pin_12)
#define RESET_HIGH_U26		GPIO_SetBits (GPIOE, GPIO_Pin_13)
#define RESET_LOW_U22		GPIO_ResetBits (GPIOC, GPIO_Pin_1)
#define RESET_LOW_U25		GPIO_ResetBits (GPIOE, GPIO_Pin_12)
#define RESET_LOW_U26		GPIO_ResetBits (GPIOE, GPIO_Pin_13)

/*  function prototypes */
void IoExp_init (void);
void setGPIOIODir (uint8_t gpio, uint8_t inputOutput);
void setGPIOPullUp (uint8_t gpio, uint8_t status);
void GPIOWrite(uint8_t gpio, uint8_t bit);
uint8_t GPIORead(uint8_t gpio);

/* update function */
void IoExpInput_update (void);
void IoExpOutput_update (void);

#endif /* FUNCTION_H_ */
