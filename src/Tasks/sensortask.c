/*
 * sensortask.c
 *
 *  Created on: Mar 15, 2013
 *      Author: Chee Cheong Lee
 */

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

#include "io_exp_mcp23s18.h"
#include "adc_mcp3208.h"
#include "pwm.h"

/*Tasks header file inclusion */
#include "watchdogtask.h"

/* this task will run every 1ms tick. update all the io, and also adc value
 * user does not read from io or adc directly, but read from shadow register which will be
 * updated from here periodically.
 *
 * since everyone will be having the same adc reading and io reading method, this piece of code
 * shouldn't be modified.
 */
portTASK_FUNCTION( vsensorTask, pvParameters )
{
	portTickType xLastWakeTime;
	uint8_t watchdogCnt = 0;

	// get last wake time. this is initialization before use vTaskDelayUntil
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		// wait for next cycle
		vTaskDelayUntil( &xLastWakeTime, (1 / portTICK_RATE_MS ));

		/********************************** update input ****************************/
		// start from here, read from io expander.
		IoExpInput_update();

		// start from here, read from ADC. all 16 channel adc are read.
		ADC_update();

		/********************************** update output ****************************/
		IoExpOutput_update();

		/*********************************update pwm*********************************/
		updateAllPwm();

		// send message to watchdog task, make sure the task not hang. send every 100ms
		// since this task is executing every 1ms, thus count 100 times = 100ms
		if (watchdogCnt++ < 100)
			continue;
		watchdogCnt = 0;
		if (watchdogQueue != 0){
			if (xQueueSend(watchdogQueue, (void *)(pcTaskGetTaskName(NULL)), (portTickType) 0) != pdPASS)
				printf("Failed Ack WD, task %s\r\n", pcTaskGetTaskName(NULL));
		}
	}
}
