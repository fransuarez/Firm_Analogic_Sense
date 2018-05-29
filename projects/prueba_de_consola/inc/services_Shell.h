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

//Definicion del RTOS a usar --------------------------------------------------
#ifdef  USE_RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern xTaskHandle 			pxCreatedTask3;
extern xSemaphoreHandle 	mutexConsola;
extern unsigned long *		ptrstack;

#endif

/************************************************************************/
#define L_FIFO_UART 	300
#define AUX_ARGC		32

typedef struct
{
    char *command;
    char *description;
    void (*func) (int argc, char **argv);

} command_table_t;

typedef enum modeprint
{
	MP_DEF=0,
	MP_DEB,
	MP_EST,
	MP_SIN_NL

} modep_t;

typedef enum modeinput
{
	CONSOLA,
	INTERNO,
	PROCESANDO,
	FIN_PROCESO

} modein_t;
/************************************************************************/
extern command_table_t * ptr_commandList;
xSemaphoreHandle mutexConsola;

//extern const command_table_t cmdlist[];
/************************************************************************/
void 	taskConsola ( void * parametrosTarea );
void 	printConsola (const char *texto, modep_t modo_impresion);
int 	sendConsola  (char *);

#endif /* CONSOLA_H_ */
