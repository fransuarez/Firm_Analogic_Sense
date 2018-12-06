/*
 * ilesManager.c
 *
 *  Created on: 20/7/2018
 *      Author: fran
 */

#include "binaryFiles.h"
#include "api_EEPROM.h"

#include "services_config.h"
#include "terminalManager.h"
#include <stdio.h>


extern TaskHandle_t			MGR_DATALOG_HANDLER;
extern UBaseType_t*			STACKS_TAREAS;

extern QueueHandle_t 		MGR_DATALOG_QUEUE;
extern QueueHandle_t 		MGR_TERMINAL_QUEUE;
extern SemaphoreHandle_t 	MGR_INPUT_MUTEX;
extern SemaphoreHandle_t 	MGR_DATALOG_MUTEX;

#define LOG_MAX_REGISTER	REG_OPR_LOG
#define SIZE_HEAD_TERMINAL  ( 20 )
#define SIZE_BUFF_TERMINAL	(SIZE_BYTES_REG_LOG + SIZE_HEAD_TERMINAL )

static char buffSendTerminal[SIZE_BUFF_TERMINAL]= "";
static uint8_t readMemory[SIZE_BYTES_REG_LOG];

//**********************************************************************************************
void convertByteToChar (char* buffOut, uint8_t * buffIn, int size)
{
	int i;

	for (i = 0; i < size; ++i)
	{
		sprintf( buffOut+i, "%x", buffIn[i] );

	}
}

static int dataLog_readRegisters (uint16_t *number, uint16_t *id)
{
	int retaux, retval=0;
	uint16_t numReg;
	terMsg_t msgToSend;
	dlogMem_t readData;

	if( dataLog_GetStatus( &numReg ) )
	{
		//Terminal_Take( &msgToSend );
		//xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );

		if( numReg && (numReg >= *number) )
		{
			numReg = *number;
		}

		if( *id < numReg )
		{
			readData.sizeReg= SIZE_BYTES_REG_LOG;
			readData.dataReg= readMemory;

			taskENTER_CRITICAL();
			retaux= dataLog_Page_Load( &readData, *id);
			taskEXIT_CRITICAL();

			if( retaux )
			{
				//while( TOMAR_SEMAFORO(	MGR_DATALOG_MUTEX, TIMEOUT_MUTEX_INPUT) );
				sprintf( buffSendTerminal, "Reg %i %i:%i - ", *id, readData.hourReg, readData.minuReg );
				convertByteToChar( &buffSendTerminal[strlen(buffSendTerminal)], readData.dataReg, SIZE_BYTES_REG_LOG );
				//LIBERAR_SEMAFORO( MGR_DATALOG_MUTEX );

				Terminal_Msg_Normal( &msgToSend, buffSendTerminal );
				xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
				vTaskDelay( MGR_DATALOG_PRINT_REG );
			}
			++(*id);
			retval= 1;
		}
		//Terminal_Release( &msgToSend );
		//xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
	}
	return retval;
}

static int dataLog_writeRegister (dlogPack_t* dataRecived)
{
	dlogMem_t  dataLogin;
	int retval;
	static uint32_t count_fail=0;

	if( TOMAR_SEMAFORO(	MGR_INPUT_MUTEX, TIMEOUT_MUTEX_INPUT) )
	{
		dataLogin.hourReg= dataRecived->auxVal_1;
		dataLogin.minuReg= dataRecived->auxVal_2;
		dataLogin.sizeReg= dataRecived->nReg*SIZE_BYTES_GPIO_REG;
		dataLogin.dataReg= (uint8_t*) dataRecived->data;
		// FIXME falta agregar la conversion de tiempo a la escala de tiempo, No usar esta porque la conversion
		// 		 ya la hice antes en cada registro. Aca deberia implementar la desconversion.
		taskENTER_CRITICAL();
		retval= dataLog_Page_Store( &dataLogin );
		taskEXIT_CRITICAL();

		if( !LIBERAR_SEMAFORO( MGR_INPUT_MUTEX ) )
		{
			count_fail++;
		}
	}

	return retval;
}

//**********************************************************************************************
void dataLog_Service (void * a)
{
	dlogPack_t dataRecived;
	int modeMonitor=0;
	uint16_t idReg, numberReg;
	uint32_t count_fail=0;

	//static terMsg_t msgToSend;
	if( !dataLog_Init() )
	{
		while(1);
	}

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_DATALOG_QUEUE, &dataRecived, TIMEOUT_QUEUE_LOG_INP ))
		{
			switch( dataRecived.cmd )
			{
				case writePage:
					if( !dataLog_writeRegister( &dataRecived ) )
					{
						//if(LIBERAR_SEMAFORO( MGR_INPUT_MUTEX )){}
						count_fail++;
					}
					//Terminal_Msg_Def( &msgToSend, sToSend );
					//xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
					break;

				case readPage:
					if( dataRecived.nReg )
					{
						idReg= dataRecived.auxVal_1;
						numberReg= dataRecived.nReg;
						modeMonitor= 1;
					}
					else
					{
						modeMonitor= 0;
					}
					break;

				default:
					break;
			}
		}
		else
		{
			if( modeMonitor )
			{
				if( numberReg && !dataLog_readRegisters( &numberReg, &idReg ) )
				{
					modeMonitor=0;
				}
			}
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
