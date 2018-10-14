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

/**********************************************************************/

void 	taskTerminal  ( void * parametrosTarea );
void 	printTerminal ( const char *texto, modep_t modo_impresion );


/************************************************************************/
// Macros:
#define terminal_msg_promt(msg)		printTerminal(msg, MP_EST)
#define terminal_msg_continue(msg)	printTerminal(msg, MP_SIN_NL)
#define terminal_msg_nline(msg)		printTerminal(msg, MP_DEF)
#define terminal_msg_debug(msg)		printTerminal(msg, MP_DEB)


#endif /* CONSOLA_H_ */
