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
#define MSG_BIENVENIDA		"***********************************************************************\r\n"
#define MSG_PROMPT 			"SYSMON>"

#define MSG_MODO_DEBUG 		"\r\n->"
#define MSG_MODO_STANDARD 	"\r\n>"
#define MSG_MODO_CONTINUO 	" "


typedef enum modeprint
{
	MP_CMD=0,
	MP_BLOCK,
	MP_RELEASE,
	MP_PRINT_NORMAL,
	MP_PRINT_DEBUG,
	MP_PRINT_CONTINUE

} modep_t;

typedef struct
{
	uint8_t 	mode;
	uint8_t 	dummy;

	uint16_t 	size;
	char* 		msg;
	//TickType_t ticktimes;
} terMsg_t;

/**********************************************************************/

void 	taskTerminal  ( void * parametrosTarea );


/************************************************************************/
// Macros:
static inline void Terminal_Msg_Normal( terMsg_t * msgToSend, char* str )
{
	msgToSend->msg= str;
	msgToSend->size= strlen(str);
	msgToSend->mode= MP_PRINT_NORMAL;
}

static inline void Terminal_Msg_Continue( terMsg_t * msgToSend, char* str )
{
	msgToSend->msg= str;
	msgToSend->size= strlen(str);
	msgToSend->mode= MP_PRINT_CONTINUE;
}

static inline void Terminal_Msg_Debug( terMsg_t * msgToSend, char* str )
{
	msgToSend->msg= str;
	msgToSend->size= strlen(str);
	msgToSend->mode= MP_PRINT_DEBUG;
}

static inline void Terminal_Take( terMsg_t * msgToSend )
{
	msgToSend->mode= MP_BLOCK;
}

static inline void Terminal_Release( terMsg_t * msgToSend)
{
	msgToSend->mode= MP_RELEASE;
}

#endif /* CONSOLA_H_ */
