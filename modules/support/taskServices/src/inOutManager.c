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

extern xTaskHandle 			MGR_INPUT_HANDLER;
extern xTaskHandle 			MGR_OUTPUT_HANDLER;

extern xQueueHandle 		MGR_INPUT_QUEUE;
extern xQueueHandle 		MGR_OUTPUT_QUEUE;
extern xQueueHandle 		MGR_TERMINAL_QUEUE;

extern UBaseType_t*			STACKS_TAREAS;

void taskControlOutputs (void * a)
{
	ledStat_t dataRecLed;
	terMsg_t msgToSend;
	static char sToSend[30]= "Aun esta vacio";
	uint8_t i;

	ciaaLED_Init();

	while (1)
	{
		ACTUALIZAR_STACK( MGR_OUTPUT_HANDLER, MGR_OUTPUT_ID_STACK );

		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_OUTPUT ))
		{
			for(i=0; i<4; i++)
			{
				if( dataRecLed.led & (1<<i) )
				{
					ciaaLED_Set( SELECT_LED(i), true );
					sprintf( sToSend, "[AI%d = %i uni]\r\n", i, dataRecLed.readVal[i] );

					Terminal_Msg_Promt( &msgToSend, sToSend );
					xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
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
	tecStat_t dataRecKey;
	ledStat_t dataToSend;

	ciaaTEC_Init();
	ciaaKEY_EnableIRQ( TECL1 );

	while (1)
	{
		ACTUALIZAR_STACK( MGR_INPUT_HANDLER, MGR_INPUT_ID_STACK );

		dataToSend.readVal= ADC_read();

		if( pdTRUE == xQueueReceive( MGR_INPUT_QUEUE, &dataRecKey, TIMEOUT_QUEUE_INPUT ))
		{
			if( !dataRecKey.state )
			{
				if( TECL1 == dataRecKey.key )
				{
					dataToSend.led= LED_1;
				}
				xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_INPUT );
			}
		}
		vTaskDelay( MGR_INPUT_DELAY );
	}
}

/*==================[irq handlers functions ]=========================*/
void GPIO0_IRQHandler (void)
{
	tecStat_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	dataToSend.key= TECL1;
	dataToSend.state= TECL_FREE;

	if( TEC1_PRESSED == ciaaTEC_Level_ISR( TECL1 ) )
	{
		dataToSend.state= TECL_PUSH;
	}

	xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );

	portEND_SWITCHING_ISR( xSwitchRequired );
}

// Por el momento no voy a usar esta irq porque es muy sensible a los cambios por ruido en la AIN.
void ADC0_IRQHandler (void)
{
	portBASE_TYPE xSwitchRequired;

	ADC_IRQ0Support();
	portEND_SWITCHING_ISR( xSwitchRequired );
}

