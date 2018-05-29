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
#include <math.h>

#include "temperaturaMath.h"
#define  RET_ERROR   ( 0 )
#define  RET_VOID    ( 1 )
#define  RET_OK      ( 2 )
#define  SIZE_BUFF 	 100


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

typedef struct datosNtc
{
	// Datos de entrada:
	double resisMedida;
	// Datos de salida:
	double tempCalc;
	double resisCalc;
	double resisError;

} dntc_t;


static int test1 				( void );
static int test2 				( void );
static int processBuffer 		( dproc_t *, char * , int  );
static int processBufferNtc 	( dntc_t *, char * , int );

static int processDataOk		( dproc_t *, char * );
static int processDataError		( dproc_t *, char * );
static int processNtcDataOk		( dntc_t * , char * );
static int processNtcDataError	( dntc_t * , char * );


int main (void) 
{
/*
	if( test1() )
	{
		puts("Test 1 OK"); 
	}
	else
	{
		puts("Fallo test 1");
	}*/

	if( test2() )
	{
		puts("Test 2 OK");
	}
	else
	{
		puts("Fallo test 2");
	}
	//while(1);		
	return EXIT_SUCCESS;
}

static int test1 ( void )
{
	int sizeMsg, retaux, retval= RET_ERROR;
	FILE * fd_data_input;
	FILE * fd_data_output;
	dproc_t data;
	char data_inp_buffer[SIZE_BUFF];
	char data_out_buffer[SIZE_BUFF];
	
	fd_data_input = fopen ( "sim_voltage_tcupla.svc", "r+" );
	if( NULL !=  fd_data_input )
	{
		fd_data_output = fopen ( "process_tem_tcupla.txt", "w+" );
		if( NULL !=  fd_data_output )
		{
			fputs( "T_ref \t V_med \t\tT_cal \t V_cal \t\t V_error \n"
					"--------------------------------------------------\n", fd_data_output );
			while( !feof( fd_data_input ) )
			{
				if( NULL != fgets( data_inp_buffer, SIZE_BUFF, fd_data_input ) )
				{
					sizeMsg= strlen( data_inp_buffer );
					//data_inp_buffer [sizeMsg-1] = '\0';
					retaux= processBuffer( &data, data_inp_buffer, sizeMsg );
					if( RET_OK == retaux )
					{
						sizeMsg= processDataOk( &data, data_out_buffer );
						if( RET_ERROR != sizeMsg )
						{
							fputs( data_out_buffer, fd_data_output );
							retval= RET_OK;
						}
					}
					else if( RET_ERROR == retaux )
					{
						sizeMsg= processDataError( &data, data_out_buffer );
						if( RET_ERROR != sizeMsg )
						{
							fputs( data_out_buffer, fd_data_output );
						}
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			puts( "\n\rNo se pudo abrir process_tem_tcupla.txt\n" );
		}
	}
	else
	{
		puts( "\n\rNo se pudo abrir sim_voltage_tcupla.svc\n" );
	}

	fclose ( fd_data_input );
	fclose ( fd_data_output );

	return retval;
}

static cntc_t termistor;
static int test2 ( void )
{
	int sizeMsg, retaux, retval= RET_ERROR;
	FILE * fd_data_input;
	FILE * fd_data_output;
	dntc_t data;
	char data_inp_buffer[SIZE_BUFF];
	char data_out_buffer[SIZE_BUFF];

	fd_data_input = fopen ( "sim_voltage_termNTC.svc", "r+" );
	if( NULL !=  fd_data_input )
	{
		fd_data_output = fopen ( "process_tem_termNTC.txt", "w+" );
		if( NULL !=  fd_data_output )
		{
			fputs( "R_med\t\t T_cal\t\t R_cal\t\t\t R_error\n"
				   "---------------------------------------------------\n", fd_data_output );
			while( !feof( fd_data_input ) )
			{
				if( NULL != fgets( data_inp_buffer, SIZE_BUFF, fd_data_input ) )
				{
					sizeMsg= strlen( data_inp_buffer );

					retaux= processBufferNtc( &data, data_inp_buffer, sizeMsg );
					if( RET_OK == retaux )
					{
						sizeMsg= processNtcDataOk( &data, data_out_buffer );
						if( RET_ERROR != sizeMsg )
						{
							fputs( data_out_buffer, fd_data_output );
							retval= RET_OK;
						}
					}
					else if( RET_ERROR == retaux )
					{
						sizeMsg= processNtcDataError( &data, data_out_buffer );
						if( RET_ERROR != sizeMsg )
						{
							fputs( data_out_buffer, fd_data_output );
						}
					}
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			puts( "\n\rNo se pudo abrir process_tem_tcupla.txt\n" );
		}
	}
	else
	{
		puts( "\n\rNo se pudo abrir sim_voltage_tcupla.svc\n" );
	}

	fclose ( fd_data_input );
	fclose ( fd_data_output );

	return retval;
}

static int processBufferNtc ( dntc_t *data, char * buffer, int sizeBuff )
{
	char * pTemp, *pRes;
	static double tempCalibracion[3]={0,0,0};
	static double resisCalibracion[3]={0,0,0};
	static int i=0, conf=0;

	double restaux;
	double tempaux;

	int retval= RET_VOID;

	if( NULL != ( pTemp = strchr( buffer, '@' ) ))
	{
		if( NULL != ( pRes = strchr( pTemp, 'R' ) ))
		{
			*pRes='\0';

			if( '?'!= *(++pTemp) )
			{
				tempCalibracion[i]= atof( pTemp );
				resisCalibracion[i]= atof( ++pRes );

				if( 2 < (++i) )
				{
					if( tstorCalcCoef(resisCalibracion, tempCalibracion, &termistor) )
					{
						conf=1;
						i=0;
					}
				}
			}
			else
			{
				retval= RET_ERROR;
				if( conf )
				{
					data->resisMedida = atoi( ++pRes );
					tempaux= tstorResToTemp (data->resisMedida, &termistor);

					if( ERROR_TEMP != tempaux )
					{
						data->tempCalc= tempaux;

						restaux= tstorTempToRes( tempaux, &termistor );
						if( ERROR_CALC != restaux )
						{
							data->resisCalc= restaux;
							data->resisError= fabs(data->resisCalc - data->resisMedida);
							retval= RET_OK;
						}
					}
				}
			}
		}
	}

	return retval;
}

static int processBuffer ( dproc_t *data, char * buffer, int sizeBuff )		
{
	char * pTemp, *pVol;
	volt_t voltaux;
	temp_t tempaux;
	int retval= RET_VOID;

	if( NULL != ( pTemp = strchr( buffer, '@' ) ) )
	{
		if( NULL != ( pVol = strchr( pTemp, 'V' ) ) )
		{
			*pVol='\0';
			data->tempAmb = atoi( ++pTemp );
			data->voltTcupla = atof( ++pVol );

			retval= RET_ERROR;
			tempaux= tcuplaKVoltToTemp( data->voltTcupla, data->tempAmb );
			if( ERROR_TEMP != tempaux )
			{
				data->tempCalc= tempaux;

				voltaux= tcuplaKTempToVolt( tempaux, data->tempAmb );
				if( ERROR_VOLT != voltaux )
				{
					data->voltCalc= voltaux;
					data->voltError= fabs(voltaux - data->voltTcupla);
					retval= RET_OK;
				}	
			}
		}
	}
	return retval;
}

static int	processDataOk( dproc_t *data, char * buffer )
{
	if( 0 < sprintf( buffer, "@%d°C\t %1.3fmV\t %d°C \t %1.3fmV \t %1.2f\%\n",
		data->tempAmb, data->voltTcupla, data->tempCalc, data->voltCalc, (data->voltError/data->voltTcupla)*100 ))
	{
		return strlen( buffer );
	}
	return RET_ERROR;
}

static int	processDataError( dproc_t *data, char * buffer )
{
	if( 0 < sprintf( buffer, "@%d°C\t %1.3fmV\t [Err]\t [Err]\t\t [Err] \n",
		data->tempAmb, data->voltTcupla) )
	{
		return strlen( buffer );
	}
	return RET_ERROR;
}

static int	processNtcDataOk( dntc_t *data, char * buffer )
{
	if( 0 < sprintf( buffer, "%d Omhs\t %1.2f°C\t %d Ohms\t\t %1.2f\%\n",
		(int)data->resisMedida, data->tempCalc, (int)data->resisCalc, (data->resisError/data->resisMedida)*100 ))
	{
		return strlen( buffer );
	}
	return RET_ERROR;
}

static int	processNtcDataError( dntc_t *data, char * buffer )
{
	if( 0 < sprintf( buffer, "%d Omhs\t [Err]\t [Err]\t\t [Err] \n", (int)data->resisMedida ) )
	{
		return strlen( buffer );
	}
	return RET_ERROR;
}

