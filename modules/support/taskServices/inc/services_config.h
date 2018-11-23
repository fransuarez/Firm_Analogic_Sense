/*
 * services_config.h
 *
 *  Created on: 9/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_TASKSERVICES_INC_SERVICES_CONFIG_H_
#define MODULES_SUPPORT_TASKSERVICES_INC_SERVICES_CONFIG_H_

//#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "auxiliar_gpios_def.h"

#include "inOutManager.h"
#include "terminalManager.h"
#include "filesManager.h"

#define  TIMEOUT_MUTEX_CONSOLA	(( TickType_t ) 10		)
#define  TIMEOUT_MUTEX_INPUT	(( TickType_t ) 10		)
#define  TIMEOUT_QUEUE_INPUT  	(( TickType_t ) 2000	)
#define  TIMEOUT_QUEUE_OUTPUT 	(( TickType_t ) 2000	) // antes tenia portMAX_DELAY
#define  TIMEOUT_QUEUE_MSG_INP 	(( TickType_t ) 2		)
#define  TIMEOUT_QUEUE_MSG_OUT 	(( TickType_t ) 500		)
#define  TIMEOUT_QUEUE_LOG_INP 	(( TickType_t ) 2		)
#define  TIMEOUT_QUEUE_LOG_OUT 	(( TickType_t ) 500		)

// ------------------------------------------------------------------------------------
#define  NUM_TASK				4
#define  STACKS_TAREAS			ptrstack

// Macros : ----------------------------------------------------------------------------
#define  ACTUALIZAR_STACK(X,Y)  (*(STACKS_TAREAS+Y)= uxTaskGetStackHighWaterMark(X))
#define  LIBERAR_TAREA			(taskYIELD())
#define  TOMAR_SEMAFORO(X,Y)	( pdTRUE == xSemaphoreTake(X,Y))
#define  LIBERAR_SEMAFORO(X)	( pdTRUE == xSemaphoreGive(X) )

// -------------------- Definiciones tarea 1  ---------------------
#define  TASK_N1				taskControlOutputs
#define  TASK_N1_DECRIPT		"Tarea de control de salidas por cola."
// Con esta cantidad de stack le quedan libres 101 Bytes. Le sobran < 1*STACK_SIZE= 128
#define  TASK_N1_STACK			(configMINIMAL_STACK_SIZE*2)
#define  TASK_N1_PRIORITY		tskIDLE_PRIORITY+2
#define  TASK_N1_HANDLER		pxCreatedTask1
#define  TASK_N1_ID_STACK		0

#define  MGR_OUTPUT_ID_STACK	TASK_N1_ID_STACK
#define  MGR_OUTPUT_HANDLER 	TASK_N1_HANDLER
#define  MGR_OUTPUT_DELAY 		((TickType_t) 50)

#define  MGR_OUTPUT_QUEUE		queueSigLed
#define  MGR_OUTPUT_QUEUE_SIZE	sizeof(dOutputQueue_t)
#define  MGR_OUTPUT_QUEUE_LENGT	2

// -------------------- Definiciones tarea 2  ---------------------
#define  TASK_N2				taskControlInputs
#define  TASK_N2_DECRIPT		"Tarea relevadora de entradas por interrupcion."
// Con esta cantidad de stack le quedan libres 79 Bytes. Le sobran < 1*STACK_SIZE =128
#define  TASK_N2_STACK			(configMINIMAL_STACK_SIZE*2)
#define  TASK_N2_PRIORITY		tskIDLE_PRIORITY+1
#define  TASK_N2_HANDLER		pxCreatedTask2
#define  TASK_N2_ID_STACK		1

#define  MGR_INPUT_ID_STACK		TASK_N2_ID_STACK
#define  MGR_INPUT_HANDLER 		TASK_N2_HANDLER
#define  MGR_INPUT_DELAY 		((TickType_t) 50)
#define  MGR_INPUT_MUTEX		mutexGpioReg

#define  MGR_INPUT_QUEUE		queueKeyPad
#define  MGR_INPUT_QUEUE_SIZE	sizeof(dInputQueue_t)
#define  MGR_INPUT_QUEUE_LENGT	2

// -------------------- Definiciones tarea 3  ---------------------
#define  TASK_N3				taskTerminal
#define  TASK_N3_DECRIPT		"Tarea control de comunicacion con terminal."
// Con esta cantidad de stack le quedan libres 119 Bytes x ahora. Le sobran < 1*STACK_SIZE =128
#define  TASK_N3_STACK			(configMINIMAL_STACK_SIZE*4)
#define  TASK_N3_PRIORITY		tskIDLE_PRIORITY+1
#define  TASK_N3_HANDLER		pxCreatedTask3
#define  TASK_N3_ID_STACK		2

#define  MGR_TERMINAL_ID_STACK	TASK_N3_ID_STACK
#define  MGR_TERMINAL_HANDLER 	TASK_N3_HANDLER
#define  MGR_TERMINAL_DELAY 	((TickType_t) 10)
#define  MGR_TERMINAL_MUTEX		mutexConsola

#define  MGR_TERMINAL_QUEUE		queueTermMsg
#define  MGR_TERMINAL_QUEUE_SIZ	sizeof(terMsg_t)
#define  MGR_TERMINAL_QUEUE_LEN	4

// -------------------- Definiciones tarea 4  ---------------------
#define  TASK_N4				dataLog_Service
#define  TASK_N4_DECRIPT		"Tarea control de guardado de registros en eeprom."
// Con esta cantidad de stack le quedan libres 119 Bytes x ahora. Le sobran < 1*STACK_SIZE =128
#define  TASK_N4_STACK			(configMINIMAL_STACK_SIZE*2)
#define  TASK_N4_PRIORITY		tskIDLE_PRIORITY+1
#define  TASK_N4_HANDLER		pxCreatedTask4
#define  TASK_N4_ID_STACK		3

#define  MGR_DATALOG_ID_STACK	TASK_N4_ID_STACK
#define  MGR_DATALOG_HANDLER 	TASK_N4_HANDLER
#define  MGR_DATALOG_DELAY 		((TickType_t) 10)

#define  MGR_DATALOG_QUEUE		queueDataLog
#define  MGR_DATALOG_QUEUE_SIZE	sizeof(dlogPack_t )
#define  MGR_DATALOG_QUEUE_LENGT 2
// ------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------
int tasks_create (void );

#endif /* MODULES_SUPPORT_TASKSERVICES_INC_SERVICES_CONFIG_H_ */
