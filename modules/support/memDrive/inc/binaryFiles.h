/*
 * binaryFiles.h
 *
 *  Created on: Nov 28, 2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_MEMDRIVE_INC_BINARYFILES_H_
#define MODULES_SUPPORT_MEMDRIVE_INC_BINARYFILES_H_

#include "api_EEPROM.h"

#define EEPROM_ABS_ADDRESS	 ( EEPROM_ADDRESS_LOW )
#define EEPROM_OTHER_REGIST	 ( EEPROM_ABS_ADDRESS )	// 8 PAGINAS LIBRES, 0x0000 - 0x03FF
#define EEPROM_ADD_INIT_TIME ( EEPROM_OTHER_REGIST + (8*EEPROM_PAGE_CHAR_SIZE) ) // 7 paginas con dia de semana+hora, 0x0400-0x077F
#define EEPROM_ADD_INIT_REG  ( EEPROM_ADD_INIT_TIME + (7*EEPROM_PAGE_CHAR_SIZE) ) // 112 pag de informacion, 0x0780 - 0x3F7F
#define EEPROM_ADD_RESERVED  ( EEPROM_ADD_INIT_REG + (112*EEPROM_PAGE_CHAR_SIZE))// Zona reservada, 0x3F80 - 0x3FFF

#define EEPROM_ADD_TIME_MAX	( EEPROM_ADD_INIT_REG )
#define EEPROM_ADD_REG_MAX	( EEPROM_ADD_RESERVED )
#define EEPROM_REG_MAX_IDS	( (EEPROM_ADD_REG_MAX-EEPROM_ADD_INIT_REG)/SIZE_WORDS_REG_LOG )

#define SIZE_WORDS_REG_TIME		( 1 )
#define SIZE_BYTES_IN_WORD		( 4 )
#define SIZE_BYTES_REG_TIME		( 4 )
#define SIZE_WORDS_STRUCT_CTR	( 3 )

#define SIZE_BYTES_REG_LOG		( EEPROM_PAGE_SIZE )
#define SIZE_WORDS_REG_LOG		( 32 )

#define TOKEN_REG_TIME			( 0xFF )

//EEPROM_ADDRESS_HIGH

//#define  TEST_MEMORIA

#ifndef TEST_MEMORIA
#include "lpc_types.h"
#else

#include "api_TYPES.h"
#define EEPROM_PAGE_SIZE	 128
#define EEPROM_AUTOPROG_AFT_LASTWORDWRITTEN	 1

#endif

typedef struct _data_log_eeprom
{
	uint8_t		sizeReg;
	uint8_t		hourReg;
	uint8_t		minuReg;
	uint8_t	 	dummy;
	uint8_t* 	dataReg;
	//TickType_t ticktimes;
} dlogMem_t;

int dataLog_Init (void);
int dataLog_Page_Store (dlogMem_t* loadData);
int dataLog_Page_Load (dlogMem_t* readData, uint16_t idReg);
int dataLog_GetStatus (uint16_t* retCount);

#endif /* MODULES_SUPPORT_MEMDRIVE_INC_BINARYFILES_H_ */
