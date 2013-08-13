
/**
  ******************************************************************************
  * @file    DAC124S085.c
  * @author  Eugene
  * @date    11/26/2012
  * @brief   This file provides 12bits  DAC124S085
  ******************************************************************************
  */

/*
 * This file provides dac configurations
 * Have to initialize by writing DAC_init();
 * Send the data to dac by writing DAC_send(dacChannel, step, GPIOx, GPIO_Pin);
 */

/* Includes ------------------------------------------------------------------*/
#include "dac_124s085.h"
#include <stdio.h>

/**
  * @brief  setup dac124s085 interface
  * @param GPIOx : DAC chip selection(port)
  * DAC1_Port for DAC1 CS port
  * DAC2_Port for DAC2 CS port
  * GPIO_Pin : DAC chip selection(pin)
  * DAC1_Pin for DAC1 CS pin
  * DAC2_Pin for DAC2 CS pin
  * @retval void
  */
void DAC_init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);
	/*configure  clk pin, cs pin and DI pin*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	SET_DAC_CLK;
	SET_DAC_DI;
	SET_DAC1_SYNC;
	SET_DAC2_SYNC;

	DAC_send(DAC_VEXTA1, 0);
	DAC_send(DAC_VEXTA2, 0);
	DAC_send(DAC_VEXTA3, 0);
	DAC_send(DAC_VEXTA4, 0);
	DAC_send(DAC_VEXTA5, 0);
	DAC_send(DAC_VEXTA6, 0);
}


/**
  * @brief  send 12bits data to dac124s085 on a particular channel
  * @param  dac step(0-4095), and vexta channel number
  * DAC_VEXTA1 0
  * DAC_VEXTA2 1
  * DAC_VEXTA3 2
  * DAC_VEXTA4 3
  * DAC_VEXTA5 4
  * DAC_VEXTA6 5
  * @retval void
  */

void DAC_send(uint16_t vextaChannel, uint16_t step)
{
	uint16_t dacOutput;
	uint16_t i;

	// pull sck and data to high
	SET_DAC_CLK;
	SET_DAC_DI;

	// protection, if user set more than 4095, cap the value max at 4095, which is 12 bits max value.
	if (step > 4095)
		step = 4095;

	// pull sync low. if vexta channel > 3, pull second dac low, other wise, 1st dac low.
	// if first bit of vexta channel is 1, it belongs to vexta channel 3, 4 or 5.
	if (vextaChannel & 0x0001){
		CLR_DAC2_SYNC;
		// mask the last bit to 0, since last bit is to used as indicator only. not as actual dac channel
		// after that, or the step with channel.
		dacOutput = ((vextaChannel & 0xFFFE) | step);
	}
	else{
		CLR_DAC1_SYNC;
		// mask the last bit to 0, since last bit is to used as indicator only. not as actual dac channel
		// after that, or the step with channel.
		dacOutput = ((vextaChannel & 0xFFFE) | step);
	}

	for (i = 0; i < 16; i ++){
		// give a delay to clock signal, since optocoupler is having maximum operating frequency
		// frequency generated shouldn't be more than that.
		SET_DAC_CLK;

		if(dacOutput & 0x8000)
			SET_DAC_DI;
		else
			CLR_DAC_DI;
		dacOutput <<= 1;

		// give a delay to clock signal, since optocoupler is having maximum operating frequency
		// frequency generated shouldn't be more than that.
		CLR_DAC_CLK;
	}

	// pull sync high. if vexta channel > 3, pull second dac high, other wise, 1st dac high.
	// if first bit of vexta channel is 1, it belongs to vexta channel 3, 4 or 5.
	if (vextaChannel & 0x0001)
		SET_DAC2_SYNC;
	else
		SET_DAC1_SYNC;

	// pull clock and data to high, to save power consumption. if signal = 0, according to schematics
	// the microcontroller will keep sinking current.
	SET_DAC_CLK;
	SET_DAC_DI;
}
