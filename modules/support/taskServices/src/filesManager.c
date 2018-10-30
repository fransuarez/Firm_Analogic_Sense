/*
 * ilesManager.c
 *
 *  Created on: 20/7/2018
 *      Author: fran
 */

#include "services_config.h"
#include "terminalManager.h"
#include "api_RTC.h"
#include "api_EEPROM.h"

extern xTaskHandle 			MGR_DATALOG_HANDLER;
extern UBaseType_t*			STACKS_TAREAS;

extern xQueueHandle 		MGR_DATALOG_QUEUE;
extern xQueueHandle 		MGR_TERMINAL_QUEUE;

static RTC_t RTC =
{
	.mday = 14,
	.month= 9,
	.year = 2018,
	.wday = 5,
	.hour = 12,
	.min  = 0,
	.sec  = 0
};

static char sToSend[124]= "Aun esta vacio";

void dataLog_Service (void * a)
{
	dlogPack_t dataRecived;
	static terMsg_t msgToSend;

	RTC_Init();
	RTC_setTime( &RTC );

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_DATALOG_QUEUE, &dataRecived, TIMEOUT_QUEUE_LOG_INP ))
		{
			RTC_getTime(&RTC);
			sprintf( sToSend, "%2d:%2d:%2d-%s", RTC.hour, RTC.min, RTC.sec, dataRecived.data.msg );

			Terminal_Msg_Def( &msgToSend, sToSend );
			xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
		}
		else
		{
		}

		ACTUALIZAR_STACK( MGR_DATALOG_HANDLER, MGR_DATALOG_ID_STACK );
		vTaskDelay( MGR_DATALOG_DELAY );
	}
}

void dataLog_RTC_Init( void )
{

}

void dataLog_RTC_Update ( int logClock )
{

}
