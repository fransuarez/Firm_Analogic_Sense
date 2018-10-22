/*
 * ilesManager.c
 *
 *  Created on: 20/7/2018
 *      Author: fran
 */

#include "services_config.h"
#include "terminalManager.h"
#include "api_RTC.h"
#include "ciaaLED.h"
#include "api_EEPROM.h"

extern xTaskHandle 			MGR_OUTPUT_HANDLER;
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

void dataLog_Service (void * a)
{
	ledStat_t dataRecLed;
	terMsg_t msgToSend;
	char sToSend[124]= "Aun esta vacio";

	RTC_Init();
	RTC_setTime( &RTC );

	while (1)
	{
		ACTUALIZAR_STACK( MGR_DATALOG_HANDLER, MGR_DATALOG_ID_STACK );

		if( pdTRUE == xQueueReceive( MGR_DATALOG_QUEUE, &dataRecLed, TIMEOUT_QUEUE_LOG_INP ))
		{

			sprintf( sToSend, "[%d:%d:%d - AI%d = %i uni]\r\n",
					 RTC.hour, RTC.min, RTC.sec,i, dataRecLed.readVal[i] );

			Terminal_Msg_Promt( &msgToSend, sToSend );
			xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
		}
		else
		{
		}
	}
}

void dataLog_RTC_Init( void )
{

}

void dataLog_RTC_Update ( int logClock )
{

}
