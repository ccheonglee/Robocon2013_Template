#ifndef __MCP3208_H
#define __MCP3208_H

#include "stm32f4xx.h"

#define ADC12_setSpi MCP3208_setSpi
#define ADC12_read(x) MCP3208_read(x)
#define ADC12_init MCP3208_init

#define SET_ADC_CS1 	GPIO_SetBits (GPIOA, GPIO_Pin_4)
#define CLR_ADC_CS1 	GPIO_ResetBits (GPIOA, GPIO_Pin_4)
#define SET_ADC_CS2 	GPIO_SetBits (GPIOC, GPIO_Pin_2)
#define CLR_ADC_CS2 	GPIO_ResetBits (GPIOC, GPIO_Pin_2)


#define ADC_CH1		0
#define ADC_CH2		1
#define ADC_CH3		2
#define ADC_CH4		3
#define ADC_CH5		4
#define ADC_CH6		5
#define ADC_CH7		6
#define ADC_CH8		7
#define ADC_CH9		8
#define ADC_CH10	9
#define ADC_CH11	10
#define ADC_CH12	11
#define ADC_CH13	12
#define ADC_CH14	13
#define ADC_CH15	14
#define ADC_BATT	15

void ADC_init (void);
uint16_t ADC_read (uint16_t adcChannel);
void ADC_update(void);

#endif
