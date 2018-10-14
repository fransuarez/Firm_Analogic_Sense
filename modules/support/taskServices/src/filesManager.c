/*
 * filesManager.c
 *
 *  Created on: 20/7/2018
 *      Author: fran
 */


#include "services_config.h"
#include "terminalManager.h"
#include "api_RTC.h"
#include "ciaaLED.h"
#include "api_EEPROM.h"

#include "Pulse_Count_Task.h"

extern xQueueHandle 		MGR_INPUT_QUEUE;
extern xQueueHandle 		MGR_OUTPUT_QUEUE;

extern xTaskHandle 			MGR_INPUT_HANDLER;
extern xTaskHandle 			MGR_OUTPUT_HANDLER;
extern UBaseType_t*			STACKS_TAREAS;
RTC_t RTC = { 2018, 9, 14, 5, 12, 0, 0 };

void taskDataLogin (void * a)
{
	signal_t dataRecLed;
	//int32_t  tOnLeds [4];
	char sToSend[124]= "Aun esta vacio";
	uint8_t  i;
	static uint8_t  analg_val;

	RTC_Init();
	RTC_setTime( &RTC );

	while (1)
	{
		ACTUALIZAR_STACK( MGR_OUTPUT_HANDLER, MGR_OUTPUT_ID_STACK );

		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_OUTPUT ))
		{
			for(i=0; i<4; i++)
			{
				analg_val= (1<<i);
				analg_val &= dataRecLed.led;

				if( analg_val )
				{
					ciaaLED_Set( SELECT_LED(i), true );

					sprintf( sToSend, "[%d:%d:%d - AI%d = %i uni]\r\n",
							RTC.hour, RTC.min, RTC.sec,i, dataRecLed.readVal[i] );
					terminal_msg_debug( sToSend );
				}
			}
		}
		else
		{
			for(i=0; i<4; i++)
			{
				ciaaLED_Set( SELECT_LED(i), false );
			}

		}
	}
}

void dataLog_RTC_Init( void )
{

}

void dataLog_RTC_Update ( int logClock )
{

}
