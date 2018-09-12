/*
 * filesManager.c
 *
 *  Created on: 20/7/2018
 *      Author: fran
 */


#include "ciaaAIN.h"
#include "shellManager.h"
#include "auxiliar_gpios_def.h"
#include "services_config.h"
#include "Pulse_Count_Task.h"

extern xQueueHandle 		HANDLER_QUEUE_INPUTS;
extern xQueueHandle 		HANDLER_QUEUE_OUTPUTS;

extern xTaskHandle 			HANDLER_MGR_INPUTS;
extern xTaskHandle 			HANDLER_MGR_OUTPUTS;
extern UBaseType_t*			STACKS_TAREAS;

void taskDataLogin (void * a)
{
	signal_t dataRecLed;
	//int32_t  tOnLeds [4];
	char sToSend[30]="Aun esta vacio";
	uint8_t  i, aux;

	while (1)
	{
		ACTUALIZAR_STACK_TAREA( HANDLER_MGR_OUTPUTS, ID_STACK_MGR_OUTPUT );

		if( pdTRUE == xQueueReceive( HANDLER_QUEUE_OUTPUTS, &dataRecLed, TIMEOUT_QUEUE_OUTPUT ))
		{
			for(i=0; i<4; i++)
			{
				aux= (1<<i);
				aux &= dataRecLed.led;
				if( aux )
				{
					Board_LED_Set( SELECT_LED(i), true );

					sprintf( sToSend, "[AI%d = %i uni]\r\n", i, dataRecLed.readVal[i] );
					printConsola( sToSend, MP_DEB );
				}
			}
		}
		else
		{
			for(i=0; i<4; i++)
			{
				Board_LED_Set( SELECT_LED(i), false );
			}
		}
	}
}
