/*
 * slowiotask.h
 *
 *  Created on: Mar 20, 2013
 *      Author: Chee Cheong Lee
 */

#ifndef SLOWIOTASK_H_
#define SLOWIOTASK_H_

xTaskHandle hslowioTask;
portTASK_FUNCTION_PROTO( vslowioTask, pvParameters );

#endif /* SLOWIOTASK_H_ */
