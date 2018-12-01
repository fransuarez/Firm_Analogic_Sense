/*
 * binaryFiles.c
 *
 *  Created on: Nov 28, 2018
 *      Author: fran
 */
#include "binaryFiles.h"
#include "api_RTC.h"

//**********************************************************************************************

typedef struct _control_eeprom
{
	uint16_t add_otherReg;		// Memoria restante
	uint16_t add_num_register; 	// Maximo preestablecido 112
	uint16_t add_lastTime; 		// Longitud de "HH:MM:SS"
	uint16_t add_nextTime;
	uint16_t add_lastReg; 		// Longitud de "HH:MM:SS"
	uint16_t add_nextReg;

} CtrlE2P_t; // Longitud en words= 3

typedef struct _reduced_time
{
	uint8_t hour;
	uint8_t min;
	uint8_t wday;
	uint8_t token;

} liteTime_t;

// El tamaño de la estructura es de 128 bytes o 32 words.
typedef struct _data_page
{
	liteTime_t 	regTime; 	// Longitud de "HH:MM:SS". Tamaño total 8 bytes o 2 words. FIXME esto deberia ser el calendario completo.
	uint8_t* 	regMem;  	// Cantidad de registros almacenadaos. Tamaño total REG_OPR_LOG*4 Bytes=120 Bytes o 30 Words.
							// Podria tratarlo como uint8_t * o (byte *)
} DataP_t;

//**********************************************************************************************
static CtrlE2P_t regEeprom;

static DataP_t readEeprom;
static DataP_t sendEeprom;
//static char sToSend[]= "[LOG send to EEPROM]/n/r";
static RTC_t dlRTC;

//**********************************************************************************************
static void dataLog_RTC_Setup (void)
{
	RTC_t ctRTC =
	{
		.mday = 1,
		.month= 1,
		.year = 2018,
		.wday = 1,
		.hour = 0,
		.min  = 0,
		.sec  = 0
	};
	RTC_Init();
	RTC_setTime( &ctRTC );
}

static void dataLog_Status_Update (void)
{
	regEeprom.add_lastReg= regEeprom.add_nextReg;
	regEeprom.add_lastTime= regEeprom.add_nextTime;

	regEeprom.add_nextReg += SIZE_BYTES_REG_LOG;
	if( EEPROM_ADD_REG_MAX <= regEeprom.add_nextReg )
	{
		regEeprom.add_nextReg = EEPROM_ADD_INIT_REG;
	}

	regEeprom.add_nextTime += SIZE_BYTES_REG_TIME;
	if( EEPROM_ADD_TIME_MAX <= regEeprom.add_nextTime )
	{
		regEeprom.add_nextTime= EEPROM_ADD_TIME_MAX;
	}

	(regEeprom.add_num_register)++;
}

static int dataLog_Status_Store (void)
{
	int retval=0;
	int i;
	uint32_t * ptrReg= (uint32_t*) &regEeprom;
	uint16_t  addrMem= regEeprom.add_otherReg;

	for (i= 0; i < SIZE_WORDS_STRUCT_CTR; ++i)
	{
		if( !EEPROM_writeWord( (uint32_t) addrMem, ptrReg[i] ) )
		{
			break;
		}
		addrMem += SIZE_BYTES_IN_WORD;
	}
	if( SIZE_WORDS_STRUCT_CTR == i )
	{
		retval= 1;
	}

	return retval;
}

static int dataLog_Time_Store ( liteTime_t * regLog )
{
	int retval=0;
	uint32_t* ptrRegTime= (uint32_t*) regLog;
	uint16_t  addrMem= regEeprom.add_nextTime;

	if( EEPROM_writeWord( (uint32_t) addrMem, *ptrRegTime ) )
	{
		retval= 1;
	}

	return retval;
}

static int dataLog_Time_Load (liteTime_t * regLog)
{
	int retval=0;
	uint32_t* ptrRegTime= (uint32_t*) regLog;
	uint16_t  addrMem    = regEeprom.add_lastTime;

	if( EEPROM_readWord( (uint32_t) addrMem, ptrRegTime ) )
	{
		retval= 1;
	}

	return retval;
}
//******************************************************************************************++
int dataLog_Init (void)
{
	// TODO agregar una operacion de lectura sobre la memoria para levantar estos registros.
	regEeprom.add_otherReg= (uint16_t) EEPROM_OTHER_REGIST;
	regEeprom.add_num_register= 0;

	regEeprom.add_nextTime= (uint16_t) EEPROM_ADD_INIT_TIME;
	regEeprom.add_lastTime= (uint16_t) EEPROM_ADD_INIT_TIME;

	regEeprom.add_nextReg= (uint16_t) EEPROM_ADD_INIT_REG;
	regEeprom.add_lastReg= (uint16_t) EEPROM_ADD_INIT_REG;

	dataLog_RTC_Setup();
	// Modo: EEPROM_AUTOP_1WORDWRITTEN sirve mas para escribir de a words/bytes
	return (EEPROM_init( EEPROM_AUTOPROG_AFT_LASTWORDWRITTEN));
}


int dataLog_Page_Store (dlogMem_t* dataToLog)
{
	uint32_t retval=0;

	sendEeprom.regMem= dataToLog->dataReg;

	//dataLog_RTC_Update( sendEeprom.regTime );
	RTC_getTime( &dlRTC );
	sendEeprom.regTime.hour=dataToLog->hourReg;
	sendEeprom.regTime.min= dataToLog->minuReg;
	sendEeprom.regTime.wday= dlRTC.wday;
	sendEeprom.regTime.token= TOKEN_REG_TIME;


	if( dataToLog->dataReg && (EEPROM_PAGE_SIZE == dataToLog->sizeReg) )
	{
		if( EEPROM_writePage( (uint32_t) regEeprom.add_nextReg, sendEeprom.regMem ) )
		{
			if( dataLog_Time_Store( &sendEeprom.regTime ) ) // FIXME ahora lo hago asi porque sino tengo que esperar a que grabe el log primero.
			{
				if( dataLog_Status_Store() )
				{
					dataLog_Status_Update();
					retval= 1;
				}
			}
		}
	}
	return retval;
}

int dataLog_Page_Load (dlogMem_t* dataToLog)
{
	uint32_t retval=0;

	readEeprom.regMem= dataToLog->dataReg;

	if( dataToLog->dataReg && (EEPROM_PAGE_SIZE == dataToLog->sizeReg ))
	{
		if( EEPROM_readPage( (uint32_t) regEeprom.add_lastReg, readEeprom.regMem ) )
		{
			if( dataLog_Time_Load( &readEeprom.regTime) )
			{
				dataToLog->hourReg= readEeprom.regTime.hour;
				dataToLog->minuReg= readEeprom.regTime.min;
				retval= 1;
			}
		}
	}
	return retval;
}
