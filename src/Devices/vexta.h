/*
  ******************************************************************************
  * @file    vexta.H
  * @author  Boon
  * @date    Dec 4, 2012
  * @brief   This file provides vexta motor header file
  ******************************************************************************
 */

#ifndef VEXTA_H_
#define VEXTA_H_

#include "stm32f4xx.h"

#define MOTOR_CW	0
#define MOTOR_CCW	1

#define MOTOR_RUN	0
#define MOTOR_BRAKE 1
#define MOTOR_FREE	2

#define VEXTA_CH1	0
#define VEXTA_CH2	1
#define VEXTA_CH3	2
#define VEXTA_CH4	3
#define VEXTA_CH5	4
#define VEXTA_CH6	5

/*  function prototypes */
void Vexta_init (void);
void VextaSetSpeed(uint8_t vextaChannel, uint16_t speed);

void VextaSetStatus(uint8_t vextaChannel, uint8_t status);
void VextaDirection(uint8_t vextaChannel, uint8_t direction);

uint8_t VextaGetAlarm(uint8_t vextaChannel);
void VextaResetAlarm(uint8_t vextaChannel);

uint32_t VextaGetFeedback(uint8_t vextaChannel, uint8_t feedbackChannel);
void VextaResetFeedback(uint8_t vextaChannel, uint8_t feedbackChannel);

#endif /* VEXTA_H_ */
