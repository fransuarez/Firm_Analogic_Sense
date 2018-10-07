/*
 * consola.c
 *
 *  Created on: 22/10/2014
 *      Author: Seba
 */

#include "ntshell.h"
#include "api_UART.h"
#include "usrcmd.h"
#include "shellManager.h"

//Definicion del RTOS a usar --------------------------------------------------
#ifdef  USE_RTOS
#include "services_config.h"
#endif

#define L_FIFO_UART 		300
#define AUX_ARGC			32

#define MSG_BIENVENIDA		"User command example for NT-Shell.\r\n"
#define MSG_PROMPT 			"LPC824>"
#define MSG_MODO_DEBUG 		"\r\n->"
#define MSG_MODO_STANDARD 	"\r\n >"
#define MSG_MODO_CONTINUO 	" "
#define MSG_MODO_DEFAULT  	"\r\n "

#define TOMAR_SEMAFORO_CONSOLA		xSemaphoreTake( MUTEX_CONSOLA, TIMEOUT_MUTEX_CONSOLA)
#define LIBERAR_SEMAFORO_CONSOLA	xSemaphoreGive( MUTEX_CONSOLA )

extern xTaskHandle 			HANDLER_CONSOLA;
extern xSemaphoreHandle 	MUTEX_CONSOLA;
extern UBaseType_t*			STACKS_TAREAS;

static modein_t				inputDatos= CONSOLA;
static char 				buffInpProg[L_FIFO_UART];
static char * 				auxargv [AUX_ARGC];
static int 					auxargc;
static int					nOverFullCS;

/* Funciones privadas ***************************************************/
static int serial_read		(char *buf, int cnt, void *extobj);
static int serial_write		(const char *buf, int cnt, void *extobj);
static int user_callback	(const char *text, void *extobj);
//static int chequearUART (void) ;

/* Funciones publicas **************************************************/

void taskConsola (void * parametrosTarea)
{
    void *extobj = 0;
    ntshell_t nts;

    UART_Init();
    sendStr_DEBUG(MSG_BIENVENIDA);
    ntshell_init(&nts, serial_read, serial_write, user_callback, extobj);
    ntshell_set_prompt(&nts, MSG_PROMPT);
    printConsola(nts.prompt, MP_DEF);

	while(1)
	{
#ifdef USE_RTOS
		ACTUALIZAR_STACK_TAREA( HANDLER_CONSOLA, ID_STACK_CONSOLA );
		TOMAR_SEMAFORO_CONSOLA;
#endif
		ntshell_execute(&nts);

		if (inputDatos== INTERNO)
		{
			user_callback ((const char *) auxargv[0], (void *) &auxargc);
			inputDatos= CONSOLA;
		}

#ifdef USE_RTOS
		LIBERAR_SEMAFORO_CONSOLA;
		LIBERAR_TAREA;
#endif
	}
}

void printConsola(const char * texto, modep_t mode)
{
#ifdef USE_RTOS
	TOMAR_SEMAFORO_CONSOLA;
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
		nOverFullCS++;

#ifdef USE_RTOS
	LIBERAR_SEMAFORO_CONSOLA;
#endif
}

int sendConsola (char * string)
{
	char * nextWord;
	int i=0;

	if (string == NULL) return 0;

	strcpy (buffInpProg, string);

	nextWord= strtok (buffInpProg, " .");
	while (nextWord != NULL && i< AUX_ARGC)
	{
		auxargv[i++]= nextWord;
		nextWord= strtok (NULL, " .");
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

/************************************************************************/
/**** Serial read function *****/
static int serial_read(char *buf, int cnt, void *extobj)
{
    /*
    for (int i = 0; i < cnt; i++) {
        buf[i] = uartRecvChar();
    }
    */
    return recvBuf_DEBUG ( buf, cnt );
}

/**** Serial write function ****/
static int serial_write(const char *buf, int cnt, void *extobj)
{
	int i = 0;
#ifdef USE_RTOS
	TOMAR_SEMAFORO_CONSOLA;
#endif

    for (; i < cnt; i++)
    {
    	sendChr_DEBUG(buf[i]);
    }

#ifdef USE_RTOS
	LIBERAR_SEMAFORO_CONSOLA;
#endif

    return cnt;
}

/**** Callback function for ntshell module ****/
static int user_callback(const char *text, void *extobj)
{
    /*
     * This is a complete example for a real embedded application.
     */
    return usrcmd_execute(text);
}

/*
int chequearUART (void)
{
	if (consola.colaTx.cantidadEnCola == 0 && consola.colaTx.pOut != consola.colaTx.pIn) {
		consola.colaTx.pOut= consola.colaTx.pIn;
		return 1;
	}
	else return 0;
}
*/

