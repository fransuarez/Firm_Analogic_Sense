/*
 * IO_Manager.c
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#include "services_config.h"
#include "terminalManager.h"

#include "ciaaGPIO_def.h"
#include "ciaaTEC.h"
#include "ciaaLED.h"
#include "ciaaAIN.h"


#define SIZE_BUFF_TERM		32
#define SAMPLE_MAX_SEC		255 //(1<<8)-1 == 4'15"

extern xTaskHandle 			MGR_INPUT_HANDLER;
extern xTaskHandle 			MGR_OUTPUT_HANDLER;
extern UBaseType_t*			STACKS_TAREAS;

extern xQueueHandle 		MGR_INPUT_QUEUE;
extern xQueueHandle 		MGR_OUTPUT_QUEUE;
extern xQueueHandle 		MGR_TERMINAL_QUEUE;
extern xQueueHandle 		MGR_DATALOG_QUEUE;
extern xSemaphoreHandle 	MGR_INPUT_MUTEX;

static char 				buffSendTerminal[SIZE_BUFF_TERM]= "vacio";
static GpioReg_t 			regToLog[REG_OPR_LOG];
static dlogPack_t 			dataToLog = { .cmd= writePage, .data= regToLog, .nReg= REG_OPR_LOG, .minutes=0, .seconds=0 };
static int idReg=0;

static int historyInputReg (GpioReg_t* data)
{
	int retval=0;

	if( data && TOMAR_SEMAFORO(	MGR_INPUT_MUTEX, TIMEOUT_MUTEX_INPUT) )
	{
		memcpy( &regToLog[idReg], data, sizeof(GpioReg_t));

		// Si se completa el total de registros entonces reinicia desde el principio y
		// se debe escibir el registro completo en memoria.
		if( REG_OPR_LOG <= (++idReg) )
		{
			idReg= 0;
			retval= 1;
		}
		if( LIBERAR_SEMAFORO( MGR_INPUT_MUTEX ) ){};
	}
	return retval;
}

void taskControlOutputs (void * a)
{
	static terMsg_t msgToSend= { .mode= MP_DEF, .msg=buffSendTerminal, .size=SIZE_BUFF_TERM };
	dOutputQueue_t dataRecLed;
	uint8_t i;

	ciaaLED_Init();

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_INPUT ))
		{
			if( outputLed == GET_TYPE(dataRecLed.gpio) )
			{
				ciaaLED_Set( GET_ID(dataRecLed.gpio), dataRecLed.data.value );

				sprintf( buffSendTerminal, "[LED_%d= %i uni]\r\n", GET_ID(dataRecLed.gpio), dataRecLed.data.value );
				Terminal_Msg_Def( &msgToSend, buffSendTerminal );
				xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
			}

			if( historyInputReg( &dataRecLed.data ))
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
	static dOutputQueue_t dataToSend;
	//static terMsg_t msgToSend;
	dInputQueue_t dataRecKey;

	ciaaTEC_Init();
	ciaaAIN_Init();
	ciaaTEC_EnableIRQ( TECL1 );
	ciaaTEC_EnableIRQ( TECL2 );

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_INPUT_QUEUE, &dataRecKey, TIMEOUT_QUEUE_INPUT ))
		{
			if( inputTecl == GET_TYPE(dataRecKey.gpio) && TECL_PUSH == dataRecKey.data.value)
			{
				dataToSend.gpio= outputLed;
				dataToSend.data.value= TRUE;
			// Si presionan la tecla 1 entonces enciendo el led 1.
				if( TECL1 == GET_ID(dataRecKey.gpio) )
				{
					dataToSend.gpio |= LED_B;
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );
				}
				// Si presionan la tecla 2 entonces se envia un mensaje con el valor de la entrada analogica.
				if( TECL2 == dataRecKey.gpio )
				{
					dataToSend.gpio |= LED_1;
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );
				}
				// Si se completa el registro de RAM se envia a la eeprom:
				if( historyInputReg( &dataRecKey.data ))
				{
					xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
				}
			}

			if( inputConfig == GET_TYPE(dataRecKey.gpio) )
			{
				uint8_t id= GET_ID(dataRecKey.gpio);

				if( AINP_VALID(id) )
				{
					//if(  func_conf_termistor == dataRecKey.mode ){}
					ciaaAIN_Config( dataRecKey.conf.auxvalue1, dataRecKey.conf.auxvalue2, dataRecKey.mode, dataRecKey.gpio);
				}
				else if( TECL_VALID(id)  )
				{

				}

			}
		}
		else // FIXME EN VEZ DE ESTO TENGO QUE USAR TIMERS DE FREERTOS O DEL SISTEMA.
		{
			/*
			sprintf( buffSendTerminal, "[AI_%d= %i uni]\r\n", AIN_1, ADC_read(AIN_1) );
			Terminal_Msg_Def( &msgToSend, buffSendTerminal );
			xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
			 */
			ciaaAIN_Update();
		}
		// FIXME EN VEZ DE ESTE SISTEMA TENGO QUE USAR DEBOUNCE CON TIMERS PARA LOS DEMAS ENTRADAS BINARIAS. O USO TODAS LAS IRQ.
		// TODO UNA VEZ RECOGIDOS LOS DATOS, ESTOS SE GRABARAN EN UNA TABLA GENERAL. AHI ESTARAN LAS ACCIONES CORRESPONDIENTES
		// 		A REALIZAR CON LOS CAMBIOS EN ELLAS. USARE UNA MATRIZ DE RELACION ENTRE DISPAROS DE LAS VARIABLES PARA ACCIONAR
		// 		LAS SALIDAS. LOS PARAMETROS DE TIEMPO POR ETAPA LOS TRABAJARA LAS SALIDAS.
		// 		LAS ACCIONES SE ENVIARAN VARIAS JUNTAS EN LA COLA DEL MODULO DE SALIDA. IDEM CON LA COLA DE GRABADO Y DEL TERMINAL.

		ACTUALIZAR_STACK( MGR_INPUT_HANDLER, MGR_INPUT_ID_STACK );
		vTaskDelay( MGR_INPUT_DELAY );
	}
}

/*==================[irq handlers functions ]=========================*/
void GPIO0_IRQHandler (void)
{
	static dInputQueue_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	dataToSend.data.value= TECL_FREE;

	if( TEC1_PRESSED & ciaaTEC_Level_ISR( TECL1 ) )
	{
		dataToSend.gpio = inputTecl;
		dataToSend.gpio|= TECL1;
		dataToSend.data.value= TECL_PUSH;
	}
	xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );

	portYIELD_FROM_ISR( xSwitchRequired );
}

void GPIO1_IRQHandler (void)
{
	static dInputQueue_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	dataToSend.data.value= TECL_FREE;

	if( TEC2_PRESSED & ciaaTEC_Level_ISR( TECL2 ) )
	{
		dataToSend.gpio = inputTecl;
		dataToSend.gpio|= TECL2;
		dataToSend.data.value= TECL_PUSH;
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

