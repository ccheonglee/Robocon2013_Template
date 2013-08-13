/**
  ******************************************************************************
  * @file    delay.c
  * @author  Eugene
  * @date    Dec 5, 2012
  * @brief   This file provides delay functions
  ******************************************************************************
  */

/*
 * This file provides delay in miliseconds and microseconds
 * create delay by writing delay_ms(ms); or delay_us(us);
 */

/* Includes ------------------------------------------------------------------*/
#include "delay.h"

/**
* @brief  Initialize timer for delay purpose. timer 6 is used
* @param  none
* @retval None
*/
void delay_init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* TIM6 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

	/* Time calculation: timer frequency = 84MHz, to get 1ms, the counter must be = 84MHz * 1ms
	 *  = 84000. It is 16 bits timer, 84000 is out of counting range. So counter = 8400, and
	 *  prescaler = 10. To hit 84000 counting time.
	 */
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 8400 - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 10 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_SetCounter (TIM6, 0);

	/* TIM3 enable counter */
	TIM_Cmd(TIM6, ENABLE);
}

void Delaynus(vu32 nus)
{
    uint8_t nCount;
    while (nus--)
        for (nCount = 6; nCount != 0; nCount--);
}

/**
* @brief  Inserts a estimated delay time in mS. CPU FREQ = 168MHz
* @param  miliseconds: specifies the delay time in mili second.
* @retval None
*/
void delay_ms(uint32_t miliseconds)
{
	uint32_t i;

	// clear timer 6 to 0. so it can count from 0.
	TIM_SetCounter (TIM6, 0);

	// one loop = 1ms. so loop for x loop to hit the delay.
	for(i=0; i < miliseconds; i++){
		Delaynus(1000);
		continue;
		// this is to wait overflow from the timer. this is auto reload timer,
		// thus no reload is required.
		while (TIM_GetFlagStatus(TIM6, TIM_FLAG_Update) != SET);
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	}
}

