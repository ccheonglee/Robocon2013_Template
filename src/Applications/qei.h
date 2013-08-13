/*
  ******************************************************************************
  * @file    qei.h
  * @author  Boon
  * @date    Dec 4, 2012
  * @brief   This file provides quadrature encoder header file
  ******************************************************************************
 */

#ifndef QEI_H_
#define QEI_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#define QEI_CH1 0
#define QEI_CH2 1
#define QEI_CH3 2

/** Functions prototype
  * @{
  */
void QEI1_init (void);
void QEI2_init (void);
void QEI3_init (void);

int32_t QEIGetFeedback (uint8_t channel);

#endif /* QEI_H_ */
