/*
 * consola.c
 *
 *  Created on: 22/10/2014
 *      Author: Seba
 */

#include "terminalManager.h"

#include "usrcmd.h"
#include "ntshell.h"
#include "api_UART.h"

//Definicion del RTOS a usar --------------------------------------------------
#ifdef  USE_RTOS
#include "services_config.h"
#endif

#define L_FIFO_UART 		300
#define AUX_ARGC			32
#define TOKENS				" ."

extern xTaskHandle 			MGR_TERMINAL_HANDLER;
extern xSemaphoreHandle 	MGR_TERMINAL_MUTEX;
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
static modein_t				inputDatos= CONSOLA;
static char 				buffInpProg[L_FIFO_UART];
static char * 				auxargv [AUX_ARGC];
static int 					auxargc;
static int					nOverFullCS;

/* Funciones privadas ***************************************************/
static int readTerminal		( char *, int , void * );
static int writeTerminal	( const char *, int , void * );
static int callParserShell	( const char *, void * );
static int sendCommandShell ( char * );


/* Funciones publicas **************************************************/

void taskTerminal (void * parametrosTarea)
{
    void *extobj = 0;
    ntshell_t nts;

    UART_Init();
    sendStr_DEBUG(MSG_BIENVENIDA);
    ntshell_init(&nts, readTerminal, writeTerminal, callParserShell, extobj);
    ntshell_set_prompt(&nts, MSG_PROMPT);
    printTerminal(nts.prompt, MP_DEF);

	while(1)
	{
		ACTUALIZAR_STACK( MGR_TERMINAL_HANDLER, MGR_TERMINAL_ID_STACK );
		//TOMAR_SEMAFORO(	MGR_TERMINAL_MUTEX, TIMEOUT_MUTEX_CONSOLA);
		ntshell_execute(&nts);

		if( INTERNO == inputDatos )
		{
			callParserShell ((const char *) auxargv[0], (void *) &auxargc);
			inputDatos= CONSOLA;
		}
		//LIBERAR_SEMAFORO( MGR_TERMINAL_MUTEX );
		vTaskDelay( MGR_TERMINAL_DELAY );

	}
}

/*
 * Funcion que envia un string a la terminal de debuging.
 * Al usar RTOS la misma debe de tener acceso sobre el recurso.
 */
void printTerminal(const char * texto, modep_t mode)
{
#ifdef USE_RTOS
	TOMAR_SEMAFORO(	MGR_TERMINAL_MUTEX, TIMEOUT_MUTEX_CONSOLA);
#endif

	switch (mode) {
	    case MP_DEB:
	    	sendStr_DEBUG(MSG_MODO_DEBUG);
	    	break;
	    case MP_EST:
	    	sendStr_DEBUG(MSG_MODO_STANDARD);
	    	break;
	    case MP_SIN_NL:
	    	sendStr_DEBUG(MSG_MODO_CONTINUO);
	    	break;
	    case MP_DEF:
	    default:
	    	sendStr_DEBUG(MSG_MODO_DEFAULT);
	}
	if (sendStr_DEBUG(texto) == 0)
	{
		nOverFullCS++;
	}
#ifdef USE_RTOS
	LIBERAR_SEMAFORO( MGR_TERMINAL_MUTEX );
#endif
}

/************************************************************************/

/**** Serial write function **
 * Rutina utilizda por el ntshell.
 * Proporciona el acceso al medio para la informacion procesada por los comandos.
 * Si se usa RTOS la misma debe de poder acceder al recurso previamente.
 */
static int writeTerminal(const char *buf, int cnt, void *extobj)
{
	int i = 0;
#ifdef USE_RTOS
	TOMAR_SEMAFORO(	MGR_TERMINAL_MUTEX, TIMEOUT_MUTEX_CONSOLA);
#endif

    for (; i < cnt; i++)
    {
    	sendChr_DEBUG( buf[i] );
    }

#ifdef USE_RTOS
	LIBERAR_SEMAFORO( MGR_TERMINAL_MUTEX );
#endif

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
	char * nextWord;
	int i=0;

	if (string == NULL) return 0;

	strcpy (buffInpProg, string);

	nextWord= strtok( buffInpProg, TOKENS);

	while (nextWord != NULL && i< AUX_ARGC)
	{
		auxargv[i++]= nextWord;
		nextWord= strtok (NULL, TOKENS);
	}

	auxargc= i;
	inputDatos= INTERNO;

	// Blanqueo los demas punteros:
	if (i< AUX_ARGC)
	{
		while (i< AUX_ARGC)
		{
			auxargv[i++]= NULL;
		}
	}
	return 1;
}

