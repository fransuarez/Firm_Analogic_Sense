/*
 * binaryFiles.c
 *
 *  Created on: Nov 28, 2018
 *      Author: fran
 */
#include "binaryFiles.h"
#include "api_RTC.h"

//**********************************************************************************************
#define ID_CHECK_VALID(X)	( EEPROM_REG_MAX_IDS >= X )
#define ID_GET_ADD(X) 		( regEeprom.add_firstReg + X*SIZE_BYTES_REG_LOG )
#define REG_CHECK_ADD(X)	( (EEPROM_ADD_INIT_TIME <= X) && (EEPROM_ADD_TIME_MAX >= X) )
#define TIME_CHECK_ADD(X)	( (EEPROM_ADD_INIT_REG <= X) && (EEPROM_ADD_REG_MAX >= X) )

typedef struct _control_eeprom
{
	uint16_t add_num_register; 	// Maximo preestablecido 112
	uint16_t add_lastTime; 		// Longitud de "HH:MM:SS"
	uint16_t add_nextTime;
	uint16_t add_firstReg;		// Primer registro
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
		.mday = 4,
		.month= 12,
		.year = 2018,
		.wday = 3,
		.hour = 18,
		.min  = 43,
		.sec  = 55
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
	if( EEPROM_REG_MAX_IDS<= regEeprom.add_num_register )
	{
		regEeprom.add_num_register= EEPROM_REG_MAX_IDS;

		regEeprom.add_firstReg += SIZE_BYTES_REG_LOG;
		if( EEPROM_ADD_REG_MAX <= regEeprom.add_firstReg )
		{
			regEeprom.add_firstReg = EEPROM_ADD_INIT_REG;
		}
	}
}

static int dataLog_Status_Store (void)
{
	int retval=0;
	int i;
	uint32_t * ptrReg= (uint32_t*) &regEeprom;
	uint16_t  addrMem= (uint16_t) EEPROM_OTHER_REGIST;

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

static int dataLog_Status_Load (CtrlE2P_t * ptrRet)
{
	int retval=0;
	int i;
	uint32_t * ptrReg= (uint32_t*) &ptrRet;
	uint16_t  addrMem= (uint16_t) EEPROM_OTHER_REGIST;

	for (i= 0; i < SIZE_WORDS_STRUCT_CTR; ++i)
	{
		if( !EEPROM_readWord( (uint32_t) addrMem, &ptrReg[i] ) )
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
	int retval;
	CtrlE2P_t regAux;

	dataLog_RTC_Setup();
	// Modo: EEPROM_AUTOP_1WORDWRITTEN sirve mas para escribir de a words/bytes
	retval= EEPROM_init( EEPROM_AUTOPROG_OFF ); //  EEPROM_AUTOPROG_AFT_LASTWORDWRITTEN

	if( dataLog_Status_Load( &regAux ) )
	//if( 0 )
	{
		if( REG_CHECK_ADD( regAux.add_lastReg) )
		{
			regEeprom.add_num_register= regAux.add_num_register;
			regEeprom.add_lastTime= regAux.add_lastTime;
			regEeprom.add_nextTime= regAux.add_nextTime;
			regEeprom.add_firstReg= regAux.add_firstReg;
			regEeprom.add_lastReg= regAux.add_lastReg;
			regEeprom.add_nextReg= regAux.add_nextReg;
		}
		else
		{
			regEeprom.add_num_register= 0;
			regEeprom.add_lastTime= (uint16_t) EEPROM_ADD_INIT_TIME;
			regEeprom.add_nextTime= (uint16_t) EEPROM_ADD_INIT_TIME;
			regEeprom.add_firstReg= (uint16_t) EEPROM_ADD_INIT_REG;
			regEeprom.add_lastReg= (uint16_t) EEPROM_ADD_INIT_REG;
			regEeprom.add_nextReg= (uint16_t) EEPROM_ADD_INIT_REG;
		}
	}
	else
	{
		regEeprom.add_num_register= 0;
		regEeprom.add_lastTime= (uint16_t) EEPROM_ADD_INIT_TIME;
		regEeprom.add_nextTime= (uint16_t) EEPROM_ADD_INIT_TIME;
		regEeprom.add_firstReg= (uint16_t) EEPROM_ADD_INIT_REG;
		regEeprom.add_lastReg= (uint16_t) EEPROM_ADD_INIT_REG;
		regEeprom.add_nextReg= (uint16_t) EEPROM_ADD_INIT_REG;
	}
	return retval;
}

int dataLog_GetStatus (uint16_t* retCount)
{
	int retval=0;

	if( retCount )
	{
		*retCount= regEeprom.add_num_register;
		retval= 1;
	}

	return retval;
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

int dataLog_Page_Load (dlogMem_t* readData, uint16_t id)
{
	uint32_t retval=0;
	uint32_t addAux;

	readEeprom.regMem= readData->dataReg;

	if( ID_CHECK_VALID(id) && (EEPROM_PAGE_SIZE == readData->sizeReg ) )
	{
		addAux= ID_GET_ADD(id);
		if( EEPROM_ADD_REG_MAX <= addAux )
		{
			addAux-= EEPROM_ADD_REG_MAX-EEPROM_ADD_INIT_REG;
		}

		if( EEPROM_readPage( addAux, readEeprom.regMem ) )
		{
			if( dataLog_Time_Load( &readEeprom.regTime) )
			{
				readData->hourReg= readEeprom.regTime.hour;
				readData->minuReg= readEeprom.regTime.min;
				retval= 1;
			}
		}
	}

	return retval;
}
