/*
 * freeRTOS_Suport.c
 *
 *  Created on: 9/7/2018
 *      Author: fran
 */
#include "main.h"
#include "services_config.h"


#define MSEC_REFRESH_CPU	2000
#define NUM_TASK_SUPPORT	2  // iddle y timerTask
#define NUM_TASK_TOTAL		NUM_TASK+NUM_TASK_SUPPORT // iddle y timerTask

TaskHandle_t 	 TASK_N1_HANDLER;
TaskHandle_t 	 TASK_N2_HANDLER;
TaskHandle_t 	 TASK_N3_HANDLER;
TaskHandle_t 	 TASK_N4_HANDLER;

QueueHandle_t 	 MGR_INPUT_QUEUE;
QueueHandle_t 	 MGR_OUTPUT_QUEUE;
QueueHandle_t 	 MGR_TERMINAL_QUEUE;
QueueHandle_t 	 MGR_DATALOG_QUEUE;

SemaphoreHandle_t MGR_TERMINAL_MUTEX;
SemaphoreHandle_t MGR_INPUT_MUTEX;
SemaphoreHandle_t MGR_DATALOG_MUTEX;

TimerHandle_t 	TIMER_1_OBJ;
TimerHandle_t 	TIMER_2_OBJ;
TimerHandle_t 	TIMER_3_OBJ;
TimerHandle_t 	TIMER_4_OBJ;


TaskStatus_t pxTaskStatusArray[NUM_TASK_TOTAL];

// Arreglo con el stack disponible de las tareas creadas en main
typedef struct _recurses_task
{
	const char * TASK_Name;
	UBaseType_t TASK_Id;
	uint32_t 	CPU_tickCount;
	uint16_t 	CPU_rate;
	uint16_t 	RAM_stack;
	uint16_t 	RAM_stackUsed;

} recTask_t;

UBaseType_t  	stacktareas[NUM_TASK];
UBaseType_t* 	ptrstack= stacktareas;
//UBaseType_t		cpuTasks[NUM_TASK+1]={0};
//UBaseType_t* 	ptrCpu= cpuTasks;

// Por alguna razon las lee en este orden al estado de las tareas:
recTask_t 		taskInfoProc[NUM_TASK_TOTAL]=
{
	{.RAM_stack= configMINIMAL_STACK_SIZE 	}, //5
	{.RAM_stack= TASK_N4_STACK				}, //4
	{.RAM_stack= TASK_N1_STACK				}, //1
	{.RAM_stack= TASK_N2_STACK				}, //2
	{.RAM_stack= configMINIMAL_STACK_SIZE*2	}, //6
	{.RAM_stack= TASK_N3_STACK 				}  //3

};

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
		retval |= 1<<0;
	}

	retaux= xTaskCreate( TASK_N2, (const char *)
						 TASK_N2_DECRIPT,
						 TASK_N2_STACK,
						 0,
						 TASK_N2_PRIORITY,
						 &TASK_N2_HANDLER );

	if( pdPASS != retaux )
	{
		retval |= 1<<1;
	}

	retaux= xTaskCreate( TASK_N3, (const char *)
						 TASK_N3_DECRIPT,
						 TASK_N3_STACK,
						 0,
						 TASK_N3_PRIORITY,
						 &TASK_N3_HANDLER );

	if( pdPASS != retaux )
	{
		retval |= 1<<2;
	}

	retaux= xTaskCreate( TASK_N4, (const char *)
						 TASK_N4_DECRIPT,
						 TASK_N4_STACK,
						 0,
						 TASK_N4_PRIORITY,
						 &TASK_N4_HANDLER );

	if( pdPASS != retaux )
	{
		retval |= 1<<3;
	}

	return retval;
}

int queues_create (void)
{
	int retval= 0;

	MGR_INPUT_QUEUE  = xQueueCreate( MGR_INPUT_QUEUE_LENGT, MGR_INPUT_QUEUE_SIZE );
	MGR_OUTPUT_QUEUE = xQueueCreate( MGR_OUTPUT_QUEUE_LENGT, MGR_OUTPUT_QUEUE_SIZE );
	MGR_TERMINAL_QUEUE = xQueueCreate( MGR_TERMINAL_QUEUE_LEN, MGR_TERMINAL_QUEUE_SIZ );
	MGR_DATALOG_QUEUE = xQueueCreate( MGR_DATALOG_QUEUE_LENGT, MGR_DATALOG_QUEUE_SIZE );

	if( !MGR_INPUT_QUEUE || !MGR_OUTPUT_QUEUE || !MGR_TERMINAL_QUEUE || !MGR_DATALOG_QUEUE)
	{
		retval |= 1<<4;
	}

	MGR_DATALOG_MUTEX= xSemaphoreCreateMutex();
	MGR_TERMINAL_MUTEX = xSemaphoreCreateMutex();
	MGR_INPUT_MUTEX= xSemaphoreCreateMutex();

	if( !MGR_DATALOG_MUTEX || !MGR_TERMINAL_MUTEX || !MGR_INPUT_MUTEX )
	{
		retval |= 1<<5;
	}

	return retval;
}

int timers_create (void)
{
	int retval= 0;

	TIMER_1_OBJ = xTimerCreate( TIMER_1_NAME, TIMER_1_PER, TIMER_1_RELOAD, TIMER_1_IDn, TIMER_1_CALLBACK );
	TIMER_2_OBJ = xTimerCreate( TIMER_2_NAME, TIMER_2_PER, TIMER_2_RELOAD, TIMER_2_IDn, TIMER_2_CALLBACK );
	TIMER_3_OBJ = xTimerCreate( TIMER_3_NAME, TIMER_3_PER, TIMER_3_RELOAD, TIMER_3_IDn, TIMER_3_CALLBACK );
	TIMER_4_OBJ = xTimerCreate( TIMER_4_NAME, TIMER_4_PER, TIMER_4_RELOAD, TIMER_4_IDn, TIMER_4_CALLBACK );

	if( !TIMER_1_OBJ || !TIMER_2_OBJ || !TIMER_3_OBJ || !TIMER_4_OBJ )
	{
		retval |= 1<<6;
	}

	return retval;
}
/* This example demonstrates how a human readable table of run time stats
information is generated from raw data provided by uxTaskGetSystemState().
The human readable table is written to pcWriteBuffer.  (see the vTaskList()
API function which actually does just this). */

void vTaskGetRunTimeStats (char *pcWriteBuffer)
{
	volatile UBaseType_t x, uxArraySize;
	UBaseType_t ulTotalRunTime, ulStatsAsPercentage;
	static int first_time= 0;

	/* Generate raw status information about each task. */
	uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, NUM_TASK_TOTAL, &ulTotalRunTime );

	/* For percentage calculations. */
	ulTotalRunTime /= 100UL;

	/* Avoid divide by zero errors. */
	if( uxArraySize && ulTotalRunTime )
	{
		/* For each populated position in the pxTaskStatusArray array,
         format the raw data as human readable ASCII data. */
		for( x = 0; x < uxArraySize; x++ )
		{
			if( !first_time )
			{
				taskInfoProc[x].TASK_Name= pxTaskStatusArray[x].pcTaskName;
				taskInfoProc[x].TASK_Id= pxTaskStatusArray[x].xTaskNumber;
			}
			/* What percentage of the total run time has the task used?
            This will always be rounded down to the nearest integer.
            ulTotalRunTimeDiv100 has already been divided by 100. */
			ulStatsAsPercentage = pxTaskStatusArray[x].ulRunTimeCounter / ulTotalRunTime;
			taskInfoProc[x].CPU_tickCount= pxTaskStatusArray[x].ulRunTimeCounter;

			if( ulStatsAsPercentage )
			{
				taskInfoProc[x].CPU_rate= ulStatsAsPercentage;
			}
			else
			{
				/* If the percentage is zero here then the task has consumed less than 1% of the total run time. */
				taskInfoProc[x].CPU_rate= 0;
			}
			taskInfoProc[x].RAM_stackUsed= taskInfoProc[x].RAM_stack-pxTaskStatusArray[x].usStackHighWaterMark;
			//taskInfoProc[x].RAM_stackUsed= taskInfoProc[x].RAM_stack-stacktareas[x];
		}
		first_time= 1;
	}
}

void vApplicationIdleHook (void)
{
	static TickType_t prev_tick=0;
	TickType_t last_tick;

	//UPDATE_CPU_HIST(NUM_TASK);

	last_tick= xTaskGetTickCount();
	if( MSEC_REFRESH_CPU < (last_tick-prev_tick) )
	{
		vTaskGetRunTimeStats(NULL);
		prev_tick= last_tick;
	}

}

/* Defined in main.c. */
uint32_t TimerForRunTimeStats (void)
{
	return xTaskGetTickCountFromISR(); /* using RTOS tick counter */
}


