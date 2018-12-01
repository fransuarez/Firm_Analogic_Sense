/*
 * ciaaE2P.c
 *
 *  Created on: 19/7/2018
 *      Author: fran
 */

#include "chip.h"
#include <string.h>

/*
 * Funciones extraidas del repositorio de Ernesto Gigliotti:
 * 	github.com/ernesto-g/micropython/blob/cese/ciaa-nxp/board_ciaa_edu_4337/src/board.c
 */
#define VALID_ADDRESS(X) 	( 0x0000 <= X && (EEPROM_PAGE_SIZE*(EEPROM_PAGE_NUM-1)) > X)
#define ALINGNED_ADDRESS(X) ( 0 == (0x03 & X))
#define ALINGNED_PAGE(X) 	( 0 == (0x7F & X))
#define PAGE_INIT_ADDRESS	( 7 ) // Equivalente a sqrt(EEPROM_PAGE_SIZE)  esto mejora la perfomance del algoritmo
//================================[EEPROM Management]=========================================

int EEPROM_init (uint8_t mode)
{
	Chip_EEPROM_Init( LPC_EEPROM );
  	/* Set Auto Programming mode: write reg word then programing */
 	Chip_EEPROM_SetAutoProg( LPC_EEPROM, mode );

 	if( EEPROM_AUTOPROG_AFT_LASTWORDWRITTEN == mode )
 	{
 		Chip_EEPROM_EnableInt( LPC_EEPROM, EEPROM_INT_ENDOFPROG );
 	}
 	return 1;
}

int EEPROM_writeByte (uint32_t addr, uint8_t value)
{
	int retval= 0;

	if( VALID_ADDRESS(addr) )
	{
		uint32_t pageAddr = addr>>PAGE_INIT_ADDRESS;
		uint32_t pageOffset = addr - (pageAddr<<PAGE_INIT_ADDRESS);

	 	//Chip_EEPROM_SetAutoProg( LPC_EEPROM, EEPROM_AUTOPROG_AFT_1WORDWRITTEN );

		uint32_t *pEepromMem = (uint32_t*) EEPROM_ADDRESS( pageAddr, pageOffset );

		// read 4 bytes in auxValue
		uint32_t auxValue = pEepromMem[0];
		uint8_t* pAuxValue = (uint8_t*) &auxValue;

		// modify auxValue with new Byte value
		pAuxValue[addr % 4] = value;

		//write auxValue back in eeprom
		pEepromMem[0] = auxValue;
		Chip_EEPROM_SetCmd( LPC_EEPROM, EEPROM_CMD_ERASE_PRG_PAGE );
		Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );

		retval= 1;
	}
	return retval;
}

int EEPROM_readByte (uint32_t addr, uint8_t* value)
{
	int retval= 0;

	if( VALID_ADDRESS(addr) && value )
	{
		uint32_t pageAddr = addr>>PAGE_INIT_ADDRESS;
		uint32_t pageOffset = addr - (pageAddr<<PAGE_INIT_ADDRESS);

		// read 4 bytes in pAuxValue
		uint8_t* pAuxValue = (uint8_t*) EEPROM_ADDRESS( pageAddr, pageOffset);

		// return auxValue in position of new Byte value
		*value= pAuxValue[addr % 4];
		Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );

		retval=1;
	}
	return retval;
}

int EEPROM_writeWord (uint32_t addr4, uint32_t value)
{
	int retval= 0;

	if( VALID_ADDRESS(addr4) && ALINGNED_ADDRESS(addr4) )
	{
		uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);

	 	//Chip_EEPROM_SetAutoProg( LPC_EEPROM, EEPROM_AUTOPROG_AFT_1WORDWRITTEN );

		uint32_t *pEepromMem = (uint32_t*) EEPROM_ADDRESS( pageAddr, pageOffset );

		pEepromMem[0] = value;
		// write 4 bytes or 1 word value in eeprom:
		Chip_EEPROM_SetCmd( LPC_EEPROM, EEPROM_CMD_ERASE_PRG_PAGE );
		Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );

		retval=1;
	}
	return retval;
}

int EEPROM_readWord (uint32_t addr4, uint32_t* value)
{
	int retval= 0;

	if( VALID_ADDRESS(addr4) && ALINGNED_ADDRESS(addr4) && value )
	{
		uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);

		// read 4 bytes in auxValue
		*value = *((uint32_t*) EEPROM_ADDRESS( pageAddr,pageOffset ));
		Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );

		retval=1;
	}
	return retval;
}

int EEPROM_writePage (uint32_t addr4, uint8_t* buffWrite)
{
	int retval= 0;

	if( VALID_ADDRESS(addr4) && ALINGNED_PAGE(addr4) )
	{
		uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);

	 	Chip_EEPROM_SetAutoProg( LPC_EEPROM, EEPROM_AUTOPROG_AFT_LASTWORDWRITTEN );

		uint32_t *pEepromMem = (uint32_t*) EEPROM_ADDRESS( pageAddr, pageOffset );

		// write 4 bytes or 1 word value in eeprom:
		//Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );
		//if( !(EEPROM_INT_ENDOFPROG & Chip_EEPROM_GetIntStatus( LPC_EEPROM )) )
		{
			//pEepromMem= (uint32_t*) buffWrite;
			memcpy( pEepromMem, buffWrite, EEPROM_PAGE_SIZE );
			//Chip_EEPROM_SetCmd(LPC_EEPROM, EEPROM_CMD_ERASE_PRG_PAGE);
			Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );

			retval=1;
		}
	}
	return retval;
}

int EEPROM_readPage (uint32_t addr4, uint8_t* readBuff)
{
	int retval= 0;

	if( VALID_ADDRESS(addr4) && ALINGNED_PAGE(addr4) && readBuff )
	{
		uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);
		uint8_t *pEepromMem = (uint8_t*) EEPROM_ADDRESS( pageAddr, pageOffset );

		// read 4 bytes in auxValue
		//Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );
		//if( EEPROM_INT_ENDOFPROG == (EEPROM_INT_ENDOFPROG & Chip_EEPROM_GetIntStatus( LPC_EEPROM )))
		{
			memcpy( readBuff, pEepromMem, EEPROM_PAGE_SIZE );

			Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );

			retval=1;
		}
	}
	return retval;
}

void EEPROM_writeEnd_ISR (void)
{
	if( EEPROM_INT_ENDOFPROG == (EEPROM_INT_ENDOFPROG & Chip_EEPROM_GetIntStatus( LPC_EEPROM )))
	{
		Chip_EEPROM_ClearIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG);
	}
}

//===========================================================================================================================

