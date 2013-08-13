/*
 * watcgdogTask.c
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
#include <string.h>

/*Tasks header file inclusion */
#include "maintask.h"
#include "sensortask.h"
#include "dcmotortask.h"
#include "positiontask.h"
#include "debugtask.h"
#include "slowiotask.h"

// this queue is to be used for receiving messages from other tasks.
// all the tasks will send a message to this task, to make sure they are running smoothly,
// A message from each task will be their TASK NAME correspondingly.
xQueueHandle watchdogQueue;

// you need to monitor all the tasks. whenever you create a new task, make sure you add the handle.
// Maximum task is 19, the last is to be used as null terminator, to indicate end of list.
static xTaskHandle handleMonitor[20] = {0};
static uint8_t taskFlag[20] = {0};

/* this task will run every 100ms tick. check if there is any.
 * PSX at this moment.
 */
portTASK_FUNCTION( vwatchdogTask, pvParameters )
{
	portTickType xLastWakeTime;
	char message[configMAX_TASK_NAME_LEN];
	char *taskName = NULL;
	uint8_t counter;

	// add the handle of tasks which are to be monitored. watchdog should not monitor itself,
	// make sure watchdog task does not hang.
	handleMonitor[0] = hpositionTask;
	handleMonitor[1] = hdcMotorTask;
	handleMonitor[2] = hdebugTask;
	handleMonitor[3] = hsensorTask;
	handleMonitor[4] = hslowioTask;
	handleMonitor[5] = 0;

	// start watchdog after 2 seconds, this is for all the task to finish initialization
	vTaskDelay((2000/ portTICK_RATE_MS));

	// create queue to hold 10 messages
	watchdogQueue = xQueueCreate(50, configMAX_TASK_NAME_LEN);

	// get last wake time. to ensure constant execution rate
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		// wait for next cycle
		vTaskDelayUntil( &xLastWakeTime, (150 / portTICK_RATE_MS ));

		// check if the task send me an acknowledgement/message.
		while (xQueueReceive(watchdogQueue, message, 0) == pdTRUE)
		{
			// compare the message with all taskName, check which task has acknowledged
			// watchdog within a period of time
			for(counter = 0; handleMonitor[counter]!=0; counter++){
				taskName = pcTaskGetTaskName(handleMonitor[counter]);
				if (strcmp(message, taskName) == 0){
					taskFlag[counter] = 1;
					break;
				}
			}
		}

		// once it loops through all tasks and messages, check which task is not acknowledge
		// watchdog
		for(counter = 0; handleMonitor[counter]!=0; counter++){
			// if a task is founded not to acknowledge, it might be hang, suspend the task.
			// and print error message, this is to save mcu power not to keep serving a hang task.
			// YOU NEED TO FIGURE OUT WHY THE TASK IS HANG, AND SOLVE THE PROBLEM, THIS TASK IS
			// TO GIVE AN IDEA ON TASK RUNNING SMOOTHLY OR NOT.
			if (taskFlag[counter] == 0){
				vTaskSuspend(handleMonitor[counter]);
				printf ("Task %s is suspended, please check is the program hang.!\r\n", pcTaskGetTaskName(handleMonitor[counter]));
			}
		}
	}
}
