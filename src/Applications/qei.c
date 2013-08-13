/*
  ******************************************************************************
  * @file    qei.c
  * @author  Boon
  * @date    Dec 4, 2012
  * @brief   This file provides quadrature encoder configuration
  ******************************************************************************
 */

/*
 * This file provide three encoder configuration.
 * Have to initialize the related encoder before using it by writing ENx_init(); ,where x = 1, 2 or 3.
 * Check the ENxPulses to get the encoder value
 * The pulses is increment when one turn of encoder is completed.
 */

/* Includes ------------------------------------------------------------------*/
#include "qei.h"
//#include "stm32f4xx_tim.h"

static int32_t qeiFeedback[3] = {0};

/**
  * @brief  Encoder2 timer initialization
  * @param  void
  * @retval void
  * @brief
  * PB4  --> TIM3 CH1
  * PB5  --> TIM3 CH2
  */

void QEI1_init (void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM3 clock source enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Enable GPIOB, clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PB.4,5 as encoder alternate function */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect TIM3 pins to AF2 */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_TIM3);

	/* Enable the TIM3 Update Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Timer configuration in Encoder mode */
	TIM_DeInit(TIM3);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling
	TIM_TimeBaseStructure.TIM_Period = (4*4000)-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12,
							 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	// Clear all pending interrupts
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	//Reset counter
	TIM3->CNT = 0;      //encoder value

	TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  Encoder3 timer initialization
  * @param  void
  * @retval void
  * @brief
  * PC6  --> TIM8 CH1
  * PC7  --> TIM8 CH2
  */
void QEI2_init (void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM8 clock source enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	/* Enable GPIOC, clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PC.6,7 as encoder alternate function */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect TIM8 pins to AF3 */
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);

	/* Timer configuration in Encoder mode */
	TIM_DeInit(TIM8);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling
	TIM_TimeBaseStructure.TIM_Period = (4*4000)-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12,
							 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM8, &TIM_ICInitStructure);

	// Initial interrupt structures
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_TIM13_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Clear all pending interrupts
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);

	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
	TIM8->CNT = 0;
	TIM_Cmd(TIM8, ENABLE);

}

void QEI3_init (void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM1 clock source enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* Enable GPIOE, clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PE.9,11 as encoder alternate function */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* Connect TIM1 pins to AF1 */
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);

	/* Timer configuration in Encoder mode */
	TIM_DeInit(TIM1);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;  // No prescaling
	TIM_TimeBaseStructure.TIM_Period = (4*4000)-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12,
							 TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);

	// Initial interrupt structures
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	// Clear all pending interrupts
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);

	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM1->CNT = 0;
	TIM_Cmd(TIM1, ENABLE);

}

int32_t QEIGetFeedback (uint8_t qeiChannel)
{
	int32_t feedback;
	switch (qeiChannel){
	case QEI_CH1:
		feedback = qeiFeedback[qeiChannel] + TIM3->CNT;
		break;
	case QEI_CH2:
		feedback = qeiFeedback[qeiChannel] + TIM8->CNT;
		break;
	case QEI_CH3:
		feedback = qeiFeedback[qeiChannel] + TIM1->CNT;
		break;
	default:
		break;
	}
	return feedback;
}

/**
  * @brief  Encoder1 interrupt update
  * @param  void
  * @retval void
  */
void TIM3_IRQHandler(void)
{
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	if (TIM3->CR1&0x10)
		qeiFeedback[QEI_CH1] -= 16000;
	else
		qeiFeedback[QEI_CH1] += 16000;
}

/**
  * @brief  Encoder2 interrupt update
  * @param  void
  * @retval void
  */
void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM8, TIM_IT_Update) == SET){
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		if (TIM8->CR1&0x10)
			qeiFeedback[QEI_CH2] -= 16000;
		else
			qeiFeedback[QEI_CH2] += 16000;
	}
}

/**
  * @brief  Encoder3 interrupt update
  * @param  void
  * @retval void
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET){
		/* Clear TIM1 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		if (TIM1->CR1&0x10)
			qeiFeedback[QEI_CH3] -= 16000;
		else
			qeiFeedback[QEI_CH3] += 16000;
	}
}
