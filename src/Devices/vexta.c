/*
  ******************************************************************************
  * @file    vexta.c
  * @author  Boon
  * @date    Dec 4, 2012
  * @brief   This file provides vexta motor configuration
  ******************************************************************************
 * This file provides 6 vexta motors configurations.
 * Have to initialize the vexta before using it by writing Vexta_init();
 * Vexta can be controlled by changing the status followed by the speed
 * VextaSpeedCHx(Speed); and VextaStatusCHx(Status);
 * where x = 1, 2, 3, 4, 5 and 6
 * Speed: 0-4095
 * Status: FORWARD
 * 		   REVERSE
 * 		   HOLD
 */

/* Includes ------------------------------------------------------------------*/
#include "vexta.h"
#include "stm32f4xx_conf.h"
#include "dac_124s085.h"
#include "io_exp_vexta.h"

static uint32_t vextaFeedback[6][5];

static void VextaFeedback_init (void);

/**
  * @brief  setup vexta interface
  * @param  void
  * @retval void
  */
void Vexta_init (void)
{
	uint8_t i, j;
	IoExpVexta_init();
	DAC_init();
	VextaFeedback_init ();

	for (i=0; i<6; i++){
		for(j=0; j<5; j++)
			vextaFeedback[i][j] = 0;
	}
}

/**
  * @brief  initialize feedback interface for vexta motor. There are 6 channels, thus 6 external
  * 		interrupt to read motor feedback
  * @param  void
  * @retval void
  */
static void VextaFeedback_init (void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOE, B, D clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Configure PE1, PD7, PB12, PB13, PD10, PD11 pin as input pull up */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	//GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Connect MOTOR feedback to PE1, PD7, PB12, PB13, PD10, PD11, use EXTI to capture feedback */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource7);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource11);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);

	/* Configure EXTI Line1, Line7, Line10, Line11, Line12, Line13 */
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line10;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line1, Line7, Line10, Line11, Line12, Line13 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  set vextaCH1 speed
  * @param  vextaChannel, VEXTA_CH1, VEXTA_CH2, VEXTA_C3, VEXTA_CH4, VEXTA_CH5, VEXTA_CH6
  * 		speed 0-4095
  * @retval void
  */
void VextaSetSpeed(uint8_t vextaChannel, uint16_t speed)
{
	switch (vextaChannel){
	case VEXTA_CH1:
		DAC_send(DAC_VEXTA1, speed);
		break;
	case VEXTA_CH2:
		DAC_send(DAC_VEXTA2, speed);
		break;
	case VEXTA_CH3:
		DAC_send(DAC_VEXTA3, speed);
		break;
	case VEXTA_CH4:
		DAC_send(DAC_VEXTA4, speed);
		break;
	case VEXTA_CH5:
		DAC_send(DAC_VEXTA5, speed);
		break;
	case VEXTA_CH6:
		DAC_send(DAC_VEXTA6, speed);
		break;
	default:
		break;
	}
}

/**
  * @brief  start/brake/free the vexta motor
  * @param  vextaChannel: which vexta motor to start/stop
  * @retval void
  */
void VextaSetStatus(uint8_t vextaChannel, uint8_t status)
{
	uint8_t runBrake, startStop;

	// refer to vexta motor user manual, page 22, Input signals and output signals.
	// on how  motor operates depending to the run/brake and start/stop signals.
	if (status == MOTOR_RUN){
		runBrake = 0;
		startStop = 0;
	}
	else if (status == MOTOR_BRAKE){
		runBrake = 1;
		startStop = 0;
	}
	else{
		runBrake = 0;
		startStop = 1;
	}

	// set the run/brake and start/stop signals to corresponding channel
	switch (vextaChannel){
	case VEXTA_CH1:
		VextaGPIOWrite(STARTSTOP1, startStop);
		VextaGPIOWrite(RUNBRAKE1, runBrake);
		break;
	case VEXTA_CH2:
		VextaGPIOWrite(STARTSTOP2, startStop);
		VextaGPIOWrite(RUNBRAKE2, runBrake);
		break;
	case VEXTA_CH3:
		VextaGPIOWrite(STARTSTOP3, startStop);
		VextaGPIOWrite(RUNBRAKE3, runBrake);
		break;
	case VEXTA_CH4:
		VextaGPIOWrite(STARTSTOP4, startStop);
		VextaGPIOWrite(RUNBRAKE4, runBrake);
		break;
	case VEXTA_CH5:
		VextaGPIOWrite(STARTSTOP5, startStop);
		VextaGPIOWrite(RUNBRAKE5, runBrake);
		break;
	case VEXTA_CH6:
		VextaGPIOWrite(STARTSTOP6, startStop);
		VextaGPIOWrite(RUNBRAKE6, runBrake);
		break;
	default:
		break;
	}
}

/**
  * @brief  run/brake vexta motor
  * @param  vextaChannel: which vexta motor to run/brake
  * @retval void
  */
void VextaDirection(uint8_t vextaChannel, uint8_t direction)
{
	switch (vextaChannel){
	case VEXTA_CH1:
		VextaGPIOWrite(DIRECTION1, direction);
		break;
	case VEXTA_CH2:
		VextaGPIOWrite(DIRECTION2, direction);
		break;
	case VEXTA_CH3:
		VextaGPIOWrite(DIRECTION3, direction);
		break;
	case VEXTA_CH4:
		VextaGPIOWrite(DIRECTION4, direction);
		break;
	case VEXTA_CH5:
		VextaGPIOWrite(DIRECTION5, direction);
		break;
	case VEXTA_CH6:
		VextaGPIOWrite(DIRECTION6, direction);
		break;
	default:
		break;
	}
}

/**
  * @brief  read alarm from vexta motor (if any)
  * @param  vextaChannel: which vexta motor to read from
  * @retval void
  */
uint8_t VextaGetAlarm(uint8_t vextaChannel)
{
	uint8_t alarm = 0;
	switch (vextaChannel){
	case VEXTA_CH1:
		alarm = VextaGPIORead(ALARM_OUT1);
		break;
	case VEXTA_CH2:
		alarm = VextaGPIORead(ALARM_OUT2);
		break;
	case VEXTA_CH3:
		alarm = VextaGPIORead(ALARM_OUT3);
		break;
	case VEXTA_CH4:
		alarm = VextaGPIORead(ALARM_OUT4);
		break;
	case VEXTA_CH5:
		alarm = VextaGPIORead(ALARM_OUT5);
		break;
	case VEXTA_CH6:
		alarm = VextaGPIORead(ALARM_OUT6);
		break;
	default:
		break;
	}
	return alarm;
}

/**
  * @brief  reset vexta alarm warning
  * @param  vextaChannel: which vexta motor to reset alarm
  * @retval void
  */
void VextaResetAlarm(uint8_t vextaChannel)
{

}

/**
  * @brief  get vexta feedback from vexta motor
  * @param  vextaChannel: which vexta motor to read from
  * @retval uint32_t. 32bit feedback from vexta
  */
uint32_t VextaGetFeedback(uint8_t vextaChannel, uint8_t feedbackChannel)
{
	// user cannot request feedback channel > 4.
	if (feedbackChannel > 4)
		feedbackChannel = 4;
	return vextaFeedback[vextaChannel][feedbackChannel];
}

/**
  * @brief  Reset Vexta Feedback to 0
  * @param  vextaChannel
  * @retval void
  */
void VextaResetFeedback(uint8_t vextaChannel, uint8_t feedbackChannel)
{
	// user cannot request feedback channel > 4.
		if (feedbackChannel > 4)
			feedbackChannel = 4;
	vextaFeedback[vextaChannel][feedbackChannel] = 0;
}

/**
  * @brief  This function handles External line 1 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET){
		vextaFeedback[1][0]++;
		vextaFeedback[1][1]++;
		vextaFeedback[1][2]++;
		vextaFeedback[1][3]++;
		vextaFeedback[1][4]++;
		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

/**
  * @brief  This function handles External line 7 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line7) != RESET){
		vextaFeedback[2][0]++;
		vextaFeedback[2][1]++;
		vextaFeedback[2][2]++;
		vextaFeedback[2][3]++;
		vextaFeedback[2][4]++;
		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line7);
	}
}

/**
  * @brief  This function handles External Line10, Line11, Line12, Line13 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10) != RESET){
		vextaFeedback[4][0]++;
		vextaFeedback[4][1]++;
		vextaFeedback[4][2]++;
		vextaFeedback[4][3]++;
		vextaFeedback[4][4]++;
		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	if(EXTI_GetITStatus(EXTI_Line11) != RESET){
		vextaFeedback[5][0]++;
		vextaFeedback[5][1]++;
		vextaFeedback[5][2]++;
		vextaFeedback[5][3]++;
		vextaFeedback[5][4]++;
		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	if(EXTI_GetITStatus(EXTI_Line13) != RESET){
		vextaFeedback[3][0]++;
		vextaFeedback[3][1]++;
		vextaFeedback[3][2]++;
		vextaFeedback[3][3]++;
		vextaFeedback[3][4]++;
		/* Clear the EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}
