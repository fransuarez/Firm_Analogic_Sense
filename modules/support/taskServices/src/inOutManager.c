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

extern xSemaphoreHandle 	MGR_INPUT_MUTEX;

#define SIZE_BUFF_TERM		32

static char 		buffSendTerminal[SIZE_BUFF_TERM]= "vacio";
static GpioReg_t 	regToLog[REG_OPR_LOG];
static dlogPack_t 	dataToLog = { .cmd= writePage, .data= regToLog, .nReg= REG_OPR_LOG };
static int idReg=0;

/*

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

static linkList_t 	bufferState;
static uint8_t 		byteConteiner[SIZE_ARRAY_BUFF][SIZE_ITEMS_BUFF];

static inline void initLinkList (linkList_t * pMsgList, uint8_t** pConteiner, uint16_t sizeConteiner)
{
	pMsgList->ptrBuff= pConteiner;
	pMsgList->ptrLast= pConteiner;
	pMsgList->posDisp= sizeConteiner;
	pMsgList->posNext= 0;
	pMsgList->sizeRow= SIZE_ITEMS_BUFF;
}

static int insertMsgToList (uint8_t gpioID, uint16_t value, genStr_t* retMsg, linkList_t* pMsgList)
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

static void quitMsgToList (linkList_t * pMsgList)
{
	if( NULL != pMsgList->ptrBuff )
	{
		pMsgList= (linkList_t*)(pMsgList->ptrBuff);
		pMsgList->ptrLast= NULL;
	}
}
*/

static int historyInputReg (GpioReg_t* data)
{
	int retval=0;

	if( data )
	{
		if( TOMAR_SEMAFORO(	MGR_INPUT_MUTEX, TIMEOUT_MUTEX_INPUT) )
		{
			memcpy( &regToLog[idReg], data, sizeof(GpioReg_t));

			if( REG_OPR_LOG <= (++idReg) )
			{
				idReg= 0;
				retval= 1;
			}
			if( LIBERAR_SEMAFORO( MGR_INPUT_MUTEX ) ){};
		}
	}
	return retval;
}

void taskControlOutputs (void * a)
{
	static terMsg_t msgToSend= { .mode= MP_DEF, .msg=buffSendTerminal, .size=SIZE_BUFF_TERM };
	GpioReg_t dataRecLed;
	uint8_t i;

	ciaaLED_Init();

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_INPUT ))
		{
			if( outputLed == dataRecLed.type )
			{
				ciaaLED_Set( dataRecLed.id, dataRecLed.value );

				sprintf( buffSendTerminal, "[LED_%d= %i uni]\r\n", dataRecLed.id, dataRecLed.value );
				Terminal_Msg_Def( &msgToSend, buffSendTerminal );
				xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
			}

			if( historyInputReg( &dataRecLed ))
			{
				xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
			}
		}
		else
		{
			for(i=LED_B; i<= LED_3; i++)
			{
				ciaaLED_Set( i, FALSE );
			}
		}

		ACTUALIZAR_STACK( MGR_OUTPUT_HANDLER, MGR_OUTPUT_ID_STACK );
	}
}

void taskControlInputs (void * a)
{
	static GpioReg_t dataToSend;
	static terMsg_t msgToSend;
	GpioReg_t dataRecKey;

	ciaaTEC_Init();
	ADC_init();
	ciaaTEC_EnableIRQ( TECL1 );
	ciaaTEC_EnableIRQ( TECL2 );


	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_INPUT_QUEUE, &dataRecKey, TIMEOUT_QUEUE_INPUT ))
		{
			if( inputTecl == dataRecKey.type && TECL_PUSH == dataRecKey.value)
			{
				dataToSend.type= outputLed;
				dataToSend.value= TRUE;
			// Si presionan la tecla 1 entonces enciendo el led 1.
				if( TECL1 == dataRecKey.id )
				{
					dataToSend.id  = LED_B;
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );
				}
				// Si presionan la tecla 2 entonces se envia un mensaje con el valor de la entrada analogica.
				if( TECL2 == dataRecKey.id )
				{
					dataToSend.id  = LED_1;
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );

					sprintf( buffSendTerminal, "[AI_%d= %i uni]\r\n", AIN_1, ADC_read(AIN_1) );
					Terminal_Msg_Def( &msgToSend, buffSendTerminal );
					xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
				}
			}

			if( historyInputReg( &dataRecKey ))
			{
				xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
			}
		}

		ACTUALIZAR_STACK( MGR_INPUT_HANDLER, MGR_INPUT_ID_STACK );
		vTaskDelay( MGR_INPUT_DELAY );
	}
}

/*==================[irq handlers functions ]=========================*/
void GPIO0_IRQHandler (void)
{
	static GpioReg_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	dataToSend.value= TECL_FREE;

	if( TEC1_PRESSED & ciaaTEC_Level_ISR( TECL1 ) )
	{
		dataToSend.type = inputTecl;
		dataToSend.id  = TECL1;
		dataToSend.value= TECL_PUSH;
	}
	xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );

	portYIELD_FROM_ISR( xSwitchRequired );
}

void GPIO1_IRQHandler (void)
{
	static GpioReg_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	dataToSend.value= TECL_FREE;

	if( TEC2_PRESSED & ciaaTEC_Level_ISR( TECL2 ) )
	{
		dataToSend.type = inputTecl;
		dataToSend.id  = TECL2;
		dataToSend.value= TECL_PUSH;
	}
	xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );

	portYIELD_FROM_ISR( xSwitchRequired );
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

