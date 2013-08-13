/*
 * positiontask.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Chee Cheong Lee
 */

#ifndef POSITIONTASK_H_
#define POSITIONTASK_H_

xTaskHandle hpositionTask;
portTASK_FUNCTION_PROTO( vpositionTask, pvParameters );

#endif /* POSITIONTASK_H_ */
