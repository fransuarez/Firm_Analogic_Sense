/*
 * consola.c
 *
 *  Created on: 22/10/2014
 *      Author: Seba
 */

#include "services_Shell.h"
#include "ntshell.h"
#include "ciaaUART.h"
#include "ciaaIO.h"
#include "usrcmd.h"

ntshell_t 		ntshell;
command_table_t *ptr_commandList;

static modein_t		inputDatos= CONSOLA;
static char 		buffInpProg[L_FIFO_UART];
static char * 		auxargv [AUX_ARGC];
static int 			auxargc;
static int			nOverFullCS;

/* Funciones privadas ***************************************************/
static int serial_read(char *buf, int cnt, void *extobj);
static int serial_write(const char *buf, int cnt, void *extobj);
static int user_callback(const char *text, void *extobj);
//static int chequearUART (void) ;

/* Funcioines publicas **************************************************/
#ifdef USE_RTOS
void taskConsola (void * parametrosTarea)
{
    void *extobj = 0;
    ntshell_t nts;

    ciaaUARTInit();
    ciaaIOInit();
    uartSendStr("User command example for NT-Shell.\r\n");
    ntshell_init(&nts, serial_read, serial_write, user_callback, extobj);
    ntshell_set_prompt(&nts, "LPC824>");
    printConsola(nts.prompt, MP_DEF);

	while(1)
	{
		*(ptrstack+2)= uxTaskGetStackHighWaterMark( pxCreatedTask3 );
		xSemaphoreTake( mutexConsola, ( portTickType ) 10  );
		ntshell_execute(&nts);

		if (inputDatos== INTERNO) {
			//xSemaphoreTake( mutexConsola, ( portTickType ) 10  );
			user_callback ((const char *) auxargv[0], (void *) &auxargc);
			inputDatos= CONSOLA;
			//xSemaphoreGive( mutexConsola );
		}
		xSemaphoreGive( mutexConsola );

		//vTaskDelay(10);
		taskYIELD();
	}
}
#endif

void printConsola(const char * texto, modep_t mode)
{
	//taskENTER_CRITICAL();
	if (mutexConsola == NULL) return;

	xSemaphoreTake( mutexConsola, ( portTickType ) 10  );
	//vTaskSuspendAll();

	//if (chequearUART()!=0);
		//UARTputs (&consola, "\n\r"); //-->Correccion de Cola Tx

	switch (mode) {
	    case MP_DEB:
	    	uartSendStr("\r\n->");
	    	break;
	    case MP_EST:
	    	uartSendStr("\r\n >");
	    	break;
	    case MP_SIN_NL:
	    	uartSendChar(' ');
	    	break;
	    case MP_DEF:
	    default:
	    	uartSendStr("\r\n ");
	}
	if (uartSendStr(texto) == 0)
		nOverFullCS++;
		//while (1);

	//xTaskResumeAll();
	xSemaphoreGive( mutexConsola );
}

int sendConsola (char * string)
{
	char * nextWord;
	int i=0;

	if (string == NULL) return 0;

	strcpy (buffInpProg, string);
	//auxargv[i++]= string;

	nextWord= strtok (buffInpProg, " .");
	while (nextWord != NULL && i< AUX_ARGC)
	{
		//printf ("%s\n",nextWord);
		auxargv[i++]= nextWord;
		nextWord= strtok (NULL, " .");
	}
	auxargc= i;
	inputDatos= INTERNO;

	// Blanqueo los demas punteros:
	if (i< AUX_ARGC)
	{
		while (i< AUX_ARGC)
			auxargv[i++]= NULL;
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
    return dbgRead( buf,cnt );
}

/**** Serial write function ****/
static int serial_write(const char *buf, int cnt, void *extobj)
{
	int i = 0;
	xSemaphoreTake( mutexConsola, ( portTickType ) 10  );
	//if (chequearUART()!=0);

    for (; i < cnt; i++)
    {
    	uartSendChar(buf[i]);
    }
    xSemaphoreGive( mutexConsola );
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

