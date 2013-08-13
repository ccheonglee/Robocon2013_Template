/*
 * debugtask.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Chee Cheong Lee
 */

#ifndef DEBUGTASK_H_
#define DEBUGTASK_H_

#define DEBUG_USART	0
#define DEBUG_LCD	1
#define DEBUG_MIWI	2

#define BUFF_FREE	0
#define BUFF_FULL	1

xTaskHandle hdebugTask;
portTASK_FUNCTION_PROTO( vdebugTask, pvParameters );

int printDebug (uint8_t debugInterface, const char * format, ...);

#endif /* DEBUGTASK_H_ */
