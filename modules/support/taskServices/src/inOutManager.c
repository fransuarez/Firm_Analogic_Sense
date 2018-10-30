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
extern UBaseType_t*			STACKS_TAREAS;

extern xQueueHandle 		MGR_INPUT_QUEUE;
extern xQueueHandle 		MGR_OUTPUT_QUEUE;
extern xQueueHandle 		MGR_TERMINAL_QUEUE;
extern xQueueHandle 		MGR_DATALOG_QUEUE;

#define SIZE_ARRAY_BUFF		MGR_DATALOG_QUEUE_LENGT
#define SIZE_ITEMS_BUFF		25
#define MSG_LOG_DINPUT		"[DI_%d=%i state]"

typedef struct _linkList
{
	uint8_t** ptrBuff;
	uint8_t** ptrLast;
	uint16_t posDisp;
	uint8_t posNext;
	uint8_t sizeRow;

} linkList_t;

static linkList_t bufferState;
static uint8_t byteConteiner[SIZE_ARRAY_BUFF][SIZE_ITEMS_BUFF];


static inline void initLinkList (linkList_t * pMsgList, uint8_t** pConteiner, uint16_t sizeConteiner)
{
	pMsgList->ptrBuff= pConteiner;
	pMsgList->ptrLast= pConteiner;
	pMsgList->posDisp= sizeConteiner;
	pMsgList->posNext= 0;
	pMsgList->sizeRow= SIZE_ITEMS_BUFF;
}

int insertMsgToList (uint8_t gpioID, uint16_t value, genStr_t* retMsg, linkList_t* pMsgList)
{
	int retval=0;

	if( retMsg && pMsgList )
	{
		if( pMsgList->posDisp <= pMsgList->posNext )
		{
			pMsgList->posNext= 0;
		}
		pMsgList->ptrLast= (pMsgList->ptrBuff)+pMsgList->posNext*pMsgList->sizeRow;
		pMsgList->posNext++;

		retMsg->msg = (char *) pMsgList->ptrLast;
		retMsg->size= (uint16_t) sprintf( retMsg->msg, MSG_LOG_DINPUT, gpioID, value );
		retMsg->size++;

		retval= 1;
	}
	return retval;
}

void quitMsgToList (linkList_t * pMsgList)
{
	if( NULL != pMsgList->ptrBuff )
	{
		pMsgList= (linkList_t*)(pMsgList->ptrBuff);
		pMsgList->ptrLast= NULL;
	}
}

void taskControlOutputs (void * a)
{
	ledStat_t dataRecLed;
	static terMsg_t msgToSend;
	static char sToSend[30]= "Aun esta vacio";

	uint8_t i;

	ciaaLED_Init();

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_INPUT ))
		{
			for(i=0; i<4; i++)
			{
				if( (1<<i) & dataRecLed.led )
				{
					ciaaLED_Set( SELECT_LED(i), true );
					sprintf( sToSend, "[AI%d = %i uni]\r\n", i, dataRecLed.readVal[i] );

					Terminal_Msg_Def( &msgToSend, sToSend );
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

		ACTUALIZAR_STACK( MGR_OUTPUT_HANDLER, MGR_OUTPUT_ID_STACK );
	}
}

void taskControlInputs (void * a)
{
	tecStat_t dataRecKey;
	static ledStat_t dataToSend;
	static dlogPack_t dataToLog;

	ciaaTEC_Init();
	ciaaTEC_EnableIRQ( TECL1 );
	ciaaTEC_EnableIRQ( TECL2 );

	initLinkList( &bufferState, (uint8_t**) byteConteiner, SIZE_ARRAY_BUFF );

	while (1)
	{
		dataToSend.readVal= ADC_read();

		if( pdTRUE == xQueueReceive( MGR_INPUT_QUEUE, &dataRecKey, TIMEOUT_QUEUE_INPUT ))
		{
			if( TECL_PUSH == dataRecKey.state)
			{
			// Si presionan la tecla 1 entonces enciendo el led 1.
				if( TECL1 == dataRecKey.key )
				{
					dataToSend.led= LED_1;
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );
				}
				// Si presionan la tecla 1 entonces enciendo el led 1.
				if( TECL2 == dataRecKey.key )
				{
					insertMsgToList((uint8_t) dataRecKey.key, (uint16_t) dataRecKey.state, &dataToLog.data, &bufferState );

					xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
				}
			}
		}
		ACTUALIZAR_STACK( MGR_INPUT_HANDLER, MGR_INPUT_ID_STACK );
		vTaskDelay( MGR_INPUT_DELAY );
	}
}

/*==================[irq handlers functions ]=========================*/
void GPIO0_IRQHandler (void)
{
	tecStat_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	if( TEC1_PRESSED & ciaaTEC_Level_ISR( TECL1 ) )
	{
		dataToSend.key  = TECL1;
		dataToSend.state= TECL_PUSH;
		xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );
	}

	portEND_SWITCHING_ISR( xSwitchRequired );
}

void GPIO1_IRQHandler (void)
{
	static tecStat_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	if( TEC2_PRESSED & ciaaTEC_Level_ISR( TECL2 ) )
	{
		dataToSend.key  = TECL2;
		dataToSend.state= TECL_PUSH;
		xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );
	}

	portEND_SWITCHING_ISR( xSwitchRequired );
}

// Por el momento no voy a usar esta irq porque es muy sensible a los cambios por ruido en la AIN.
void ADC0_IRQHandler (void)
{
	/*
	portBASE_TYPE xSwitchRequired;

	ADC_IRQ0Support();
	portEND_SWITCHING_ISR( xSwitchRequired );
	*/
}

