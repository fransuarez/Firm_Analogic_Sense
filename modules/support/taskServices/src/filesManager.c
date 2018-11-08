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
extern xSemaphoreHandle 	MGR_INPUT_MUTEX;

#define LOG_MAX_REGISTER	30

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

typedef struct _data_page
{
	char 		time[8]; 				// Longitud de "HH:MM:SS". Tamaño total 8 bytes o 2 words.
	GpioReg_t* 	regMem;  	// Cantidad de registros almacenadaos. Tamaño total REG_OPR_LOG*4 Bytes=120 Bytes o 30 Words.

} DataP_t; 								// El tamaño de la estructura es de 128 bytes o 32 words.

typedef struct _control_eeprom
{
	uint32_t nextReg; 	// Longitud de "HH:MM:SS"
	uint32_t numReg;

} CtrlE2P_t;

static CtrlE2P_t regEeprom;
static DataP_t readEeprom;
static DataP_t sendEeprom;
//static char sToSend[]= "[LOG send to EEPROM]/n/r";

static void dataLog_RTC_Update (char * regRTC)
{
	RTC_getTime(&RTC);
	sprintf( regRTC, "%2d:%2d:%2d", RTC.hour, RTC.min, RTC.sec );
}

static int dataLog_Store_Page (uint16_t numReg)
{
	uint32_t sizebloq;
	uint32_t retval=0;

	dataLog_RTC_Update( sendEeprom.time );

	sizebloq= numReg*sizeof(GpioReg_t)+strlen(sendEeprom.time);

	if( LOG_MAX_REGISTER == numReg && EEPROM_PAGE_SIZE == sizebloq )
	{
		EEPROM_writeWord( regEeprom.nextReg, (uint32_t) sendEeprom.time[0] );
		EEPROM_writeWord( regEeprom.nextReg, (uint32_t) sendEeprom.time[4] );

		regEeprom.nextReg += 0x0008;
		if( EEPROM_writePage( regEeprom.nextReg, (uint8_t *) sendEeprom.regMem ) )
		{
			if( EEPROM_ADDRESS_HIGH > regEeprom.nextReg )
			{
				regEeprom.nextReg += EEPROM_PAGE_SIZE;
				regEeprom.numReg++;
			}
			else
			{
				regEeprom.nextReg=0;
				regEeprom.numReg=EEPROM_PAGE_NUM-1; // Fixme Por ahora si llego al maximo lo dejo con el contador al maximo.
			}
			retval=1;
		}

	}
	return retval;
}

void dataLog_Service (void * a)
{
	dlogPack_t dataRecived;
	//static terMsg_t msgToSend;
	EEPROM_init( EEPROM_AUTOPROG_AFT_LASTWORDWRITTEN ); // Modo: EEPROM_AUTOP_1WORDWRITTEN sirve mas para escribir de a words/bytes
	RTC_Init();
	RTC_setTime( &RTC );
	regEeprom.numReg=0;
	regEeprom.nextReg=0x0000;

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_DATALOG_QUEUE, &dataRecived, TIMEOUT_QUEUE_LOG_INP ))
		{
			switch( dataRecived.cmd )
			{
				case writePage:
					sendEeprom.regMem= dataRecived.data;
					if( TOMAR_SEMAFORO(	MGR_INPUT_MUTEX, TIMEOUT_MUTEX_INPUT) )
					{
						dataLog_Store_Page( dataRecived.nReg );
						//Terminal_Msg_Def( &msgToSend, sToSend );
						//xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
					}
					break;
				default:
					break;
			}
		}
		else
		{
		}

		ACTUALIZAR_STACK( MGR_DATALOG_HANDLER, MGR_DATALOG_ID_STACK );
		vTaskDelay( MGR_DATALOG_DELAY );
	}
}

void FLASH_EEPROM_IRQHandler (void)
{
	//portBASE_TYPE xSwitchRequired=0;

	EEPROM_writeEnd_ISR();
	if( LIBERAR_SEMAFORO( MGR_INPUT_MUTEX ) ){};

	//portEND_SWITCHING_ISR( xSwitchRequired );
	portYIELD();
}
