/*
 * ciaaE2P.c
 *
 *  Created on: 19/7/2018
 *      Author: fran
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "api_EEPROM.h"

/*
 * Funciones extraidas del repositorio de Ernesto Gigliotti:
 * 	github.com/ernesto-g/micropython/blob/cese/ciaa-nxp/board_ciaa_edu_4337/src/board.c
 */
#define EEPROM_PAGE_SIZE	EEPROM_PAGE_CHAR_SIZE
#define EEPROM_PAGE_NUM 	EEPROM_PAGE_PAG_NUM
#define SIZE_BUFF 			EEPROM_PAGE_CHAR_SIZE

#define VALID_ADDRESS(X) 	( 0x0000 <= X && (EEPROM_PAGE_SIZE*(EEPROM_PAGE_NUM-1)) > X)
#define ALINGNED_ADDRESS(X) ( 0 == (0x03 & X))
#define ALINGNED_PAGE(X) 	( 0 == (0x7F & X))
#define PAGE_INIT_ADDRESS	( 7 ) // Equivalente a sqrt(EEPROM_PAGE_SIZE)  esto mejora la perfomance del algoritmo
//================================[EEPROM Management]=========================================

FILE * fd_memmory_object;

char data_inp_buffer[SIZE_BUFF];
char data_out_buffer[SIZE_BUFF];


int EEPROM_init (uint8_t mode)
{
	int retval=0;

	fd_memmory_object = fopen ( "eeprom_emulate.bin", "wb+" );
	if( NULL !=  fd_memmory_object )
	{
		retval= 1;
	}
	return retval;
}

int EEPROM_writeByte (uint32_t addr, uint8_t value)
{
	int retval= 0;
	int retaux;
	long int actual_address;

	if( VALID_ADDRESS(addr) )
	{
		//ftell ( fd_memmory_object );
		//uint32_t pageAddr = addr>>PAGE_INIT_ADDRESS;
		//uint32_t pageOffset = addr - (pageAddr<<PAGE_INIT_ADDRESS);

		actual_address= (long int) addr;

		if( !fseek ( fd_memmory_object, actual_address, SEEK_SET ) )
		{
			//retaux= fputc ( value, fd_memmory_object );
			retaux= fwrite ( &value, 1, 1, fd_memmory_object );

			if( (EOF != retaux) &&  (1 == retaux) )
			{
				retval= 1;
			}
		}
	}
	return retval;
}

int EEPROM_readByte (uint32_t addr, uint8_t* value)
{
	int retval= 0;
	int retaux;
	long int actual_address;

	if( VALID_ADDRESS(addr) && value )
	{
		//uint32_t pageAddr = addr>>PAGE_INIT_ADDRESS;
		//uint32_t pageOffset = addr - (pageAddr<<PAGE_INIT_ADDRESS);

		actual_address= (long int) addr;

		if( !fseek ( fd_memmory_object, actual_address, SEEK_SET ) )
		{
			//retaux= fgetc ( fd_memmory_object );
			retaux=  fread ( value, 1, 1, fd_memmory_object );

			if( EOF != retaux )
			{
				//*value= retaux;
				retval= 1;
			}
		}
	}
	return retval;
}

int EEPROM_writeWord (uint32_t addr4, uint32_t value)
{
	int retval= 0;
	int retaux;
	//int i;
	long int actual_address;

	if( VALID_ADDRESS(addr4) && ALINGNED_ADDRESS(addr4) )
	{
		//ftell ( fd_memmory_object );
		//uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		//uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);

		actual_address= (long int) addr4;

		if( !fseek ( fd_memmory_object, actual_address, SEEK_SET ) )
		{
			//for (i = 0; i < 4; ++i)
			{
				//retaux= fputc ( (char)(value>>(i*8)), fd_memmory_object );
				retaux= fwrite ( &value, 4, 1, fd_memmory_object );

				if( EOF != retaux )
				{
					retval= 1;
				}
			}

		}
	}
	return retval;
}

int EEPROM_readWord (uint32_t addr4, uint32_t* value)
{
	int retval= 0;
	int retaux;
	//int i;
	long int actual_address;

	if( VALID_ADDRESS(addr4) && ALINGNED_ADDRESS(addr4) && value )
	{
		//uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		//uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);

		actual_address= (long int) addr4;

		if( !fseek ( fd_memmory_object, actual_address, SEEK_SET ) )
		{
//			for (i = 0; i < 4; ++i)
			{
				//retaux= fgetc ( fd_memmory_object );
				retaux=  fread ( value, 1, 4, fd_memmory_object );

				if( EOF != retaux )
				{
					//*value &= (char)(retaux<<(i*8));
					retval= 1;
				}
			}

		}
	}
	return retval;
}

int EEPROM_writePage (uint32_t addr4, uint8_t* buffWrite)
{
	int retval= 0;
	int retaux;
	long int actual_address;

	if( VALID_ADDRESS(addr4) && ALINGNED_PAGE(addr4) )
	{
		//uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		//uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);

		actual_address= (long int) addr4;

		if( !fseek ( fd_memmory_object, actual_address, SEEK_SET ) )
		{
			retaux= fwrite ( buffWrite, 1, EEPROM_PAGE_SIZE, fd_memmory_object );
			if( EOF != retaux )
			{
				retval= 1;
			}
		}
	}
	return retval;
}

int EEPROM_readPage (uint32_t addr4, uint8_t* readBuff)
{
	int retval= 0;
	int retaux;
	long int actual_address;

	if( VALID_ADDRESS(addr4) && ALINGNED_PAGE(addr4) && readBuff )
	{
		//uint32_t pageAddr = addr4>>PAGE_INIT_ADDRESS;
		//uint32_t pageOffset = addr4 - (pageAddr<<PAGE_INIT_ADDRESS);

		actual_address= (long int) addr4;

		if( !fseek ( fd_memmory_object, actual_address, SEEK_SET ) )
		{
			retaux=  fread ( readBuff, 1, EEPROM_PAGE_SIZE, fd_memmory_object );

			if( retaux )
			{
				retval= 1;
			}
		}
	}
	return retval;
}

void EEPROM_endTask_Mock (void)
{
	fclose ( fd_memmory_object );
}

void EEPROM_writeEnd_ISR (void)
{

}

//===========================================================================================================================

