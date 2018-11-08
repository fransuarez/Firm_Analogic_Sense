/*
 * freeRTOS_Suport.c
 *
 *  Created on: 9/7/2018
 *      Author: fran
 */
#include "services_config.h"

xTaskHandle 	 TASK_N1_HANDLER;
xTaskHandle 	 TASK_N2_HANDLER;
xTaskHandle 	 TASK_N3_HANDLER;
xTaskHandle 	 TASK_N4_HANDLER;

xQueueHandle 	 MGR_INPUT_QUEUE;
xQueueHandle 	 MGR_OUTPUT_QUEUE;
xQueueHandle 	 MGR_TERMINAL_QUEUE;
xQueueHandle 	 MGR_DATALOG_QUEUE;

xSemaphoreHandle MGR_TERMINAL_MUTEX;
xSemaphoreHandle MGR_INPUT_MUTEX;

// Arreglo con el stack disponible de las tareas creadas en main
UBaseType_t 	 stacktareas[NUM_TASK];
UBaseType_t* 	 STACKS_TAREAS= stacktareas;


int tasks_create ( void )
{
	int retval= 0;
	BaseType_t retaux;

	retaux= xTaskCreate( TASK_N1, (const char *)
						 TASK_N1_DECRIPT,
						 TASK_N1_STACK,
						 0,
						 TASK_N1_PRIORITY,
						 &TASK_N1_HANDLER );

	if( pdPASS != retaux )
	{
		retval= 1;
	}

	retaux= xTaskCreate( TASK_N2, (const char *)
						 TASK_N2_DECRIPT,
						 TASK_N2_STACK,
						 0,
						 TASK_N2_PRIORITY,
						 &TASK_N2_HANDLER );

	if( pdPASS != retaux )
	{
		retval= 2;
	}

	retaux= xTaskCreate( TASK_N3, (const char *)
						 TASK_N3_DECRIPT,
						 TASK_N3_STACK,
						 0,
						 TASK_N3_PRIORITY,
						 &TASK_N3_HANDLER );

	if( pdPASS != retaux )
	{
		retval= 3;
	}

	retaux= xTaskCreate( TASK_N4, (const char *)
						 TASK_N4_DECRIPT,
						 TASK_N4_STACK,
						 0,
						 TASK_N4_PRIORITY,
						 &TASK_N4_HANDLER );

	if( pdPASS != retaux )
	{
		retval= 4;
	}

	MGR_INPUT_QUEUE  = xQueueCreate( MGR_INPUT_QUEUE_LENGT, MGR_INPUT_QUEUE_SIZE );
	MGR_OUTPUT_QUEUE = xQueueCreate( MGR_OUTPUT_QUEUE_LENGT, MGR_OUTPUT_QUEUE_SIZE );
	MGR_TERMINAL_QUEUE = xQueueCreate( MGR_TERMINAL_QUEUE_LEN, MGR_TERMINAL_QUEUE_SIZ );
	MGR_DATALOG_QUEUE = xQueueCreate( MGR_DATALOG_QUEUE_LENGT, MGR_DATALOG_QUEUE_SIZE );

	MGR_TERMINAL_MUTEX = xSemaphoreCreateMutex();
	MGR_INPUT_MUTEX= xSemaphoreCreateMutex();

	return retval;
}
