/*
 * dcmotortask.c
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

/*Tasks header file inclusion */
#include "watchdogtask.h"

/*
 * This task will run every 10ms, to control all dc motor. since multiple dc motor controlling
 * are needed, however, creating a lot of rtos task is not feasible. Thus controlling all dc motor
 * within one task, with time slicing multitasking is preferable.
 *
 * every 10ms, the task will check is there any motor need to be on, or off, based on sensor
 * reading. everyone may have different number of motor to run, this function will need to be
 * modified from robot to robot
 */
portTASK_FUNCTION( vdcMotorTask, pvParameters )
{
	portTickType xLastWakeTime;
	uint8_t watchdogCnt = 0;

	// get last wake time. this is initialization before use vTaskDelayUntil
	xLastWakeTime = xTaskGetTickCount();

	while(1){
		vTaskDelayUntil(&xLastWakeTime, (10 / portTICK_RATE_MS));

		// motor to be controlled here. use time slicing method to control multiple motors.


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
