/*
 * maintask.h
 *
 *  Created on: Mar 15, 2013
 *      Author: Chee Cheong Lee
 */

#ifndef MAINTASK_H_
#define MAINTASK_H_

xTaskHandle hmainTask;
portTASK_FUNCTION_PROTO( vmainTask, pvParameters );

#endif /* MAINTASK_H_ */
