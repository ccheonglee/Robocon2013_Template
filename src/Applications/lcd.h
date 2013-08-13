/*
  ******************************************************************************
  * @file    lcd.h
  * @author  Eugene
  * @date    Dec 6, 2012
  * @brief   This file provides lcd header file
  ******************************************************************************
 */


#ifndef LCD_H_
#define LCD_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


#define INST_REG 0
#define DATA_REG 1

#define SET_LCD_RS 		GPIO_SetBits (GPIOE, GPIO_Pin_7)
#define CLR_LCD_RS 		GPIO_ResetBits (GPIOE, GPIO_Pin_7)

#define SET_LCD_EN 		GPIO_SetBits (GPIOE, GPIO_Pin_8)
#define CLR_LCD_EN 		GPIO_ResetBits (GPIOE, GPIO_Pin_8)

#define SET_LCD_D4 		GPIO_SetBits (GPIOB, GPIO_Pin_1)
#define CLR_LCD_D4 		GPIO_ResetBits (GPIOB, GPIO_Pin_1)

#define SET_LCD_D5 		GPIO_SetBits (GPIOB, GPIO_Pin_0)
#define CLR_LCD_D5 		GPIO_ResetBits (GPIOB, GPIO_Pin_0)

#define SET_LCD_D6 		GPIO_SetBits (GPIOC, GPIO_Pin_5)
#define CLR_LCD_D6 		GPIO_ResetBits (GPIOC, GPIO_Pin_5)

#define SET_LCD_D7 		GPIO_SetBits (GPIOC, GPIO_Pin_4)
#define CLR_LCD_D7 		GPIO_ResetBits (GPIOC, GPIO_Pin_4)

/** Functions prototype
  * @{
  */
void init_lcd(void);
void lcd_print (char *str);
void clear_lcd (void);

#endif /* LCD_H_ */
