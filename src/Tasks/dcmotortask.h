/*
 * dcmotortask.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Chee Cheong Lee
 */

#ifndef DCMOTORTASK_H_
#define DCMOTORTASK_H_

xTaskHandle hdcMotorTask;
portTASK_FUNCTION_PROTO( vdcMotorTask, pvParameters );

#endif /* DCMOTORTASK_H_ */
