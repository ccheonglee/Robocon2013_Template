/*
 * debugtask.c
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
#include <stdlib.h>
#include <stdarg.h>

/*Tasks header file inclusion */
#include "watchdogtask.h"

typedef struct DATA_BUFF{
	char buffer[200];
	struct DATA_BUFF *next;
} BuffNode_t;

BuffNode_t *rootNode = NULL;

/*
 * This task will run every 100ms, to output debug message. via usart/lcd/miwi module
 * THIS PIECE OF CODE IS NOT YET READY.
 *
 */
portTASK_FUNCTION( vdebugTask, pvParameters )
{
	BuffNode_t *currentNode = NULL;

	portTickType xLastWakeTime;
	uint8_t watchdogCnt = 0;

	// get last wake time. this is initialization before use vTaskDelayUntil
	xLastWakeTime = xTaskGetTickCount();

	while(1)
	{
		vTaskDelayUntil( &xLastWakeTime, (100/ portTICK_RATE_MS));

		// print if current node not equal to null. mean something inside the buffer.
		if (rootNode != NULL){

			printf (rootNode->buffer);
			// delete the node once it is printed. and move to next node.
			// be careful, this is single direction linked list, once move to next, you cannot come
			// back to previous node, and thus, you are not able to free previous node.
			// to handle this situation, use a pointer to hold current pointer before moving to
			// next node. and free current pointer.
			currentNode = rootNode;
			rootNode = rootNode->next;
			free(currentNode);
		}


		// send message to watchdog task, make sure the task not hang. send every 100ms
		// since this task is executing every 100ms, thus count 1 times = 100ms
		if (watchdogCnt++ < 1)
			continue;
		watchdogCnt = 0;
		if (watchdogQueue != 0){
			if (xQueueSend(watchdogQueue, (void *)(pcTaskGetTaskName(NULL)), (portTickType) 0) != pdPASS)
				printf("Failed Ack WD, task %s\r\n", pcTaskGetTaskName(NULL));
		}
	}
}

/**
  * @brief  alternative way to print to debug output. use as printf, need to specified interface
  * @param1	which interface you wish to print. USART, MIWI, LCD
  * @param2 string format (just like printf)
  * @param3 args (just like printf)
  * @retval int
  *
  * CAUTION: BEWARE OF RE-ENTRANCY ISSUE. MAKE SURE THIS FUNCTION WILL NOT BE INTERRUPTED BY
  * vdebugTask. This problem to be fixed.
  */
int printDebug (uint8_t debugInterface, const char * format, ...)
{
	va_list args;
	BuffNode_t *newBuffer, *currentPtr;

	// get user arguments
	va_start(args, format);

	// create a new buffer.
	newBuffer = malloc(sizeof(BuffNode_t));
	newBuffer->next = NULL;

	// check if rootNode is empty, if it is empty, rootNode = newBuffer.
	// else append it on the linked list.
	if (rootNode == NULL)
		rootNode = newBuffer;
	else{
		// looking for last element for the root node
		currentPtr = rootNode;
		while (currentPtr->next != NULL)
			currentPtr = currentPtr->next;
		currentPtr->next = newBuffer;
	}

	// print the string into the buffer and return from function.
	return printf (newBuffer->buffer, format, args);
}
