/*
 * consola.h
 *
 *  Created on: 22/10/2014
 *      Author: Seba
 */

#ifndef CONSOLA_SHELL_H_
#define CONSOLA_SHELL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USE_RTOS

/************************************************************************/
#define MSG_BIENVENIDA		"User command example for NT-Shell.\r\n"
#define MSG_PROMPT 			"LPC824>"

#define MSG_MODO_DEBUG 		"\r\n->"
#define MSG_MODO_STANDARD 	"\r\n >"
#define MSG_MODO_CONTINUO 	" "
#define MSG_MODO_DEFAULT  	"\r\n "

typedef enum modeprint
{
	MP_DEF=0,
	MP_DEB,
	MP_EST,
	MP_SIN_NL

} modep_t;

typedef struct
{
	uint8_t 	mode;
	uint8_t 	cmdShell;

	uint16_t 	size;
	char* 		msg;
	//TickType_t ticktimes;
} terMsg_t;

/**********************************************************************/

void 	taskTerminal  ( void * parametrosTarea );


/************************************************************************/
// Macros:
static inline void Terminal_Msg_Def( terMsg_t * msgToSend, char* str )
{
	msgToSend->msg= str;
	msgToSend->size= strlen(str);
	msgToSend->mode= MP_DEF;
}

static inline void Terminal_Msg_InL( terMsg_t * msgToSend, char* str )
{
	msgToSend->msg= str;
	msgToSend->size= strlen(str);
	msgToSend->mode= MP_SIN_NL;
}

static inline void Terminal_Msg_Est( terMsg_t * msgToSend, char* str )
{
	msgToSend->msg= str;
	msgToSend->size= strlen(str);
	msgToSend->mode= MP_EST;
}

static inline void Terminal_Msg_Deb( terMsg_t * msgToSend, char* str )
{
	msgToSend->msg= str;
	msgToSend->size= strlen(str);
	msgToSend->mode= MP_DEB;
}

#endif /* CONSOLA_H_ */
