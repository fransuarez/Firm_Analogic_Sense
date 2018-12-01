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


extern xTaskHandle 			MGR_DATALOG_HANDLER;
extern UBaseType_t*			STACKS_TAREAS;

extern xQueueHandle 		MGR_DATALOG_QUEUE;
extern xQueueHandle 		MGR_TERMINAL_QUEUE;
extern xSemaphoreHandle 	MGR_INPUT_MUTEX;

#define LOG_MAX_REGISTER	REG_OPR_LOG


//**********************************************************************************************


//**********************************************************************************************
void dataLog_Service (void * a)
{
	dlogPack_t dataRecived;
	dlogMem_t  dataLogin;
	static uint32_t count_fail=0;

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
					if( TOMAR_SEMAFORO(	MGR_INPUT_MUTEX, TIMEOUT_MUTEX_INPUT) )
					{
						dataLogin.hourReg= dataRecived.hourSamples;
						dataLogin.minuReg= dataRecived.minuSamples;
						dataLogin.sizeReg= dataRecived.nReg*SIZE_BYTES_GPIO_REG;
						dataLogin.dataReg= (uint8_t*) dataRecived.data;
						// FIXME falta agregar la conversion de tiempo a la escala de tiempo, No usar esta porque la conversion
						// 		 ya la hice antes en cada registro. Aca deberia implementar la desconversion.
						if( !dataLog_Page_Store( &dataLogin ) )
						{
							//if(LIBERAR_SEMAFORO( MGR_INPUT_MUTEX )){}
							count_fail++;
						}

						if(LIBERAR_SEMAFORO( MGR_INPUT_MUTEX ))
						{
							count_fail++;
						}

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
