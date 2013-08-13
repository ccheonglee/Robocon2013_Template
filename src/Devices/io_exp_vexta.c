/**
  ******************************************************************************
  * @file    io_exp_vexta.c
  * @author  Eugene
  * @date    Dec 21, 2012
  * @brief   This file provides vexta io expander code
  ******************************************************************************
  */

/*
 * This file provides io expander configurations for vexta.
 * Have to initialize it by writing IoExpVexta_init();
 * Write data to GPIO pin through IoExpVexta_Write(interface_protocol, data_to_Write, ioSelect);
 * Read data from GPIO pin through IoExpVexta_Read(interface_protocol, ioSelect);
 */

/* Includes ------------------------------------------------------------------*/
#include "io_exp_vexta.h"
#include <stdio.h>


static uint8_t shadowOutRegA[2] = {0xF8, 0xF8};
static uint8_t shadowOutRegB[2] = {0xFF, 0xFF};

static uint8_t softSpiSend (uint8_t dataOut);
static void IoExpVexta_Write(uint8_t reg, uint8_t data);
static uint8_t IoExpVexta_Read(uint8_t reg);

/**
  * @brief  setup IO expander for vexta
  * @param 	void
  * @retval void
  */
void IoExpVexta_init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// configure io expander SO pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// configure io expander CS1 and RST1 pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// configure io expander CS2 and RST2 pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_3 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// configure io expander CS1 and RST1 pin
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	CLR_IOE_RESET1;
	SET_IOE_RESET1;
	CLR_IOE_RESET2;
	SET_IOE_RESET2;

	SET_IOE_CS1;
	SET_IOE_CS2;

	SET_IOE_SCK;
	SET_IOE_SI;

	// initialize all the IO direction to input as open drain. TO ALL IO_EXPANDER
	CLR_IOE_CS1;
	IoExpVexta_Write(IO_EXP_IODIRA, 0x07);		// pin GPA0,1,2 as input. the rest as output
	SET_IOE_CS1;

	CLR_IOE_CS1;
	IoExpVexta_Write(IO_EXP_IODIRB, 0x00);		// all are output
	SET_IOE_CS1;

	CLR_IOE_CS1;
	IoExpVexta_Write(IO_EXP_GPPUA, 0xFF);		// enable pull up
	SET_IOE_CS1;

	CLR_IOE_CS1;
	IoExpVexta_Write(IO_EXP_GPPUB, 0xFF);		// enable pull up
	SET_IOE_CS1;

	CLR_IOE_CS1;
	IoExpVexta_Write(IO_EXP_OLATA, 0xF8);		// output port a 11111000
	SET_IOE_CS1;

	CLR_IOE_CS1;
	IoExpVexta_Write(IO_EXP_OLATB, 0xFF);		// output port b 11111111
	SET_IOE_CS1;

	// write to IC2
	CLR_IOE_CS2;
	IoExpVexta_Write(IO_EXP_IODIRA, 0x07);		// pin GPA0,1,2 as input. the rest as output
	SET_IOE_CS2;

	CLR_IOE_CS2;
	IoExpVexta_Write(IO_EXP_IODIRB, 0x00);		// all gpb are output
	SET_IOE_CS2;

	CLR_IOE_CS2;
	IoExpVexta_Write(IO_EXP_GPPUA, 0xFF);		// enable pull up
	SET_IOE_CS2;

	CLR_IOE_CS2;
	IoExpVexta_Write(IO_EXP_GPPUB, 0xFF);		// enable pull up
	SET_IOE_CS2;

	CLR_IOE_CS2;
	IoExpVexta_Write(IO_EXP_OLATA, 0xF8);		// output port a 11111000
	SET_IOE_CS2;

	CLR_IOE_CS2;
	IoExpVexta_Write(IO_EXP_OLATB, 0xFF);		// output port b 11111111
	SET_IOE_CS2;
}

static void IoExpVexta_Write(uint8_t reg, uint8_t data)
{
	softSpiSend (0x40);		//write command
	softSpiSend (reg);		//write register address
	softSpiSend (data);		//write data
}

/**
  * @brief  read data from IO expander GPIO
  * @param 	interface protocol is the registers in IO expander
  * 		IODIRA			 0x00
			IODIRB           0x01
			IPOLA            0x02
			IPOLB            0x03
			GPINTENA         0x04
			GPINTENB         0x05
			DEFVALA          0x06
			DEFVALB          0x07
			INTCONA          0x08
			INTCONB          0x09
			IOCONA           0x0A
			IOCONB           0x0B
			GPPUA            0x0C
			GPPUB            0x0D
			INTFA            0x0E
			INTFB            0x0F
			INTCAPA          0x010
			INTCAPB          0x011
			GPIO_A           0x012
			GPIO_B           0x013
			OLATA            0x014
			OLATB		     0x015
  * ioSelect : IO_C0 , IO_E14 or IO_E15
  * @retval void
  */
static uint8_t IoExpVexta_Read(uint8_t reg)
{
	uint8_t dataIn;
	softSpiSend (0x41);			//write command
	softSpiSend (reg);				//write register address
	dataIn = softSpiSend (0x00);	//write dummy data to read data
	return dataIn;
}

static uint8_t softSpiSend (uint8_t dataOut)
{
	uint8_t dataIn = 0, i;

	SET_IOE_SCK;
	SET_IOE_SI;
	for (i=0; i<8; i++){
		// clock data out after falling edge
		CLR_IOE_SCK;

		if(dataOut & 0x80)
			SET_IOE_SI;
		else
			CLR_IOE_SI;
		dataOut <<= 1;

		SET_IOE_SCK;

		// read data in after rising edge
		dataIn <<= 1;
		dataIn |= READ_MISO;
	}
	SET_IOE_SCK;
	SET_IOE_SI;

	return dataIn;
}

void VextaGPIOWrite(uint8_t gpio, uint8_t bit)
{
	uint8_t ioExpNumber, ioPort;

	// turn the CS to low accordingly.
	if(gpio < ALARM_OUT4)
		CLR_IOE_CS1;
	else if(gpio > IOVEXTA_GPB7 && gpio <= IOVEXTA_GPB7_2)
		CLR_IOE_CS2;
	else
		return;

	// each io expander come with 16 IOs, with 2 ports. Each port 8 IOs.
	// IO 0 -> 15 = ioExpNumber 0, IO 16 -> 31 = ioExpNumber 1, IO32 -> 47 = ioExpNumber 2
	ioExpNumber = gpio/16;

	// find the ioPorts for the IC.
	ioPort = gpio%16;

	// write to the io expander, if ioPort less than 8, it is on gpioa of io expander
	if (ioPort < 8){
		if(bit > 0)
			shadowOutRegA[ioExpNumber] |= 1<<ioPort;
		else
			shadowOutRegA[ioExpNumber] &= ~(1<<ioPort);
		IoExpVexta_Write(IO_EXP_OLATA, shadowOutRegA[ioExpNumber]);
	}
	// write to the io expander, if ioPort at 8 and onwards, it is on gpiob of io expander
	else{
		ioPort -=8;
		if(bit > 0)
			shadowOutRegB[ioExpNumber] |= 1<<ioPort;
		else
			shadowOutRegB[ioExpNumber] &= ~(1<<ioPort);
		IoExpVexta_Write(IO_EXP_OLATB, shadowOutRegB[ioExpNumber]);
	}

	// turn the CS to high accordingly.
	if(gpio < ALARM_OUT4)
		SET_IOE_CS1;
	else if(gpio > IOVEXTA_GPB7 && gpio <= IOVEXTA_GPB7_2)
		SET_IOE_CS2;
	else
		return;
}

uint8_t VextaGPIORead (uint8_t gpio)
{
	uint8_t ioPort;
	uint8_t readback;

	// turn the CS to low accordingly.
	if(gpio < ALARM_OUT4)
		CLR_IOE_CS1;
	else if(gpio > IOVEXTA_GPB7 && gpio <= IOVEXTA_GPB7_2)
		CLR_IOE_CS2;
	else
		return 255;

	// find the ioPorts for the IC.
	ioPort = gpio%16;

	// read from io expander, if ioPort less than 8, it is on gpioa of io expander
	if (ioPort < 8)
		readback = IoExpVexta_Read(IO_EXP_GPIOA);
	// read from the io expander, if ioPort at 8 and onwards, it is on gpiob of io expander
	else
		readback = IoExpVexta_Read(IO_EXP_GPIOB);

	// turn the CS to high accordingly.
	if(gpio < ALARM_OUT4)
		SET_IOE_CS1;
	else if(gpio > IOVEXTA_GPB7 && gpio <= IOVEXTA_GPB7_2)
		SET_IOE_CS2;
	else
		return 255;

	return readback;
}

