/*
 * watchdogTask.h
 *
 *  Created on: Mar 20, 2013
 *      Author: Chee Cheong Lee
 */

#ifndef WATCHDOGTASK_H_
#define WATCHDOGTASK_H_

xTaskHandle hwatchdogTask;
portTASK_FUNCTION_PROTO( vwatchdogTask, pvParameters );

// extern this variable since it is needed by all tasks.
extern xQueueHandle watchdogQueue;

#endif /* WATCHDOGTASK_H_ */
