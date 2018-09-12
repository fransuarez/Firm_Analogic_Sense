/*
 * freeRTOS_Suport.c
 *
 *  Created on: 9/7/2018
 *      Author: fran
 */
#include "services_config.h"

xTaskHandle 	 HANDLER_TASK_N1;
xTaskHandle 	 HANDLER_TASK_N2;
xTaskHandle 	 HANDLER_TASK_N3;

xQueueHandle 	 HANDLER_QUEUE_INPUTS;
xQueueHandle 	 HANDLER_QUEUE_OUTPUTS;
xSemaphoreHandle MUTEX_CONSOLA;

// Arreglo con el stack disponible de las tareas creadas en main
UBaseType_t 	 stacktareas[NUM_TASK];
UBaseType_t* 	 STACKS_TAREAS= stacktareas;


int tasks_create ( void )
{
	int retval= 0;
	BaseType_t retaux;

	retaux= xTaskCreate( TASK_N1, (const char *)
						 DESCR_TASK_N1,
						 STACK_TASK_N1,
						 NULL,
						 PRIOIDAD_TASK_N1,
						 &HANDLER_TASK_N1 );

	if( pdPASS != retaux )
	{
		retval= 1;
	}

	retaux= xTaskCreate( TASK_N2, (const char *)
						 DESCR_TASK_N2,
						 STACK_TASK_N2,
						 NULL,
						 PRIOIDAD_TASK_N2,
						 &HANDLER_TASK_N2 );

	if( pdPASS != retaux )
	{
		retval= 2;
	}

	retaux= xTaskCreate( TASK_N3, (const char *)
						 DESCR_TASK_N3,
						 STACK_TASK_N3,
						 NULL,
						 PRIOIDAD_TASK_N3,
						 &HANDLER_TASK_N3 );

	if( pdPASS != retaux )
	{
		retval= 3;
	}

	HANDLER_QUEUE_INPUTS  = xQueueCreate( SIZE_QUEUE_INPUTS, SIZE_ITEMS_INPUTS );
	HANDLER_QUEUE_OUTPUTS = xQueueCreate( SIZE_QUEUE_OUTPUTS, SIZE_ITEMS_OUTPUTS );
	MUTEX_CONSOLA = xSemaphoreCreateMutex();

	return retval;
}
