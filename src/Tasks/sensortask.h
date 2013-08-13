/*
 * sensortask.h
 *
 *  Created on: Mar 15, 2013
 *      Author: Chee Cheong Lee
 */

#ifndef SENSORTASK_H_
#define SENSORTASK_H_

xTaskHandle hsensorTask;
portTASK_FUNCTION_PROTO( vsensorTask, pvParameters );

#endif /* SENSORTASK_H_ */
