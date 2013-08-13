/**
*****************************************************************************
**
**	Author: Robocon 2013. Latest Update 18 March 2013, 10:29p.m.
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed 揳s is,�without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
**
*****************************************************************************
*/

/* Includes */

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

/*Devices header file inclusion */
#include "dac_124s085.h"
#include "delay.h"
#include "ee_24lc256.h"
#include "adc_mcp3208.h"
#include "io_exp_mcp23s18.h"
#include "io_exp_vexta.h"
#include "miwi_ir.h"

/*Applications header file inclusion */
#include "lis302dl.h"
#include "lcd.h"
#include "ps2.h"
#include "pwm.h"
#include "usart.h"
#include "qei.h"
#include "vexta.h"

/*Tasks header file inclusion */
#include "maintask.h"
#include "sensortask.h"
#include "dcmotortask.h"
#include "positiontask.h"
#include "debugtask.h"
#include "slowiotask.h"
#include "watchdogtask.h"

/* Private macro */
/* Private variables */

// Task priorities: Higher numbers are higher priority.
#define main_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define sensor_TASK_PRIORITY     ( tskIDLE_PRIORITY + 3 )
#define dcMotor_TASK_PRIORITY    ( tskIDLE_PRIORITY + 3 )
#define position_TASK_PRIORITY   ( tskIDLE_PRIORITY + 4 )
#define debug_TASK_PRIORITY      ( tskIDLE_PRIORITY + 2 )
#define slowIo_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2 )
#define watchdog_TASK_PRIORITY   ( tskIDLE_PRIORITY + 4 )

xSemaphoreHandle xSemaphore = NULL;

/* Private function prototypes */
/* Private functions */
void dummyInit (void);

/*
 * ===========================================================================
 *
 *  Abstract: main program
 *
 * ===========================================================================
 */

int main(void)
{
	// wait for 1 second, make sure all devices are ready before configuration
	delay_ms (100);
	Usart1_init(115200);
	printf ("usart init done\r\n");
	ADC_init ();
	printf ("adc init done\r\n");
	QEI1_init ();
	QEI2_init ();
	QEI3_init ();
	printf ("qei init done\r\n");
	IoExp_init ();
	printf ("io exp init done\r\n");
	PWM_init();
	printf ("pwm init done\r\n");
	psx_init();
	printf ("psx init done\r\n");
	Vexta_init();
	printf ("vexta init done\r\n");
	//MiwiIR_init();

	//eepromInit ();
	//eepromWriteByte (0x0001, 0xA5);
	//eepromReadback = eepromReadByte (0x0001);
	printf ("STM32F Discovery is ready!\r\n");

	// Tasks get started here...
	xTaskCreate( vmainTask, (signed char *) "main", 1024,
				NULL, main_TASK_PRIORITY, &hmainTask );
	xTaskCreate( vsensorTask, (signed char *) "sensor", 1024,
				NULL, sensor_TASK_PRIORITY, &hsensorTask );
	xTaskCreate( vdcMotorTask, (signed char *) "dcmotor", 1024,
				NULL, dcMotor_TASK_PRIORITY, &hdcMotorTask );
	xTaskCreate( vpositionTask, (signed char *) "position", 1024,
				NULL, position_TASK_PRIORITY, &hpositionTask );
	xTaskCreate( vdebugTask, (signed char *) "debug", 1024,
				NULL, debug_TASK_PRIORITY, &hdebugTask );
	xTaskCreate( vslowioTask, (signed char *) "slowio", 1024,
				NULL, slowIo_TASK_PRIORITY, &hslowioTask );
	xTaskCreate( vwatchdogTask, (signed char *) "wdog", 1024,
				NULL, watchdog_TASK_PRIORITY, &hwatchdogTask );

	vTaskStartScheduler(); // This should never return.

    /* Infinite loop */
    while (1);
    return 0;
}

uint64_t u64Ticks=0;        // Counts OS ticks (default = 1000Hz).
uint64_t u64IdleTicksCnt=0; // Counts when the OS has no task to execute.
// This FreeRTOS callback function gets called once per tick (default = 1000Hz).
// ----------------------------------------------------------------------------
void vApplicationTickHook( void ) {
    ++u64Ticks;
}

// This FreeRTOS call-back function gets when no other task is ready to execute.
// On a completely unloaded system this is getting called at over 2.5MHz!
// ----------------------------------------------------------------------------
void vApplicationIdleHook( void ) {
    ++u64IdleTicksCnt;
}

// A required FreeRTOS function.
// ----------------------------------------------------------------------------
void vApplicationMallocFailedHook( void ) {
    configASSERT( 0 );  // Latch on any failure / error.
}

void dummyInit (void)
{
	uint8_t ctrl = 0;

	LIS302DL_InitTypeDef  LIS302DL_InitStruct;

	/* Set configuration of LIS302DL*/
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	LIS302DL_Init(&LIS302DL_InitStruct);

	/* Configure Interrupt control register: enable Click interrupt1 */
	ctrl = 0x47;
	LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);

	/* Enable Interrupt generation on click/double click on Z axis */
	ctrl = 0x00;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);
}
