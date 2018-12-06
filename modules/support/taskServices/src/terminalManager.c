/*
 * consola.c
 *
 *  Created on: 22/10/2014
 *      Author: Seba
 */

#include "terminalManager.h"
#include "services_config.h"

#include "usrcmd.h"
#include "ntshell.h"
#include "api_UART.h"

//Definicion del RTOS a usar --------------------------------------------------

#define L_FIFO_UART 		300
#define AUX_ARGC			32
#define TOKENS				" ."
//**** VT100 Defines ********************************************************
#define PROMT_COL_DEF		0
#define PROMT_FIL_DEF		20
#define MSG_COL_DEF			0
#define MSG_FIL_DEF			0

#define WINDOWS_COL_MAX		80
#define WINDOWS_FIL_MAX		26


#define SET_POSITION(C,L)	( vtsend_cursor_position( &nts.vtsend, C, L ) ) //( vt_SetPosition( C, L ) )
#define SET_NEW_LINE(X)		( {vtsend_cursor_down( &nts.vtsend, X ); vtsend_cursor_backward( &nts.vtsend, WINDOWS_COL_MAX)} ) //( vt_SetNewLine(X) )

#define PROMT_POS_INIT()	( SET_POSITION( PROMT_COL_DEF, PROMT_FIL_DEF ) ) //( vt_SetPosition( PROMT_COL_DEF, PROMT_FIL_DEF ) )
#define PROMT_POS_INIT_COL() ( vtsend_cursor_backward( &nts.vtsend, WINDOWS_COL_MAX) ) //( vt_PromtPosInit() )
#define PROMT_POS_SALVE()	 ( vtsend_cursor_position_save( &nts.vtsend ) ) 	//( vt_PromtPosSalve() )
#define PROMT_POS_RESTORE()	 ( vtsend_cursor_position_restore( &nts.vtsend ) ) //( vt_PromtPosRestore() )

#define PROMT_POS_UP_FIL(X) ( (WINDOWS_FIL_MAX<X)? 1: 0 )
#define PROMT_POS_UP_COL(X) ( (WINDOWS_COL_MAX<X)? 1: 0 )

#define MSG_POS_INIT()		( SET_POSITION( MSG_COL_DEF, MSG_FIL_DEF ) ) //( vt_SetPosition( MSG_COL_DEF, MSG_FIL_DEF ) )
#define MSG_POS_UP_FIL(X)	( (PROMT_FIL_DEF<X)? 1: 0 )

//**************************************
//#define SET_POSITION(C,L)	( 1 )
//#define SET_NEW_LINE(X)		( 1 )

//#define PROMT_POS_INIT()	( 1 )
//#define PROMT_POS_INIT_COL() ( 1 )
//#define PROMT_POS_SALVE()	( 1 )
//#define PROMT_POS_RESTORE()	( 1 )

//#define PROMT_POS_UP_FIL(X) ( 1 )
//#define PROMT_POS_UP_COL(X) ( 1 )

//#define MSG_POS_INIT()	( 1 )
//#define MSG_POS_UP_FIL(X)	( 1 )

//***************** ********************************************************

extern TaskHandle_t			MGR_TERMINAL_HANDLER;
extern SemaphoreHandle_t 	MGR_TERMINAL_MUTEX;
extern QueueHandle_t 		MGR_TERMINAL_QUEUE;
extern QueueHandle_t 		MGR_DATALOG_QUEUE;
extern QueueHandle_t 		MGR_INPUT_QUEUE;
extern QueueHandle_t 		MGR_OUTPUT_QUEUE;

extern UBaseType_t*			STACKS_TAREAS;
/***********************************************************************************/

typedef struct
{
    char *command;
    char *description;
    void (*func) (int argc, char **argv);

} command_table_t;

typedef enum modeinput
{
	CONSOLA,
	INTERNO,
	PROCESANDO,
	FIN_PROCESO

} modein_t;

/***********************************************************************************/
static modein_t		inputDatos= CONSOLA;
static char 		buffInpProg[L_FIFO_UART];

static char * 		auxargv [AUX_ARGC];
static int 			auxargc;
static int			nOverFullCS;

static dlogPack_t 	dataToLog;
static ntshell_t 	nts;

/* Funciones privadas ***************************************************/
static int  readTerminal	 ( char *, int , void * );
static int  writeTerminal	 ( const char *, int , void * );
static int  callParserShell	 ( const char *, void * );
static int  sendCommandShell ( char * );
static int  printTerminal 	 ( terMsg_t * objMsg );

/* Funciones publicas **************************************************/

void taskTerminal (void * parametrosTarea)
{
	terMsg_t msgRecived;
    cmd_data_t pDataInp;
	dInOutQueue_t dataGpio;


    UART_Init();
    ntshell_init(&nts, readTerminal, writeTerminal, callParserShell, NULL);
    ntshell_set_prompt(&nts, MSG_PROMPT);

    SET_POSITION(PROMT_COL_DEF, PROMT_FIL_DEF-1);
    sendStr_DEBUG(MSG_BIENVENIDA);
    sendStr_DEBUG(nts.prompt);
	PROMT_POS_SALVE();

	while(1)
	{
		if( pdTRUE == xQueueReceive( MGR_TERMINAL_QUEUE, &msgRecived, TIMEOUT_QUEUE_MSG_INP ))
		{
			switch( msgRecived.mode )
			{
				case MP_CMD:
					if( sendCommandShell( msgRecived.msg ) )
					{
						callParserShell ((const char *) auxargv[0], (void *) &auxargc);
					}
					break;
				case MP_PRINT_NORMAL:
				case MP_PRINT_DEBUG:
				case MP_PRINT_CONTINUE:
					if( printTerminal( &msgRecived ))
					{
					}
					break;

				case MP_BLOCK:
					inputDatos= PROCESANDO;
					break;

				case MP_RELEASE:
				default:
					inputDatos= CONSOLA;
					break;
			}
		}

		//if( CONSOLA == inputDatos )
		{
			ntshell_execute(&nts);

			if( usrcmd_getStatus( &pDataInp ))
			{
				switch( pDataInp.destino ) {
					case CMD_TASK_N1:
						dataToLog.cmd= readPage;
						dataToLog.nReg= pDataInp.arg1;
						dataToLog.auxVal_1= pDataInp.arg2;
						dataToLog.auxVal_2= pDataInp.arg3;
						xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
						break;
					case CMD_TASK_N2:
						//if( 0xFFFF == pDataInp.arg1 )
						{
							dataGpio.mode= inputMonitor;
							dataGpio.gpio= pDataInp.arg1;
						}
						xQueueSend( MGR_INPUT_QUEUE, &dataGpio, TIMEOUT_QUEUE_INPUT );
						break;
					default:
						break;
				}
				usrcmd_setStatus();
			}
		}
/*
		if( INTERNO == inputDatos )
		{
			callParserShell ((const char *) auxargv[0], (void *) &auxargc);
			inputDatos= CONSOLA;
		}

		if( PROCESANDO == inputDatos )
		{
			if( 'c'== UART_RecvChar() )
			{
				xQueueSend( MGR_INPUT_QUEUE, &dataGpio, TIMEOUT_QUEUE_INPUT );
			}
		}
*/
		ACTUALIZAR_STACK( MGR_TERMINAL_HANDLER, MGR_TERMINAL_ID_STACK );
		vTaskDelay( MGR_TERMINAL_DELAY );
	}
}

/*
 * Funcion que envia un string a la terminal de debuging.
 * Al usar RTOS la misma debe de tener acceso sobre el recurso.
 */
static int printTerminal( terMsg_t * objMsg )
{
	int retval=0;
	static int col=MSG_COL_DEF;
	static int fil=MSG_FIL_DEF;
	uint16_t byteRest= objMsg->size;

	if( TOMAR_SEMAFORO(	MGR_TERMINAL_MUTEX, TIMEOUT_MUTEX_CONSOLA) )
	{
		PROMT_POS_SALVE();
		SET_POSITION(col, fil);

		switch (objMsg->mode)
		{
		case MP_PRINT_DEBUG:
			sendStr_DEBUG( MSG_MODO_DEBUG );
			col+=2;
			break;
		case MP_PRINT_CONTINUE:
			sendStr_DEBUG( MSG_MODO_CONTINUO );
			col+=1;
			break;
		case MP_PRINT_NORMAL:
		default:
			sendStr_DEBUG( MSG_MODO_STANDARD );
			col+=1;
		}

		do
		{
			if( (WINDOWS_COL_MAX-col) < byteRest )
			{
				if( !sendBuf_DEBUG( objMsg->msg, WINDOWS_COL_MAX ))
				{
					nOverFullCS++;
				}
				byteRest-= WINDOWS_COL_MAX;
				//MSG_NEW_LINE();
			}
			else
			{
				if( !sendStr_DEBUG( objMsg->msg ))
				{
					nOverFullCS++;
				}
			}
			if( MSG_POS_UP_FIL(++fil) )
			{
				// FIXME aca debe ir la rutina que limpia esta parte de la pantalla.
				fil=MSG_FIL_DEF;
			}
			col=MSG_COL_DEF;
			SET_POSITION( col, fil );
		}
		while( WINDOWS_COL_MAX < byteRest );

		if( LIBERAR_SEMAFORO( MGR_TERMINAL_MUTEX ) )
		{
		}
		retval= 1;

		//PROMT_POS_RESTORE();
	}
	return retval;
}

/************************************************************************/

/**** Serial write function **
 * Rutina utilizda por el ntshell.
 * Proporciona el acceso al medio para la informacion procesada por los comandos.
 * Si se usa RTOS la misma debe de poder acceder al recurso previamente.
 */
static int writeTerminal(const char *buf, int cnt, void *extobj)
{
	static int nColPromt= PROMT_COL_DEF;
	static int nFilPromt= PROMT_FIL_DEF;
	int i = 0;

	if( TOMAR_SEMAFORO(	MGR_TERMINAL_MUTEX, TIMEOUT_MUTEX_CONSOLA) )
	{
		PROMT_POS_RESTORE();

		for (; i < cnt; i++)
		{
			sendChr_DEBUG( buf[i] );
			if( '\n' == buf[i] )
			{
				if( PROMT_POS_UP_FIL( ++nFilPromt ) )
				{
					nFilPromt= PROMT_FIL_DEF;
					nColPromt= PROMT_COL_DEF;
					PROMT_POS_INIT();
				}
			}
			else
			{
				if( PROMT_POS_UP_COL( ++nColPromt ) )
				{
					//PROMT_POS_INIT_COL();
					nColPromt= PROMT_COL_DEF;
					if( PROMT_POS_UP_FIL( ++nFilPromt ) )
					{
						nFilPromt= PROMT_FIL_DEF;
						nColPromt= PROMT_COL_DEF;
						PROMT_POS_INIT();
					}
					else
					{
						SET_POSITION( nColPromt, nFilPromt );
					}
				}
			}
		}
		PROMT_POS_SALVE();

		if( LIBERAR_SEMAFORO( MGR_TERMINAL_MUTEX ) )
		{
			//i++;
		}
	}

	return cnt;
}

/**** Serial read function ****
 * Rutina utilizada por el ntshell.
 * Proporciona el acceso para lectura de caracteres para la preparacion de comandos.
 */
static int readTerminal(char *buf, int cnt, void *extobj)
{
    /*
    for (int i = 0; i < cnt; i++) {
        buf[i] = uartRecvChar();
    }
    */
    return recvBuf_DEBUG ( buf, cnt );
}

/************************************************************************/

/**** Callback function for ntshell module ***
 * Rutina encargada de llamar y procesar los comandos segun los predefinidos en usercmd.c
 */
static int callParserShell(const char *text, void *extobj)
{
    return usrcmd_execute(text);
}

/*
 * Funcion que parsea un string segun los tokens definidos en TOKENS y arma el vector
 * auxiliar auxargv para ser prosesado por el ntshell en la proxima llamada.
 */
static int sendCommandShell (char * string)
{
	int retval=0;
	char * nextWord;
	int i=0;

	if( NULL != string)
	{
		strcpy (buffInpProg, string);

		nextWord= strtok( buffInpProg, TOKENS);

		while (nextWord != NULL && i< AUX_ARGC)
		{
			auxargv[i++]= nextWord;
			nextWord= strtok (NULL, TOKENS);
		}
		auxargc= i;

		// Blanqueo los demas punteros:
		if (i< AUX_ARGC)
		{
			while (i< AUX_ARGC)
			{
				auxargv[i++]= NULL;
			}
		}
		retval=1;
	}

	return retval;
}

/*
static inline void vt_SetPosition (int C, int L)
{
	vtsend_cursor_position( &nts.vtsend, C, L );
}

static inline void vt_SetNewLine (int X)
{
	vtsend_cursor_down( &(nts.vtsend), X );
	vtsend_cursor_backward( &(nts.vtsend), WINDOWS_COL_MAX);
}

static inline void vt_PromtPosInit (void)
{
	vtsend_cursor_backward( &(nts.vtsend), WINDOWS_COL_MAX);
}

static inline void vt_PromtPosSalve (void)
{
	vtsend_cursor_position_save( &(nts.vtsend) );
}

static inline void vt_PromtPosRestore (void)
{
	vtsend_cursor_position_restore( &(nts.vtsend) );
}
*/

