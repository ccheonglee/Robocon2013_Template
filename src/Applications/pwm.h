/*
  ******************************************************************************
  * @file    pwm.h
  * @author  Eugene
  * @date    Dec 4, 2012
  * @brief   This file provides pwm header file
  ******************************************************************************
  */

#ifndef PWM_H_
#define PWM_H_

#include "stm32f4xx.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;


/** Functions prototype
  * @{
  */
void PWM_init(void);

void PWM1(uint16_t dutyCycle);
void PWM2(uint16_t dutyCycle);
void PWM3(uint16_t dutyCycle);
void PWM4(uint16_t dutyCycle);
void PWM5(uint16_t dutyCycle);
void PWM6(uint16_t dutyCycle);
void PWM7(uint16_t dutyCycle);
void PWM8(uint16_t dutyCycle);
void PWM9(uint16_t dutyCycle);
void PWM10(uint16_t dutyCycle);
void PWM11(uint16_t dutyCycle);
void PWM12(uint16_t dutyCycle);

void updateAllPwm (void);

#endif /* PWM_H_ */
