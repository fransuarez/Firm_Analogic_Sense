/*
 ============================================================================
 Name        : temperatura.c
 Author      : Francisco
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binaryFiles.h"
#include "api_EEPROM.h"


#define  RET_ERROR   		( 0 )
#define  RET_OK      		( 1 )
#define  RET_FULL   		( 2 )
#define  RET_TIME      		( 3 )

#define  SIZE_BUFF_MEM 	 	( EEPROM_PAGE_CHAR_SIZE )
#define  SIZE_BUFF_FILES 	( 64 )
#define  SIZE_BYTES_DPROC 	( 4 )

typedef struct dataprocess
{
	uint8_t 	name;
	uint8_t		secTime;
	uint16_t 	value; 	// Esto despues se puede optimizar y dejar solo 4 char/bytes que representen un uint32
} dproc_t;

static int test1 			( void );
static int processBuffer 	( uint8_t *, uint8_t *  );
static int processData		( void  );

static uint8_t mem_read_buffer[SIZE_BUFF_MEM];
static uint8_t mem_write_buffer[SIZE_BUFF_MEM];

static dlogMem_t data_write= { .dataReg= mem_write_buffer, .sizeReg= SIZE_BUFF_MEM };
static dlogMem_t data_read = { .dataReg= mem_read_buffer , .sizeReg= SIZE_BUFF_MEM };;


//****************************************************************************
static int processBuffer ( uint8_t * buffer_in, uint8_t * buffer_out )
{
	char * pTemp, *pVol;
	dproc_t data;
	int retval= RET_ERROR;
	static int count=0;

	if( NULL != ( pTemp = strchr( (char *) buffer_in, 'T' ) ) )
	{
		if( NULL != ( pVol = strchr( pTemp, '\0' ) ) )
		{
			data_write.hourReg= (uint8_t) atoi( ++pTemp );
			pTemp += 4;
			data_write.minuReg= (uint8_t) atoi( pTemp );
			retval= RET_TIME;
		}
	}

	else if(  NULL != ( pTemp = strchr( (char *) buffer_in, '@' ) ))
	{
		if( NULL != ( pVol = strchr( pTemp, '\0' ) ) )
		{
			data.name = (uint8_t) atoi( ++pTemp );

			pTemp += 3;
			data.secTime = (uint8_t)atoi( pTemp );

			pTemp += 4;
			data.value = (uint16_t) atoi( pTemp );

			memcpy( buffer_out, &data, SIZE_BYTES_DPROC );

			if( SIZE_WORDS_REG_LOG == ++count )
			{
				retval= RET_FULL;
				count= 0;
			}
			else
			{
				retval= RET_OK;
			}
		}
	}

	return retval;
}

static int	processData( void )
{
	int retval= RET_ERROR;

	if( dataLog_Page_Store( &data_write ) )
	{
		retval= RET_OK;
	}

	return retval;
}

//***************************************************************************************+
static int test1 (void)
{
	int sizeMsg, retproc, retval= RET_ERROR;
	FILE * fd_data_input;
	FILE * fd_data_output;
	char data_inp_buffer[SIZE_BUFF_FILES];
	char data_out_buffer[SIZE_BUFF_FILES];
	char * retaux;
	uint8_t * ptrBuffWrite;

	fd_data_input = fopen ( "read_registers.txt" , "r+" );
	if( NULL ==  fd_data_input )
	{
		printf( "\r\nNo se pudo abrir el archivo de entradas.\n" );
		return 0;
	}
	fd_data_output = fopen ( "write_registers.txt" , "w+" );
	if( NULL ==  fd_data_output )
	{
		printf( "\r\nNo se pudo abrir el archivo de salida.\n" );
		return 0;
	}

	if( !dataLog_Init() )
	{
		return 0;
	}
	ptrBuffWrite= mem_write_buffer;
	// PROCESOS DE ESCRITURA
	while( !feof( fd_data_input ) )
	{
		retaux= fgets( data_inp_buffer, SIZE_BUFF_FILES, fd_data_input );
		if( NULL != retaux )
		{
			sizeMsg= strlen( data_inp_buffer );
			data_inp_buffer [sizeMsg-1] = '\0';

			retproc= processBuffer( (uint8_t *) data_inp_buffer, ptrBuffWrite );
			if( RET_FULL ==  retproc )
			{
				sizeMsg= processData();
				if( sizeMsg )
				{
					retval= RET_OK;
				}
			}
			if( RET_OK == retproc )
			{
				ptrBuffWrite+= SIZE_BYTES_IN_WORD;
			}
		}
		else
		{
			break;
		}
	}


	// PROCESOS DE LECTURA
	int i;
	uint8_t * ptr;
	dproc_t data;

	if( dataLog_Page_Load( &data_read ) )
	{
		if( 0 < sprintf( data_out_buffer, "T %2d:%2d\n\n", data_read.hourReg, data_read.minuReg) )
		{
			fputs( data_out_buffer, fd_data_output );
			retval= RET_OK;
		}

		ptr= data_read.dataReg;
		for (i = 0; i < SIZE_WORDS_REG_LOG; ++i)
		{
			memcpy( &data, ptr, SIZE_BYTES_IN_WORD );
			if( 0 < sprintf( data_out_buffer, "@%2d %2d %2d \n", data.name, data.secTime, data.value) )
			{
				fputs( data_out_buffer, fd_data_output );
			}
			ptr+= SIZE_BYTES_IN_WORD;
		}
	}

//	printf( "Temperatura medida: %i\n", tempAmbiente );
//	printf( "Voltaje medido: %f\n", voltajeMedido );
//	tempaux= tcuplaKVoltToTemp( voltajeMedido, tempAmbiente );

	fclose ( fd_data_input );
	fclose ( fd_data_output );
	EEPROM_endTask_Mock();

	return retval;
}

//***************************************************************************************+

int main (void)
{

	if( test1() )
	{
		puts("Test 1 OK");
	}
	else
	{
		puts("Fallo test 1"); /* prints Hello World */
	}

	//while(1);
	return EXIT_SUCCESS;
}

