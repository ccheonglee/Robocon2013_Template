/**
  ******************************************************************************
  * @file    mcp3208.c
  * @author  Lee Chee Cheong
  * @version V1.0
  * @date    08/22/2010
  * @brief   This file provides 12bits adc driver for mcp320x
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc_mcp3208.h"
#include "stdio.h"

static void ADC_setSpi (void);
static uint8_t ADC_sendByte (uint8_t dataOut);
static uint16_t adcBuffer[16];

/**
  * @brief  setup mcp3208 interface .(spi pin/bit banging pin etc)
  * @param  none
  * @retval void
  */
void ADC_init (void)
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


	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	/*configure cs1 pin*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*configure cs2 pin*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	SET_ADC_CS1;           //set cs high for mcp3208
	SET_ADC_CS2;

	ADC_setSpi ();
}

/**
  * @brief  setup mcp3208 spi interface and mode
  * @param  none
  * @retval void
  */
static void ADC_setSpi (void)
{
   SPI_InitTypeDef   SPI_InitStructure;

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

   /* DISABLE SPI1 */
   SPI_Cmd(SPI1, DISABLE);
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;

   /* Apply spi configuration after enabling it */
   SPI_Init(SPI1, &SPI_InitStructure);
   /* Enable SPI1 */
   SPI_Cmd(SPI1, ENABLE);
}

/**
  * @brief  read adc from 12bits adc(mcp320x) on a particular channel
  * @param  adc number (0-7), and channel number of that particular adc (0-7)
            maximum readings, number of sample and so on
            adcChannel: ADC_CHx where x = 0, 1, 2, 3, 4, 5, 6 or 7
  * @retval 0 if no error, 1 if any error occur
  */
uint16_t ADC_read (uint16_t adcChannel)
{
   return adcBuffer[adcChannel];
}

/**
  * @brief  send a byte via spi and receive a byte from spi (full duplex)
  * @param  a 8 bits data to send
  * @retval a 8 bits data receive from spi
  */
uint8_t ADC_sendByte (uint8_t dataOut)
{
   while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
   SPI_I2S_SendData(SPI1, dataOut);
   while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
   return SPI_I2S_ReceiveData(SPI1);
}

/**
  * @brief  this is a update function. will be called from sensor task.
  * @param  update all 16 channel adc
  * @retval none
  */
void ADC_update (void)
{
	uint16_t startByte, adcChannel;
	uint16_t adcReadback;

	for(adcChannel = 0; adcChannel < 8; adcChannel++){
		startByte = 0x0600 | adcChannel<<6;
		CLR_ADC_CS2;
		ADC_sendByte ((uint8_t)(startByte>>8));
		adcReadback = ADC_sendByte ((uint8_t)(startByte&0xFF));
		adcReadback <<= 8;
		adcReadback |= ADC_sendByte (0x00);
		adcReadback &= 0x0FFF;
		SET_ADC_CS2;               //disable all mcp3208
		adcBuffer[adcChannel] = adcReadback;
	}
	for(adcChannel = 0; adcChannel < 8; adcChannel++){
		startByte = 0x0600 | adcChannel<<6;
		CLR_ADC_CS1;
		ADC_sendByte (startByte>>8);
		adcReadback = ADC_sendByte (startByte&0xFF);
		adcReadback <<= 8;
		adcReadback |= ADC_sendByte (0x00);
		adcReadback &= 0x0FFF;
		SET_ADC_CS1;               //disable all mcp3208
		adcBuffer[adcChannel + 8] = adcReadback;
	}
}
