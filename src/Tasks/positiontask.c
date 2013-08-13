/*
 * positiontask.c
 *
 *  Created on: Mar 16, 2013
 *      Author: Chee Cheong Lee
 */

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

// gyro connected to ADC Channel 1
#define GYRO_ADC	ADC_CH1

// QEI for positioning connect to QEI 1
#define POSITION_ENCODER	QEI_CH1

/*Applications header file inclusion */
#include "adc_mcp3208.h"
#include "qei.h"

/*Tasks header file inclusion */
#include "watchdogtask.h"

/*
 * This task will run every 10ms, to calculate robot position. integration will be needed
 * DO NOT place any slow response code inside this task. For example, LCD display,
 * USART communication and so on. concentrate on calculation. any debugging message needed,
 * just print through debug task.
 *
 * DO NOT PLACE ANY BLOCKING, SLOW RESPONSE CODE INSIDE THIS TASK. THIS IS TIME CRITICAL TASK
 */
portTASK_FUNCTION( vpositionTask, pvParameters )
{
	portTickType xLastWakeTime;
	uint8_t watchdogCnt = 0;

	// variables for gyroscope
	uint32_t zeroRef = 0, i, gyroData = 0;
	float curAngle = 0, curAngVel = 0, preAngVel = 0;

	// get last wake time. this is initialization before use vTaskDelayUntil
	xLastWakeTime = xTaskGetTickCount();

	// gyro initialization, wait for 1s, get zero reference.
	// counter clockwise < 2.5V, clockwise > 2.5V
	for (i=0; i<100; i++)												//1s startup time,, wait for 1s, wait for gyro stable
		vTaskDelayUntil( &xLastWakeTime, (10/portTICK_RATE_MS));

	// get 100 samples to calculate zero angular speed reference
	for (i=0; i<100; i++){
		vTaskDelayUntil( &xLastWakeTime, (10/portTICK_RATE_MS));
		zeroRef += ADC_read(GYRO_ADC);
	}
	zeroRef = zeroRef/100;

	while(1){
		vTaskDelayUntil( &xLastWakeTime, (10/portTICK_RATE_MS));

		// get angle data here.
		gyroData = ADC_read(GYRO_ADC);									//get current gyro reading
		curAngVel = ((float)gyroData - (float)zeroRef)*0.054961832;		//calculate angular velocity
		if (((gyroData > zeroRef) && ((gyroData - zeroRef)>3))
			|| ((gyroData < zeroRef) && ((zeroRef - gyroData)>3))){
			curAngle -= (curAngVel + preAngVel) * 0.005;				//get angle. 0.005 is timer interval/2
			if (curAngle > 360)											//convert to 0-360 degree only
				curAngle -= 360;
			else if (curAngle < 0)
				curAngle += 360;
			preAngVel = curAngVel;

			// this is for debug purpose only. disable when it is not needed.
			if (++i >= 100){											//display at hyperterminal every second
				//printf ("%.2f\r\n", curAngle);
				i = 0;
			}
		}


		// calculate position here. from angle data, and also encoder input, calculate the position.

		// send message to watchdog task, make sure the task not hang. send every 100ms
		// since this task is executing every 10ms, thus count 10 times = 100ms
		if (watchdogCnt++ < 10)
			continue;
		watchdogCnt = 0;
		if (watchdogQueue != 0){
			if (xQueueSend(watchdogQueue, (void *)(pcTaskGetTaskName(NULL)), (portTickType) 0) != pdPASS)
				printf("Failed Ack WD, task %s\r\n", pcTaskGetTaskName(NULL));
		}
	}
}
