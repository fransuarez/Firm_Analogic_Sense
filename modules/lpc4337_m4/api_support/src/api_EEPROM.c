/*
 * ciaaE2P.c
 *
 *  Created on: 19/7/2018
 *      Author: fran
 */

#include "chip.h"
#include "lpc_types.h"
/*
 * Funciones extraidas del repositorio de Ernesto Gigliotti:
 * 	github.com/ernesto-g/micropython/blob/cese/ciaa-nxp/board_ciaa_edu_4337/src/board.c
 */

//================================[EEPROM Management]=========================================

void EEPROM_init (void)
{
	Chip_EEPROM_Init( LPC_EEPROM );
  	/* Set Auto Programming mode */
 	Chip_EEPROM_SetAutoProg( LPC_EEPROM, EEPROM_AUTOPROG_AFT_1WORDWRITTEN );
}

void EEPROM_writeByte (uint32_t addr, uint8_t value)
{
	uint32_t addr4 = addr/4;
	uint32_t pageAddr = addr4/EEPROM_PAGE_SIZE;
	uint32_t pageOffset = addr4 - pageAddr*EEPROM_PAGE_SIZE;

	uint32_t *pEepromMem = (uint32_t*)EEPROM_ADDRESS( pageAddr,pageOffset*4 );

	// read 4 bytes in auxValue
	uint32_t auxValue = pEepromMem[0];
	uint8_t* pAuxValue = (uint8_t*)&auxValue;

	// modify auxValue with new Byte value
	uint32_t indexInBlock = addr % 4;
	pAuxValue[indexInBlock] = value;

	//write auxValue back in eeprom
	pEepromMem[0] = auxValue;
	Chip_EEPROM_WaitForIntStatus( LPC_EEPROM, EEPROM_INT_ENDOFPROG );
}

uint8_t EEPROM_readByte (uint32_t addr)
{
	uint32_t addr4 = addr/4;
	uint32_t pageAddr = addr4/EEPROM_PAGE_SIZE;
	uint32_t pageOffset = addr4 - pageAddr*EEPROM_PAGE_SIZE;

	uint32_t *pEepromMem = (uint32_t*) EEPROM_ADDRESS( pageAddr,pageOffset*4 );

	// read 4 bytes in auxValue
	uint32_t auxValue = pEepromMem[0];
	uint8_t* pAuxValue = (uint8_t*) &auxValue;

	// modify auxValue with new Byte value
	uint32_t indexInBlock = addr % 4;
	return pAuxValue[indexInBlock];

}

//===========================================================================================================================

