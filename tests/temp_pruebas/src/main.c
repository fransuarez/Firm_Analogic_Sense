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

#include "temperaturaMath.h"
#define  RET_ERROR   ( 0 )
#define  RET_OK      ( 1 )

typedef struct dataprocess
{
	// Datos de entrada:
	temp_t tempAmb;
	volt_t voltTcupla;
	// Datos de salida:
	temp_t tempCalc;
	volt_t voltCalc;
	volt_t voltError;
} dproc_t;

static int test1 			( volt_t , temp_t );
static int processBuffer 	( dproc_t *, char * , int  );
static int processData		( dproc_t *, char *  );


int main (void)
{

	if( test1( (volt_t) 2.602 , (temp_t) 25) )
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

#define SIZE_BUFF 		100
static int test1 (volt_t voltajeMedido, temp_t tempAmbiente)
{
	int sizeMsg, retval= RET_ERROR;
	FILE * fd_data_input;
	FILE * fd_data_output;
	dproc_t data;
	char data_inp_buffer[SIZE_BUFF];
	char data_out_buffer[SIZE_BUFF];

	fd_data_input = fopen ( "sim_voltage_tcupla.svc", "r+" );
	if( NULL ==  fd_data_input )
	{
		printf( "\r\nNo se pudo abrir sim_voltage_tcupla.svc\n" );
		return 0;
	}
	else
	{
		fd_data_output = fopen ( "process_tem_tcupla.txt", "w+" );
		if( NULL ==  fd_data_output )
		{
			printf( "\r\nNo se pudo abrir process_tem_tcupla.txt\n" );
			return 0;
		}
		else
		{
	}
				while( !feof( fd_data_input ) )
	{
		if( NULL != fgets( data_inp_buffer, SIZE_BUFF, fd_data_input ) )
		{
			sizeMsg= strlen( data_inp_buffer );
			data_inp_buffer [sizeMsg-1] = '\0';

			if( ERROR_TEMP != processBuffer( &data, data_inp_buffer, sizeMsg ) )
			{
				sizeMsg= processData( &data, data_out_buffer );
				if( sizeMsg )
				{
					fputs( data_out_buffer, fd_data_output );
					retval= RET_OK;
				}
			}
		}
		else
		{
			break;
		}
	}
		}

//	printf( "Temperatura medida: %i\n", tempAmbiente );
//	printf( "Voltaje medido: %f\n", voltajeMedido );
//	tempaux= tcuplaKVoltToTemp( voltajeMedido, tempAmbiente );


	fclose ( fd_data_input );
	fclose ( fd_data_output );

	return retval;
}


static int processBuffer ( dproc_t *data, char * buffer, int sizeBuff )
{
	char * pch;
	volt_t voltaux;
	temp_t tempaux;
	int retval= RET_ERROR;

	if( NULL != ( pch = strtok( buffer, "@V" ) ) )
	{
		data->tempAmb = atoi(pch);

		if( NULL != ( pch = strtok( NULL, "@V" ) ) )
		{
			data->voltTcupla = atof(pch+1);

			tempaux= tcuplaKVoltToTemp( data->voltTcupla, data->tempAmb );
			if( ERROR_TEMP != tempaux )
			{
				data->tempCalc= tempaux;

				voltaux= tcuplaKTempToVolt( tempaux, data->tempAmb );
				if( ERROR_VOLT != voltaux )
				{
					data->voltCalc= voltaux;
					data->voltError= abs(voltaux - data->voltTcupla);
					retval= RET_OK;
				}
			}
		}
	}
	return retval;
}

static int	processData( dproc_t *data, char * buffer )
{
	if( 0 < sprintf( buffer, "\n\r@%d°C \t%fmV >> %d°C, %fmV @%fmV E", data->tempAmb, data->voltTcupla,
										   data->tempCalc, data->voltCalc, data->voltError) )
	{
		return strlen( buffer );
	}
	return 0;
}

