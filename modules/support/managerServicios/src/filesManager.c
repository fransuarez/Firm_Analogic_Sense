/*
 * filesManager.c
 *
 *  Created on: 20/7/2018
 *      Author: fran
 */


#include "api_RTC.h"
#include "ciaaLED.h"
#include "api_EEPROM.h"

#include "shellManager.h"
#include "auxiliar_gpios_def.h"
#include "services_config.h"
#include "Pulse_Count_Task.h"

extern xQueueHandle 		HANDLER_QUEUE_INPUTS;
extern xQueueHandle 		HANDLER_QUEUE_OUTPUTS;

extern xTaskHandle 			HANDLER_MGR_INPUTS;
extern xTaskHandle 			HANDLER_MGR_OUTPUTS;
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
		ACTUALIZAR_STACK_TAREA( HANDLER_MGR_OUTPUTS, ID_STACK_MGR_OUTPUT );

		if( pdTRUE == xQueueReceive( HANDLER_QUEUE_OUTPUTS, &dataRecLed, TIMEOUT_QUEUE_OUTPUT ))
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
					printConsola( sToSend, MP_DEB );
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
