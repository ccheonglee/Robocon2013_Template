/* -----------------------------------------------------------------------
	TIMx Configuration: generate 4 PWM signals with 4 different duty cycles.
	SystemCoreClock for APB2 is 168MHz, APB1 is 84MHz
*/

/*
 * CHANGE LOG:
 * 24 March 2013	:All pwm will be updated only if there is a changed. If there is no change, no update
 * 					will be performed.
 */

/*Includes ------------------------------------------------------------------*/
#include "pwm.h"

/**
  * @brief  setup pwm interface
  * @param  none
  * @retval void
  */

// frequency must more than 3KHz
#define FREQUENCY 5000

static uint16_t TIM10_11_PERIOD	 = ((uint32_t)(168000000)/FREQUENCY);
static uint16_t TIM4_5_12_PERIOD = ((uint32_t)(84000000)/FREQUENCY);

static uint16_t pwmDutyCycleCounter[12] = {0};
static uint8_t pwmChanged[12] = {0};

static void updatePWM1(uint16_t dutyCycleCounter);
static void updatePWM2(uint16_t dutyCycleCounter);
static void updatePWM3(uint16_t dutyCycleCounter);
static void updatePWM4(uint16_t dutyCycleCounter);
static void updatePWM5(uint16_t dutyCycleCounter);
static void updatePWM6(uint16_t dutyCycleCounter);
static void updatePWM7(uint16_t dutyCycleCounter);
static void updatePWM8(uint16_t dutyCycleCounter);
static void updatePWM9(uint16_t dutyCycleCounter);
static void updatePWM10(uint16_t dutyCycleCounter);
static void updatePWM11(uint16_t dutyCycleCounter);
static void updatePWM12(uint16_t dutyCycleCounter);

void PWM_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* TIMx clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 | RCC_APB1Periph_TIM5 | RCC_APB1Periph_TIM12, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM10 |  RCC_APB2Periph_TIM11, ENABLE);

	/* GPIOA, GPIOB and GPIOD clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD, ENABLE);

	/* GPIOA Configuration: TIM5 CH1 (PA0), TIM5 CH2 (PA1), TIM5 CH3 (PA2) and TIM5 CH4 (PA3) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB Configuration:  TIM10 CH1 (PB8), TIM11 CH1 (PB9), TIM12 CH1 (PB14) and TIM12 CH2 (PB15) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_14 | GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOD Configuration:  TIM4 CH1 (PD12), TIM4 CH2 (PD13), TIM4 CH3 (PD14) and TIM4 CH4 (PD15) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* Connect TIMx pins to AF2 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_TIM5);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM11);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_TIM12);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_TIM12);

	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_TIM4);

	//PWM
	//84M/1000=84000  (timer f/f needed)
	//84000=16800*5 (period*prescaler)
	//16800*0.5=8400 (if for duty cycle 50% and 1000Hz)(4200 for CCRx_Val)(8400 for Period_Val)

	//84M/100=840000  (timer f/f needed)
	//840000=16800*50 (period*prescaler)
	//16800*0.5=8400 (if for duty cycle 50% and 100Hz)(8400 for CCRx_Val)(16800 for Period_Val)

	//(Prescaler - 1)
	//(Period - 1)

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = (TIM4_5_12_PERIOD-1);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM12, &TIM_TimeBaseStructure);

	//PWM
	//168M/1000=168000  (timer f/f needed)
	//168000=8400*20 (period*prescaler)
	//8400*0.5=4200 (if for duty cycle 50% and 1000Hz)(4200 for CCRx_Val)(8400 for Period_Val)

	//168M/100=1680000  (timer f/f needed)
	//1680000 =16800*100 (period*prescaler)
	//16800*0.5=8400 (if for duty cycle 50% and 100Hz)(8400 for CCRx_Val)(16800 for Period_Val)

	//(Prescaler - 1)
	//(Period - 1)

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = (TIM10_11_PERIOD-1);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_ARRPreloadConfig(TIM5, ENABLE);
	TIM_ARRPreloadConfig(TIM10, ENABLE);
	TIM_ARRPreloadConfig(TIM11, ENABLE);
	TIM_ARRPreloadConfig(TIM12, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM4, ENABLE);
	TIM_Cmd(TIM5, ENABLE);
	TIM_Cmd(TIM10, ENABLE);
	TIM_Cmd(TIM11, ENABLE);
	TIM_Cmd(TIM12, ENABLE);

	PWM1(0);
	PWM2(0);
	PWM3(0);
	PWM4(0);
	PWM5(0);
	PWM6(0);
	PWM7(0);
	PWM8(0);
	PWM9(0);
	PWM10(0);
	PWM11(0);
	PWM12(0);

	updateAllPwm();
}

void PWM1(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[0] = (dutyCycle*TIM10_11_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[0] = TIM10_11_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[0] = 1;
}

void PWM2(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[1] = (dutyCycle*TIM10_11_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[1] = TIM10_11_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[1] = 1;
}

void PWM3(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[2] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[2] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[2] = 1;
}

void PWM4(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[3] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[3] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[3] = 1;
}

void PWM5(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[4] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[4] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[4] = 1;
}

void PWM6(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[5] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[5] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[5] = 1;
}

void PWM7(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[6] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[6] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[6] = 1;
}

void PWM8(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[7] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[7] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[7] = 1;
}

void PWM9(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[8] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[8] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[8] = 1;
}

void PWM10(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[9] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[9] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[9] = 1;
}

void PWM11(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[10] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[10] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[10] = 1;
}

void PWM12(uint16_t dutyCycle)
{
	// calculate the pwm percentage to exact cycle needed.
	pwmDutyCycleCounter[11] = (dutyCycle*TIM4_5_12_PERIOD)/100;

	// if duty cycle more than 100, set ON cycle more than PERIOD cycle. This will cause microcontroller
	// generate 100% duty cycle.
	if (dutyCycle >= 100)
		pwmDutyCycleCounter[11] = TIM4_5_12_PERIOD + 1;

	// set the flag indicate there is change on pwm.
	pwmChanged[11] = 1;
}

/**
  * @brief  set pwm_pinB8 value for TIM10_CH1
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM1(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM10, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinB9 value for TIM11_CH1
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM2(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM11, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinB14 value for TIM12_CH1
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM3(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM12, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinB15 value for TIM12_CH2
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM4(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM12, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM12, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinA2 value for TIM5_CH3
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM5(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM5, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM5, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinA0 value for TIM5_CH1
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM6(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM5, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinD12 value for TIM4_CH1
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM7(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinD14 value for TIM4_CH3
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM8(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC3Init(TIM4, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinA3 value for TIM5_CH4
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM9(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC4Init(TIM5, &TIM_OCInitStructure);

	TIM_OC4PreloadConfig(TIM5, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinA1 value for TIM5_CH2
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM10(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM5, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinD13 value for TIM4_CH2
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM11(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC2Init(TIM4, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

/**
  * @brief  set pwm_pinD15 value for TIM4_CH4
  * @param  speed as the pwm pulse
  * @retval void
  */
static void updatePWM12(uint16_t dutyCycleCounter)
{
	/* PWM1 Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = dutyCycleCounter;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC4Init(TIM4, &TIM_OCInitStructure);

	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
}

void updateAllPwm (void)
{
	// if there is changes in pwm 0, update the pwm and clear the flag. same to all channel.
	if (pwmChanged[0]){
		updatePWM1(pwmDutyCycleCounter[0]);
		pwmChanged[0] = 0;
	}
	if (pwmChanged[1]){
		updatePWM2(pwmDutyCycleCounter[1]);
		pwmChanged[1] = 0;
	}
	if (pwmChanged[2]){
		updatePWM3(pwmDutyCycleCounter[2]);
		pwmChanged[2] = 0;
	}
	if (pwmChanged[3]){
		updatePWM4(pwmDutyCycleCounter[3]);
		pwmChanged[3] = 0;
	}
	if (pwmChanged[4]){
		updatePWM5(pwmDutyCycleCounter[4]);
		pwmChanged[4] = 0;
	}
	if (pwmChanged[5]){
		updatePWM6(pwmDutyCycleCounter[5]);
		pwmChanged[5] = 0;
	}
	if (pwmChanged[6]){
		updatePWM7(pwmDutyCycleCounter[6]);
		pwmChanged[6] = 0;
	}
	if (pwmChanged[7]){
		updatePWM8(pwmDutyCycleCounter[7]);
		pwmChanged[7] = 0;
	}
	if (pwmChanged[8]){
		updatePWM9(pwmDutyCycleCounter[8]);
		pwmChanged[8] = 0;
	}
	if (pwmChanged[9]){
		updatePWM10(pwmDutyCycleCounter[9]);
		pwmChanged[9] = 0;
	}
	if (pwmChanged[10]){
		updatePWM11(pwmDutyCycleCounter[10]);
		pwmChanged[10] = 0;
	}
	if (pwmChanged[11]){
		updatePWM12(pwmDutyCycleCounter[11]);
		pwmChanged[11] = 0;
	}
}
