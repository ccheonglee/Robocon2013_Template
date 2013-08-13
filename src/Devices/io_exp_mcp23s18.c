/*
  * @file    io_exp_mcp23s18.c
  * @author  Boon
  * @date    Nov 28, 2012
  * @brief   This file provides a set of functions to configure io expander using spi.
 */

/*
 * CHANGE LOG:
 * 24 March 2013	:All OUTPUT Port will be updated only if there a change. If there is no change,
 * 					no update on io port will be performed.
 */

/*
 * This file provides io expander configurations
 * Have to initialize it by writing IoExpSpi_init();
 * Write data to GPIO pin through IoExpSpi_Write(interface_protocol, data_to_Write, ioSelect);
 * Read data from GPIO pin through IoExpSpi_Read(interface_protocol, ioSelect);
 */
/* Includes ------------------------------------------------------------------*/
#include "io_exp_mcp23s18.h"
#include <stdio.h>

#define	RETRY	3

static void IoExp_setSpi (void);
static uint8_t IoExp_Read(int8_t reg);
static void IoExp_Write(int8_t reg, uint8_t data);
static uint8_t spiSend (int8_t data);

static uint8_t shadowOutRegA[3] = {0};
static uint8_t shadowOutRegB[3] = {0};
static uint8_t OutRegAChanged[3] = {0};
static uint8_t OutRegBChanged[3] = {0};

static uint8_t shadowIodirRegA[3] = {0xFF};
static uint8_t shadowIodirRegB[3] = {0xFF};

static uint8_t shadowPuRegA[3] = {0};
static uint8_t shadowPuRegB[3] = {0};

static uint8_t shadowGPIOA[3] = {0};
static uint8_t shadowGPIOB[3] = {0};

/**
  * @brief  setup IO expander
  * @param 	void
  * @retval void
  */
void IoExp_init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	/* Configure SPIx_Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	/* Configure GPIO PIN for IO Expander Chip select and Reset */
	/*configure io_c0 cs pin, cs1 reset pin*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/*configure io_e14 and io_e15 cs pin, io_e12 and io_e13 reset pin*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	CS_HIGH_U22;
	CS_HIGH_U25;
	CS_HIGH_U26;

	RESET_HIGH_U22;
	RESET_HIGH_U25;
	RESET_HIGH_U26;

	IoExp_setSpi ();

	// initialize all the IO direction to input as open drain. TO ALL IO_EXPANDER
	CS_LOW_U22;
	IoExp_Write(IO_EXP_IODIRA, 0xFF);		// set all pins as input
	CS_HIGH_U22;
	CS_LOW_U22;
	IoExp_Write(IO_EXP_IODIRB, 0xFF);
	CS_HIGH_U22;
	CS_LOW_U22;
	IoExp_Write(IO_EXP_GPPUA, 0x00);			// disable pull up
	CS_HIGH_U22;
	CS_LOW_U22;
	IoExp_Write(IO_EXP_GPPUB, 0x00);
	CS_HIGH_U22;

	CS_LOW_U25;
	IoExp_Write(IO_EXP_IODIRA, 0xFF);		// set all pins as input
	CS_HIGH_U25;
	CS_LOW_U25;
	IoExp_Write(IO_EXP_IODIRB, 0xFF);
	CS_HIGH_U25;
	CS_LOW_U25;
	IoExp_Write(IO_EXP_GPPUA, 0x00);			// disable pull up
	CS_HIGH_U25;
	CS_LOW_U25;
	IoExp_Write(IO_EXP_GPPUB, 0x00);
	CS_HIGH_U25;

	CS_LOW_U26;
	IoExp_Write(IO_EXP_IODIRA, 0xFF);		// set all pins as input
	CS_HIGH_U26;
	CS_LOW_U26;
	IoExp_Write(IO_EXP_IODIRB, 0xFF);
	CS_HIGH_U26;
	CS_LOW_U26;
	IoExp_Write(IO_EXP_GPPUA, 0x00);			// disable pull up
	CS_HIGH_U26;
	CS_LOW_U26;
	IoExp_Write(IO_EXP_GPPUB, 0x00);
	CS_HIGH_U26;

	/*	DEBUG PURPOSE
	uint8_t gpioa, gpiob;
	while (1){
		CS_LOW_U25;
		gpioa = IoExp_Read(IO_EXP_GPIOA);
		CS_HIGH_U25;
		CS_LOW_U25;
		gpiob = IoExp_Read(IO_EXP_GPIOB);
		CS_HIGH_U25;
		printf ("gpioa = %X, gpiob = %X\r\n", gpioa, gpiob);
		Delaynus(1000000);
	}
	*/
}

/**
  * @brief  setup SPI for IO expander
  * @param 	void
  * @retval void
  */
void IoExp_setSpi (void)
{
	SPI_InitTypeDef   SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* DISABLE SPI1 */
	SPI_Cmd(SPI1, DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;

	/* Apply spi configuration after enabling it */
	SPI_Init(SPI1, &SPI_InitStructure);
	/* Enable SPI1 */
	SPI_Cmd(SPI1, ENABLE);
}

/**
  * @brief  write data to the IO expander
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
			OLATB
  * ioSelect : IO_C0 , IO_E14 or IO_E15
  * @retval void
  */
void IoExp_Write(int8_t reg, uint8_t data)
{
	spiSend (0x40);		//write command
	spiSend (reg);		//write register address
	spiSend (data);		//write data
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
uint8_t IoExp_Read(int8_t reg)
{
	uint8_t dataIn;
	spiSend (0x41);			//write command
	spiSend (reg);				//write register address
	dataIn = spiSend (0x00);	//write dummy data to read data
	return dataIn;
}

uint8_t spiSend (int8_t data)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	/* Send a Byte through the SPI peripheral */
	SPI_I2S_SendData(SPI1, data);

	/* Wait to receive a Byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the Byte read from the SPI bus */
	return (uint8_t)SPI_I2S_ReceiveData(SPI1);
}

void setGPIOIODir (uint8_t gpio, uint8_t inputOutput)
{
	uint8_t ioExpNumber, ioPort;

	// turn the CS to low accordingly.
	if(gpio < GPA0_2)
		CS_LOW_U22;
	else if(gpio > GPB0 && gpio <= GPB0_2)
		CS_LOW_U25;
	else if(gpio > GPB0_2 && gpio <= GPIO_PWM12)
		CS_LOW_U26;
	else
		return;

	// each io expander come with 16 IOs, with 2 ports. Each port 8 IOs.
	// IO 0 -> 15 = ioExpNumber 0, IO 16 -> 31 = ioExpNumber 1, IO32 -> 47 = ioExpNumber 2
	ioExpNumber = gpio/16;

	// find the ioPorts for the IC.
	ioPort = gpio%16;

	// write to the io expander, if ioPort less than 8, it is on gpioa of io expander
	if (ioPort < 8){
		if(inputOutput == IO_INPUT)
			shadowIodirRegA[ioExpNumber] |= 1<<ioPort;
		else
			shadowIodirRegA[ioExpNumber] &= ~(1<<ioPort);
		IoExp_Write(IO_EXP_IODIRA, shadowIodirRegA[ioExpNumber]);
	}
	// write to the io expander, if ioPort at 8 and onwards, it is on gpiob of io expander
	else{
		ioPort -=8;
		if(inputOutput == IO_INPUT)
			shadowIodirRegB[ioExpNumber] |= 1<<ioPort;
		else
			shadowIodirRegB[ioExpNumber] &= ~(1<<ioPort);
		IoExp_Write(IO_EXP_IODIRB, shadowIodirRegB[ioExpNumber]);
	}

	// turn the CS to high accordingly.
	if(gpio < GPA0_2)
		CS_HIGH_U22;
	else if(gpio > GPB0 && gpio <= GPB0_2)
		CS_HIGH_U25;
	else if(gpio > GPB0_2 && gpio <= GPIO_PWM12)
		CS_HIGH_U26;
	else
		return;
}

void setGPIOPullUp (uint8_t gpio, uint8_t status)
{
	uint8_t ioExpNumber, ioPort;

	// turn the CS to low accordingly.
	if(gpio < GPA0_2)
		CS_LOW_U22;
	else if(gpio > GPB0 && gpio <= GPB0_2)
		CS_LOW_U25;
	else if(gpio > GPB0_2 && gpio <= GPIO_PWM12)
		CS_LOW_U26;
	else
		return;

	// each io expander come with 16 IOs, with 2 ports. Each port 8 IOs.
	// IO 0 -> 15 = ioExpNumber 0, IO 16 -> 31 = ioExpNumber 1, IO32 -> 47 = ioExpNumber 2
	ioExpNumber = gpio/16;

	// find the ioPorts for the IC.
	ioPort = gpio%16;

	// write to the io expander, if ioPort less than 8, it is on gpioa of io expander
	if (ioPort < 8){
		if(status > 0)
			shadowPuRegA[ioExpNumber] |= 1<<ioPort;
		else
			shadowPuRegA[ioExpNumber] &= ~(1<<ioPort);
		IoExp_Write(IO_EXP_GPPUA, shadowPuRegA[ioExpNumber]);
	}
	// write to the io expander, if ioPort at 8 and onwards, it is on gpiob of io expander
	else{
		ioPort -=8;
		if(status > 0)
			shadowPuRegB[ioExpNumber] |= 1<<ioPort;
		else
			shadowPuRegB[ioExpNumber] &= ~(1<<ioPort);
		IoExp_Write(IO_EXP_GPPUB, shadowPuRegB[ioExpNumber]);
	}

	// turn the CS to high accordingly.
	if(gpio < GPA0_2)
		CS_HIGH_U22;
	else if(gpio > GPB0 && gpio <= GPB0_2)
		CS_HIGH_U25;
	else if(gpio > GPB0_2 && gpio <= GPIO_PWM12)
		CS_HIGH_U26;
	else
		return;
}

void GPIOWrite(uint8_t gpio, uint8_t bit)
{
	uint8_t ioExpNumber, ioPort;

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

		// set this flag to indicate there is change.
		OutRegAChanged[ioExpNumber] = 1;
	}
	// write to the io expander, if ioPort at 8 and onwards, it is on gpiob of io expander
	else{
		ioPort -=8;
		if(bit > 0)
			shadowOutRegB[ioExpNumber] |= 1<<ioPort;
		else
			shadowOutRegB[ioExpNumber] &= ~(1<<ioPort);

		// set this flag to indicate there is change.
		OutRegBChanged[ioExpNumber] = 1;
	}
}

uint8_t GPIORead(uint8_t gpio)
{
	uint8_t ioPort, icNumber;
	uint8_t readback;

	// find which ic this io belongs to
	icNumber = gpio/16;

	// find whic io port this io belongs to.
	ioPort = gpio%16;

	// read from io expander, if ioPort less than 8, it is on gpioa of io expander
	if (ioPort < 8)
		readback = shadowGPIOA[icNumber];
	// read from the io expander, if ioPort at 8 and onwards, it is on gpiob of io expander
	else{
		ioPort -= 8;
		readback = shadowGPIOB[icNumber];
	}

	// check if the io is on or off
	if(readback & (1<<ioPort))
		readback = 1;
	else
		readback = 0;

	return readback;
}

/**
  * @brief  this is a update function. will be called from sensor task.
  * @param  update all 48 io expander input
  * @retval none
  */
void IoExpInput_update (void)
{
	uint8_t readback;

	// read from U22, and update it to shadow register. user will read from shadow register
	// BOTH GPIOA and GPIOB are read
	CS_LOW_U22;
	readback = IoExp_Read(IO_EXP_GPIOA);
	CS_HIGH_U22;
	shadowGPIOA[0] = readback;

	asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

	CS_LOW_U22;
	readback = IoExp_Read(IO_EXP_GPIOB);
	CS_HIGH_U22;
	shadowGPIOB[0] = readback;

	// read from U25, and update it to shadow register. user will read from shadow register
	// BOTH GPIOA and GPIOB are read
	CS_LOW_U25;
	readback = IoExp_Read(IO_EXP_GPIOA);
	CS_HIGH_U25;
	shadowGPIOA[1] = readback;

	asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

	CS_LOW_U25;
	readback = IoExp_Read(IO_EXP_GPIOB);
	CS_HIGH_U25;
	shadowGPIOB[1] = readback;

	// read from U26, and update it to shadow register. user will read from shadow register
	// BOTH GPIOA and GPIOB are read
	CS_LOW_U26;
	readback = IoExp_Read(IO_EXP_GPIOA);
	CS_HIGH_U26;
	shadowGPIOA[2] = readback;

	asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

	CS_LOW_U26;
	readback = IoExp_Read(IO_EXP_GPIOB);
	CS_HIGH_U26;
	shadowGPIOB[2] = readback;
}

/**
  * @brief  this is a update function. will be called from sensor task.
  * @param  update all 48 io expander output
  * @retval none
  */
void IoExpOutput_update (void)
{
	uint8_t readback, i;
	// read from U22, and update it to shadow register. user will read from shadow register
	// BOTH GPIOA and GPIOB are read

	// check the flag if we need to update this IO port
	if (OutRegAChanged[0]){
		for (i = 0; i< RETRY; i++){
			// write to output latch
			CS_LOW_U22;
			IoExp_Write(IO_EXP_OLATA, shadowOutRegA[0]);
			CS_HIGH_U22;

			asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

			// read back from io expander
			CS_LOW_U22;
			readback = IoExp_Read(IO_EXP_OLATA);
			CS_HIGH_U22;

			// if what you write and what you read is the same, break the loop, else try 3 times
			if (readback == shadowOutRegA[0])
				break;
		}

		// clear the flag once the IO Port is updated.
		OutRegAChanged[0] = 0;
	}
	// check the flag if we need to update this IO port
	if (OutRegBChanged[0]){
		for (i = 0; i< RETRY; i++){
			// write to output latch
			CS_LOW_U22;
			IoExp_Write(IO_EXP_OLATB, shadowOutRegB[0]);
			CS_HIGH_U22;

			asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

			// read back from io expander
			CS_LOW_U22;
			readback = IoExp_Read(IO_EXP_OLATB);
			CS_HIGH_U22;

			// if what you write and what you read is the same, break the loop, else try 3 times
			if (readback == shadowOutRegB[0])
				break;
		}
		// clear the flag once the IO Port is updated.
		OutRegBChanged[0] = 0;
	}


	// read from U25, and update it to shadow register. user will read from shadow register
	// BOTH GPIOA and GPIOB are read

	// check the flag if we need to update this IO port
	if (OutRegAChanged[1]){
		for (i = 0; i< RETRY; i++){
			// write to output latch
			CS_LOW_U25;
			IoExp_Write(IO_EXP_OLATA, shadowOutRegA[1]);
			CS_HIGH_U25;

			asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

			// read back from io expander
			CS_LOW_U25;
			readback = IoExp_Read(IO_EXP_OLATA);
			CS_HIGH_U25;

			// if what you write and what you read is the same, break the loop, else try 3 times
			if (readback == shadowOutRegA[1])
				break;
		}

		// clear the flag once the IO Port is updated.
		OutRegAChanged[1] = 0;
	}
	// check the flag if we need to update this IO port
	if (OutRegBChanged[1]){
		for (i = 0; i< RETRY; i++){
			// write to output latch
			CS_LOW_U25;
			IoExp_Write(IO_EXP_OLATB, shadowOutRegB[1]);
			CS_HIGH_U25;

			asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

			// read back from io expander
			CS_LOW_U25;
			readback = IoExp_Read(IO_EXP_OLATB);
			CS_HIGH_U25;

			// if what you write and what you read is the same, break the loop, else try 3 times
			if (readback == shadowOutRegB[1])
				break;
		}

		// clear the flag once the IO Port is updated.
		OutRegBChanged[1] = 0;
	}


	// read from U26, and update it to shadow register. user will read from shadow register
	// BOTH GPIOA and GPIOB are read'

	// check the flag if we need to update this IO port
	if (OutRegAChanged[2]){
		for (i = 0; i< RETRY; i++){
			// write to output latch
			CS_LOW_U26;
			IoExp_Write(IO_EXP_OLATA, shadowOutRegA[2]);
			CS_HIGH_U26;

			asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

			// read back from io expander
			CS_LOW_U26;
			readback = IoExp_Read(IO_EXP_OLATA);
			CS_HIGH_U26;

			// if what you write and what you read is the same, break the loop, else try 3 times
			if (readback == shadowOutRegA[2])
				break;
		}

		// clear the flag once the IO Port is updated.
		OutRegAChanged[2] = 0;
	}
	// check the flag if we need to update this IO port
	if (OutRegBChanged[2]){
		for (i = 0; i< RETRY; i++){
			// write to output latch
			CS_LOW_U26;
			IoExp_Write(IO_EXP_OLATB, shadowOutRegB[2]);
			CS_HIGH_U26;

			asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");

			// read back from io expander
			CS_LOW_U26;
			readback = IoExp_Read(IO_EXP_OLATB);
			CS_HIGH_U26;

			// if what you write and what you read is the same, break the loop, else try 3 times
			if (readback == shadowOutRegB[2])
				break;
		}
		// clear the flag once the IO Port is updated.
		OutRegBChanged[2] = 0;
	}
}
