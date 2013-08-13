/*
 * maintask.c
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


/*Applications header file inclusion */
#include "lcd.h"
#include "ps2.h"
#include "pwm.h"
#include "usart.h"
#include "qei.h"
#include "adc_mcp3208.h"
#include "io_exp_mcp23s18.h"
#include "vexta.h"
#include "delay.h"

int32_t qeiFeedback;
uint16_t i, adcReadback;

portTASK_FUNCTION( vmainTask, pvParameters )
{
	taskENTER_CRITICAL();
	setGPIOIODir(GPIO_LED5, IO_OUTPUT);
	setGPIOIODir(GPIO_LED6, IO_OUTPUT);
	setGPIOIODir(GPIO_LED7, IO_OUTPUT);
	setGPIOIODir(GPIO_LED8, IO_OUTPUT);
	setGPIOPullUp(GPIO_LED5, 1);
	setGPIOPullUp(GPIO_LED6, 1);
	setGPIOPullUp(GPIO_LED7, 1);
	setGPIOPullUp(GPIO_LED8, 1);
	setGPIOIODir(GPA0_2, IO_INPUT);
	setGPIOPullUp(GPA0_2, 1);
	taskEXIT_CRITICAL();

	// write to gpio
	GPIOWrite(GPIO_LED5, 0);
	GPIOWrite(GPIO_LED6, 1);
	GPIOWrite(GPIO_LED7, 0);
	GPIOWrite(GPIO_LED8, 1);

	VextaSetSpeed(VEXTA_CH1, 2048);
	VextaSetSpeed(VEXTA_CH2, 2048);
	VextaSetSpeed(VEXTA_CH3, 2048);
	VextaSetSpeed(VEXTA_CH4, 2048);
	VextaSetSpeed(VEXTA_CH5, 2048);
	VextaSetSpeed(VEXTA_CH6, 2048);

	while (1){
		adcReadback = GPIORead(GPA0_2);
		vTaskDelay(250);
		printf ("gpioA: %u\r\n", adcReadback);
		if (adcReadback == 0){
			printf ("got 0\r\n");
			if (GPIORead(GPIO_LED5)){
				GPIOWrite(GPIO_LED5, 0);
			}
			else{
				GPIOWrite(GPIO_LED5, 1);
			}
		}
		if (adcReadback == 4095){
			printf ("got 4095\r\n");
			if (GPIORead(GPIO_LED6)){
				GPIOWrite(GPIO_LED6, 0);
			}
			else{
				GPIOWrite(GPIO_LED6, 1);
			}
		}
	}

	while (1){
		adcReadback = ADC_read(ADC_CH1);
		if (adcReadback == 0){
			printf ("got 0\r\n");
			if (GPIORead(GPIO_LED5)){
				GPIOWrite(GPIO_LED5, 0);
			}
			else{
				GPIOWrite(GPIO_LED5, 1);
			}
		}
		if (adcReadback == 4095){
			printf ("got 4095\r\n");
			if (GPIORead(GPIO_LED6)){
				GPIOWrite(GPIO_LED6, 0);
			}
			else{
				GPIOWrite(GPIO_LED6, 1);
			}
		}
	}

	PWM1(59);

	while (1){
		qeiFeedback = QEIGetFeedback(QEI_CH3);
		//printf ("QEI : %ld\r\n", qeiFeedback);
		vTaskDelay((500/ portTICK_RATE_MS));
		if (GPIORead(GPIO_LED5)){
			GPIOWrite(GPIO_LED5, 0);
			printf ("read on switch off\r\n");
		}
		else{
			GPIOWrite(GPIO_LED5, 1);
			printf ("read off switch on\r\n");
		}
	}

	// read GPIO
	GPIORead(GPA0_2);

	// read adc
	ADC_read(ADC_CH13);
	// set direction
	VextaDirection(VEXTA_CH2, MOTOR_CW);

	// start motor
	VextaSetStatus(VEXTA_CH2, MOTOR_RUN);

	// delay 2000 ms = 2s
	vTaskDelay((2000/ portTICK_RATE_MS));
	// set direction
	VextaDirection(VEXTA_CH2, MOTOR_CCW);

	// change speed
	VextaSetSpeed(VEXTA_CH2, 2048);

	// get feedback
	//printf ("feedback = %lu\r\n", VextaGetFeedback(VEXTA_CH2));
	vTaskDelay((1000/ portTICK_RATE_MS));

	// read adc
	while (1){
		printf ("adc = %u\r\n", ADC_read(ADC_CH1));
		printf ("adc = %u\r\n", ADC_read(ADC_CH2));
		printf ("adc = %u\r\n", ADC_read(ADC_CH3));
		printf ("adc = %u\r\n", ADC_read(ADC_CH4));
		printf ("adc = %u\r\n", ADC_read(ADC_CH5));
		printf ("adc = %u\r\n", ADC_read(ADC_CH6));
		printf ("adc = %u\r\n", ADC_read(ADC_CH7));
		printf ("adc = %u\r\n", ADC_read(ADC_CH8));
		printf ("adc = %u\r\n", ADC_read(ADC_CH9));
		printf ("adc = %u\r\n", ADC_read(ADC_CH10));
		printf ("adc = %u\r\n", ADC_read(ADC_CH11));
		printf ("adc = %u\r\n", ADC_read(ADC_CH12));
		printf ("adc = %u\r\n", ADC_read(ADC_CH13));
		printf ("adc = %u\r\n", ADC_read(ADC_CH14));
		printf ("adc = %u\r\n", ADC_read(ADC_CH15));
		printf ("adc = %u\r\n", ADC_read(ADC_BATT));
		vTaskDelay((250/ portTICK_RATE_MS));
	}
	while (1);
}
