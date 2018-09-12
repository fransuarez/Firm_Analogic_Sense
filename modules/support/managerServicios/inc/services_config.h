/*
 * services_config.h
 *
 *  Created on: 9/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_MANAGERSERVICIOS_INC_SERVICES_CONFIG_H_
#define MODULES_SUPPORT_MANAGERSERVICIOS_INC_SERVICES_CONFIG_H_

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "auxiliar_gpios_def.h"

#include "shellManager.h"
#include "inOutManager.h"
#include "shellManager.h"

typedef  UBaseType_t* 		stackTask_t;

#define  TIMEOUT_MUTEX_CONSOLA	(( portTickType ) 10	)
#define  TIMEOUT_QUEUE_INPUT  	(( TickType_t )   500	)
#define  TIMEOUT_QUEUE_OUTPUT 	(( TickType_t )   2000	) // antes tenia portMAX_DELAY

// ------------------------------------------------------------------------------------

#define  NUM_TASK				3
#define  ID_STACK_MGR_OUTPUT	0
#define  ID_STACK_MGR_INPUT		1
#define  ID_STACK_CONSOLA		2

#define  TASK_N1				taskControlOutputs
#define  TASK_N2				taskControlInputs
#define  TASK_N3				taskConsola

#define  PRIOIDAD_TASK_N1		tskIDLE_PRIORITY+2
#define  PRIOIDAD_TASK_N2		tskIDLE_PRIORITY+1
#define  PRIOIDAD_TASK_N3		tskIDLE_PRIORITY+1

// Con esta cantidad de stack le quedan libres 101 Bytes. Le sobran < 1*STACK_SIZE= 128
#define  STACK_TASK_N1			(configMINIMAL_STACK_SIZE*2)

// Con esta cantidad de stack le quedan libres 79 Bytes. Le sobran < 1*STACK_SIZE =128
#define  STACK_TASK_N2			(configMINIMAL_STACK_SIZE*1)

// Con esta cantidad de stack le quedan libres 119 Bytes x ahora. Le sobran < 1*STACK_SIZE =128
#define  STACK_TASK_N3			(configMINIMAL_STACK_SIZE*4)

#define  HANDLER_TASK_N1		pxCreatedTask1
#define  HANDLER_TASK_N2		pxCreatedTask2
#define  HANDLER_TASK_N3		pxCreatedTask3

#define  DESCR_TASK_N1			"Tarea de control de salidas por cola."
#define  DESCR_TASK_N2			"Tarea relevadora de entradas por interrupcion."
#define  DESCR_TASK_N3			"Tarea control de comunicacion con terminal."

// Defines relacionados a variables declaradas en alguna funcion main ----------------
#define  STACKS_TAREAS			ptrstack
#define  MUTEX_CONSOLA			mutexConsola

#define  HANDLER_QUEUE_INPUTS	queueKeyPad
#define  SIZE_ITEMS_INPUTS		sizeof(queue_t)
#define  SIZE_QUEUE_INPUTS		2

#define  HANDLER_QUEUE_OUTPUTS	queueSigLed
#define  SIZE_ITEMS_OUTPUTS		sizeof(signal_t)
#define  SIZE_QUEUE_OUTPUTS		2

#define  HANDLER_CONSOLA 		HANDLER_TASK_N3
#define  HANDLER_MGR_INPUTS 	HANDLER_TASK_N2
#define  HANDLER_MGR_OUTPUTS 	HANDLER_TASK_N1

// Macros : ----------------------------------------------------------------------------
#define  ACTUALIZAR_STACK_TAREA(X,Y) 	*(STACKS_TAREAS+Y)= uxTaskGetStackHighWaterMark(X)
#define  LIBERAR_TAREA			taskYIELD()

// ------------------------------------------------------------------------------------
int tasks_create (void );

#endif /* MODULES_SUPPORT_MANAGERSERVICIOS_INC_SERVICES_CONFIG_H_ */
