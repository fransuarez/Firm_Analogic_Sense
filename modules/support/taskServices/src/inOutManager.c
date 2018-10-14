/*
 * IO_Manager.c
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#include "services_config.h"
#include "terminalManager.h"

#include "api_ADC.h"
#include "ciaaTEC.h"
#include "ciaaLED.h"

extern xQueueHandle 		MGR_INPUT_QUEUE;
extern xTaskHandle 			MGR_INPUT_HANDLER;

extern xQueueHandle 		MGR_OUTPUT_QUEUE;
extern xTaskHandle 			MGR_OUTPUT_HANDLER;
extern UBaseType_t*			STACKS_TAREAS;

void taskControlOutputs (void * a)
{
	signal_t dataRecLed;
	char sToSend[30]="Aun esta vacio";
	uint8_t  i, aux;

	ciaaLED_Init();

	while (1)
	{
		ACTUALIZAR_STACK( MGR_OUTPUT_HANDLER, MGR_OUTPUT_ID_STACK );

		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_OUTPUT ))
		{
			for(i=0; i<4; i++)
			{
				aux= (1<<i);
				aux &= dataRecLed.led;
				if( aux )
				{
					ciaaLED_Set( SELECT_LED(i), true );

					sprintf( sToSend, "[AI%d = %i uni]\r\n", i, dataRecLed.readVal[i] );
					// FIXME: esto debe ir a traves de una cola para que lo procese la tarea terminal directamente.
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

void taskControlInputs (void * a)
{
	queue_t dataRecKey;
	signal_t dataToSend;

	ciaaTEC_Init();
	dataToSend.led= (LED_1 | LED_2 | LED_3);

	while (1)
	{
		ACTUALIZAR_STACK( MGR_INPUT_HANDLER, MGR_INPUT_ID_STACK );

		dataToSend.readVal= ADC_read();

		if( pdTRUE == xQueueReceive( MGR_INPUT_QUEUE, &dataRecKey, TIMEOUT_QUEUE_INPUT ))
		{
			if( !dataRecKey.state )
			{
				xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_INPUT );
			}
		}

		vTaskDelay( MGR_INPUT_DELAY );
	}
}

/*==================[irq handlers functions ]=========================*/
void GPIO0_IRQHandler (void)
{
	queue_t dataToSend;
	portBASE_TYPE xSwitchRequired;
	uint8_t retaux;

	dataToSend.key= TECL1;

	retaux= LEVEL_KEY_Detec_From_ISR( TECL1 );

	if( LO_LEVEL == retaux)
	{
		dataToSend.state= LO_LEVEL;
		xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );
	}

	portEND_SWITCHING_ISR( xSwitchRequired );
}

// Por el momento no voy a usar esta irq porque es muy sensible a los cambios por ruido en la AIN.
void ADC0_IRQHandler (void)
{
	portBASE_TYPE xSwitchRequired;

	ADC_IRQ0Support();
	portEND_SWITCHING_ISR( xSwitchRequired );
}

