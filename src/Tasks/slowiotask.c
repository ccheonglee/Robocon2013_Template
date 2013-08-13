/*
 * slowiotask.c
 *
 *  Created on: Mar 20, 2013
 *      Author: Chee Cheong Lee
 */

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

#include "ps2.h"

/*Tasks header file inclusion */
#include "watchdogtask.h"

/* this task will run every 25ms tick. update all the slow input/output/.
 * PSX at this moment.
 */
portTASK_FUNCTION( vslowioTask, pvParameters )
{
	portTickType xLastWakeTime;
	uint8_t watchdogCnt = 0;

	// get last wake time. this is initialization before use vTaskDelayUntil
	xLastWakeTime = xTaskGetTickCount();
	while(1)
	{
		// wait for next cycle
		vTaskDelayUntil( &xLastWakeTime, (25 / portTICK_RATE_MS ));
		//get_psx();

		// send message to watchdog task, make sure the task not hang. send every 100ms
		// since this task is executing every 25ms, thus count 4 times = 100ms
		if (watchdogCnt++ < 4)
			continue;
		watchdogCnt = 0;
		if (watchdogQueue != 0){
			if (xQueueSend(watchdogQueue, (void *)(pcTaskGetTaskName(NULL)), (portTickType) 0) != pdPASS)
				printf("Failed Ack WD, task %s\r\n", pcTaskGetTaskName(NULL));
		}
	}
}
