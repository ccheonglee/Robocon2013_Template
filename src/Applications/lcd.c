/**
  ******************************************************************************
  * @file    lcd.c
  * @author  Eugene
  * @date    Dec 6, 2012
  * @brief   This file provides lcd function
  *
  * lcd command table
  * 1. Function Set: 8-bit, 1 Line, 5x7 Dots 	 0x30
  * 2. Function Set: 8-bit, 2 Line, 5x7 Dots	 0x38
  * 3. Function Set: 4-bit, 1 Line, 5x7 Dots	 0x20
  * 4. Function Set: 4-bit, 2 Line, 5x7 Dots	 0x28
  * 5. Entry Mode								 0x06
  * 6. Display off Cursor off					 0x08
  * (clearing display without clearing DDRAM content)
  * 7. Display on Cursor on 					 0x0E
  * 8. Display on Cursor off					 0x0C
  * 9. Display on Cursor blinking	 			 0x0F
  * 10.Shift entire display left				 0x18
  * 12.Shift entire display right				 0x1C
  * 13.Move cursor left by one character		 0x10
  * 14.Move cursor right by one character		 0x14
  * 15.Clear Display (also clear DDRAM content)	 0x01
  * 16.Set DDRAM address or coursor position on display	 	0x80+add*
  * 17.Set CGRAM address or set pointer to CGRAM location	0x40+add**
  ******************************************************************************
  * This file provide lcd configurations.
  * Have to initialize it before used by writing init_lcd();
  * Print string on lcd by the function lcd_print(string);
  * ex: lcd_print("Welcome\trobocon\n2013!");
  * clear the lcd screen by clear_lcd();
  */

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"
#include "delay.h"

static void LowLevelInit_LCD(void);
static void set_lcd (uint8_t RegSelect, uint8_t WRData);
static void get_lcd_ready (void);
/**
  * @brief  setup lcd interface
  * @param  void
  * @retval void
  */

void init_lcd(void)
{
	LowLevelInit_LCD();
	delay_ms(100);
	CLR_LCD_RS;
	SET_LCD_EN;
	// 0011 * 3	times
	CLR_LCD_D7 ;
	CLR_LCD_D6;
	SET_LCD_D5;
	SET_LCD_D4;
	nop();
	nop();
	nop();
	nop();
	CLR_LCD_EN;
	delay_ms(5);

	SET_LCD_EN;
	nop();
	nop();
	nop();
	nop();
	CLR_LCD_EN;

	delay_us(200);

	SET_LCD_EN;
	nop();
	nop();
	nop();
	nop();
	CLR_LCD_EN;

	delay_us(200);

	/************** 8 Bit Initialisation *******************
	//data length:8 bit mode ; display no: 2 line ;char font : 5*8
	set_lcd(INST_REG,0x38);
	delay_us(100);
	set_lcd(INST_REG,0x08);
	delay_us(100);
	/************** 4 Bit Initialisation *******************/
	//data length:4 bit mode ; display no: 2 line ;char font : 5*8
	SET_LCD_EN;
	CLR_LCD_D7;
	CLR_LCD_D6;
	SET_LCD_D5;
    CLR_LCD_D4;
	nop();
	nop();
	nop();
	nop();

	CLR_LCD_EN;
	nop();
	nop();
	nop();
	nop();
	delay_us(100);
	//display:Off ; cursor: off; blink of cursor: off
	set_lcd(INST_REG,0x28);
	delay_us(100);
	/*****************************************************/
	//clear display n return home position
	set_lcd(INST_REG,0x01);
	delay_ms(5);
	set_lcd(INST_REG,0x02);
	delay_ms(5);

	//cursor move direction: decrement position; shift the display:	move cursor
	set_lcd(INST_REG,0x06);
	delay_us(100);
	//cursor move: display shift; shift direction: left
	set_lcd(INST_REG,0x14);
	delay_us(100);
	// display : on cursor : off; blink of cursor: off
	set_lcd(INST_REG,0x0C);
	delay_us(100);

}

/**
  * @brief  setup lcd bit configurations
  * @param  void
  * @retval void
  */

static void LowLevelInit_LCD(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE| RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC , ENABLE);
	/*configure ps2 pin*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  send command or data to lcd
  * @param  Regselect is parameter to choose either send command or data through RS pin
  * Regselect = INST_REG ( send command)
  * Regselect = DATA_REG ( send data)
  * WRData : instruction or data byte
  * @retval void
  */

static void set_lcd (uint8_t RegSelect, uint8_t WRData)
{
	get_lcd_ready ();

	if(RegSelect)SET_LCD_RS;
	else CLR_LCD_RS;
	/******************** 8 Bit Interface *******************
	En = 1;
	DB0_7 = WRData;
	nop();
	nop();
	nop();
	nop();

	En = 0;
	nop();
	nop();
	nop();
	nop();
	********************************************************/
	/******************** 4 Bit Interface********************/
	SET_LCD_EN;

	if(WRData & 0x08)
		SET_LCD_D7;
	else
		CLR_LCD_D7;

	if(WRData & 0x04)
		SET_LCD_D6;
	else
		CLR_LCD_D6;

	if(WRData & 0x02)
		SET_LCD_D5;
	else
		CLR_LCD_D5;

	if(WRData & 0x01)
		SET_LCD_D4;
	else
		CLR_LCD_D4;

	nop();
	nop();
	nop();
	nop();

	CLR_LCD_EN;
	nop();
	nop();
	nop();
	nop();
	SET_LCD_EN;

	if(WRData & 0x08)
		SET_LCD_D7;
	else
		CLR_LCD_D7;

	if(WRData & 0x04)
		SET_LCD_D6;
	else
		CLR_LCD_D6;

	if(WRData & 0x02)
		SET_LCD_D5;
	else
		CLR_LCD_D5;

	if(WRData & 0x01)
		SET_LCD_D4;
	else
		CLR_LCD_D4;

	nop();
	nop();
	nop();
	nop();

	CLR_LCD_EN;
	nop();
	nop();
	nop();
	nop();
	/********************************************************/
}

/**
  * @brief  function to enable lcd
  * @param  void
  * @retval void
  */

static void get_lcd_ready (void)
{
	delay_ms(1);
	CLR_LCD_EN;
	nop();
	nop();
	nop();

}

/**
  * @brief  function to print string on lcd
  * @param  *str : char array to be print on lcd
  * 		example :
  * 		lcd_print("Welcome\trobocon\n2013!");
  * @retval void
  */

void lcd_print (char *str)
{
	uint8_t line = 0;
	set_lcd (INST_REG,0x02);	// Return Cursor to Home
	for (; *str != '\0'; str ++)
	{
		if (*str == '\n')
		{
			line ++;
			switch (line)
			{
				case 1: set_lcd (INST_REG, 0xC0); //C0
						break;
				case 2: set_lcd (INST_REG, 0x94); //94
						break;
				case 3: set_lcd (INST_REG, 0xD4); //D4
						break;
			}
		}
		else
		{
			set_lcd (DATA_REG, *str);
		}
	}
}

/**
  * @brief  clear lcd screen
  * @param  void
  * @retval void
  */
void clear_lcd (void)
{
	set_lcd(INST_REG,0x01);
}

