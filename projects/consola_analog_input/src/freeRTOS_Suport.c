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

xQueueHandle 	 MGR_INPUT_QUEUE;
xQueueHandle 	 MGR_OUTPUT_QUEUE;
xSemaphoreHandle MGR_TERMINAL_MUTEX;

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
						 NULL,
						 TASK_N1_PRIORITY,
						 &TASK_N1_HANDLER );

	if( pdPASS != retaux )
	{
		retval= 1;
	}

	retaux= xTaskCreate( TASK_N2, (const char *)
						 TASK_N2_DECRIPT,
						 TASK_N2_STACK,
						 NULL,
						 TASK_N2_PRIORITY,
						 &TASK_N2_HANDLER );

	if( pdPASS != retaux )
	{
		retval= 2;
	}

	retaux= xTaskCreate( TASK_N3, (const char *)
						 TASK_N3_DECRIPT,
						 TASK_N3_STACK,
						 NULL,
						 TASK_N3_PRIORITY,
						 &TASK_N3_HANDLER );

	if( pdPASS != retaux )
	{
		retval= 3;
	}

	MGR_INPUT_QUEUE  = xQueueCreate( MGR_INPUT_QUEUE_LENGT, MGR_INPUT_QUEUE_SIZE );
	MGR_OUTPUT_QUEUE = xQueueCreate( MGR_OUTPUT_QUEUE_LENGT, MGR_OUTPUT_QUEUE_SIZE );
	MGR_TERMINAL_MUTEX = xSemaphoreCreateMutex();

	return retval;
}
