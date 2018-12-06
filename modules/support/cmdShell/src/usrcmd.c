/**
 * @file usrcmd.c
 * @author CuBeatSystems
 * @author Shinichiro Nakamura
 * @copyright
 * ===============================================================
 * Natural Tiny Shell (NT-Shell) Version 0.3.1
 * ===============================================================
 * Copyright (c) 2010-2016 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

//#include "usrcmd.h"
#include "ntlibc.h"
#include "ntopt.h"
#include "api_UART.h"
#include "usrcmd.h"
#include <stdlib.h>

//extern sch_task_t schedTask[LENG_TASK];
//extern xSemaphoreHandle mutexSheduler;
//extern bool	printEstado;
#define DATA_PROCCESS_START() 	( flag_data_proccess=1 )
#define DATA_PROCCESS_END()		( flag_data_proccess=0 )
//static char	aux_data_buff[BUFF_SIZE];
static int flag_data_proccess= 0;

//********************************************************************************************
static int cmd_statSystem 		(int argc, char **argv);
static int cmd_readLogs 		(int argc, char **argv);
static int cmd_monInputs 		(int argc, char **argv);
static int cmd_stopProcess 		(int argc, char **argv);

static int usrcmd_help			( int argc, char **argv );
static int usrcmd_info			( int argc, char **argv );

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int checkandSet			(char * val, int min, int max);

//********************************************************************************************

typedef int (*USRCMDFUNC)(int argc, char **argv);

typedef struct {
    char *cmd;
    char *desc;
    USRCMDFUNC func;

} cmd_table_t;

//********************************************************************************************
static const cmd_table_t cmdlist[] = {
    { "help", "This is a description text string for help command.", usrcmd_help },
    { "info", "This is a description text string for info command.", usrcmd_info },
	{ "stat", "Muestra el estado general del sistema"				, cmd_statSystem 		},

	{ "hist", "Devuelve los últimos registros guardados en memoria."				, cmd_readLogs   },
	{ "mon" , "Muestra el valor y la variación de las entradas y salidas."			, cmd_monInputs },
	{ "conf", "Configura pines max, min y tiempos de muestreo."						, cmd_statSystem },
//Parametros [nXm, nXM, tXm, tXM]: nXmax, nXmin, tXmax, tXmin. X: Número de parámetro. n: Nivel. t: Tiempo.
	{ "test", "Verifica los salidas forzando un valor de entrada."					, cmd_statSystem },
//Parametros [PX val]: PX val, donde P=I/O, X=id puerto, val= valor del puerto.
	{ "time", "Modifica el calendario interno usado en la grabación de históricos."	, cmd_statSystem },
//Parametros [c DDMMAA], calendario día(DD), mes(MM), año(AA).t HHMMSS, tiempo hora(HH), minutos(MM), segundos(SS).
	{ "stop", "Detiene cualquier proceso bloqueante de la terminal."				, cmd_stopProcess },

};

static cmd_data_t data_arguments;

/************************************************************/
/**** Callback function for ntopt module ****/
static int usrcmd_ntopt_callback (int argc, char **argv, void *extobj)
{
	int i = 0;

    if (argc == 0) {
        return 0;
    }
    const cmd_table_t *p = &cmdlist[0];
    for (; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        if (ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            return p->func(argc, argv);
        }
        p++;
    }
    UART_SendStr("Unknown command found.\r\n");
    return 0;
    /*
    void func_cb_ntopt (int argc, char **argv)
    {
        int   execnt = 0;
        command_table_t *p = ptr_commandList;

        if (argc == 0) return;

        while (p->command != NULL)
        {
            if (strcmp (argv[0], p->command) == 0) {
                p->func(argc, argv);
                execnt++;
                break;
            }
            p++;
        }
        if (execnt == 0)
            UARTputs (&consola, "\n\rCommand not found.");
    }
    */
}

static int usrcmd_help (int argc, char **argv)
{
    const cmd_table_t *p = &cmdlist[0];
    int i = 0;
    for (; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
    	UART_SendStr(p->cmd);
    	UART_SendStr("\t:");
    	UART_SendStr(p->desc);
    	UART_SendStr("\r\n");
        p++;
    }
    return 0;
}

static int usrcmd_info (int argc, char **argv)
{
    if (argc != 2) {
    	UART_SendStr("info sys\r\n");
    	UART_SendStr("info ver\r\n");
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "sys") == 0) {
    	UART_SendStr("NXP LPC824 Monitor\r\n");
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "ver") == 0) {
    	UART_SendStr("Version 0.0.0\r\n");
        return 0;
    }
    UART_SendStr("Unknown sub command found\r\n");
    return -1;
}

static int cmd_statSystem (int argc, char **argv)
{
	//printEstado= true;
	return 0;
}

static int cmd_readLogs (int argc, char **argv)
{
	data_arguments.destino= CMD_TASK_N1;

    if( !ntlibc_strcmp( argv[1], "all" ) )
    {
    	data_arguments.arg1= 0xFFFF;
    	data_arguments.arg2= ntlibc_atoi( argv[2] );
    	UART_SendStr( "Reading logs registers...\r\n" );

    	DATA_PROCCESS_START();

		return 1;
    }
	return 0;
}

static int cmd_monInputs (int argc, char **argv)
{
	data_arguments.destino= CMD_TASK_N2;

    if( !ntlibc_strcmp( argv[1], "all" ) )
    {
    	data_arguments.arg1= 0xFFFF;
    	//data_arguments.arg2= ntlibc_atoi( argv[2] );
    	UART_SendStr( "Reading inputs ports...\r\n" );

    	DATA_PROCCESS_START();

		return 1;
    }
	return 0;
}
static int cmd_stopProcess (int argc, char **argv)
{
	// data_arguments.destino; Conserva el ultimo valor

	data_arguments.arg1= 0x0000;
	//data_arguments.arg2= ntlibc_atoi( argv[2] );
	//UART_SendStr( "Reading inputs ports...\r\n" );

	DATA_PROCCESS_START();

	return 1;
}
// pr

/******************************************************************/
static int checkandSet(char * val, int min, int max)
{
	int retval= 0, auxval;

	auxval=  atoi (val);
	if (auxval >= min && auxval <= max)
			retval= auxval;

	return retval;
}

/************************************************************/

int usrcmd_execute(const char *text)
{
    return ntopt_parse(text, usrcmd_ntopt_callback, 0);
}

int usrcmd_getStatus (cmd_data_t * ptrData)
{
	ptrData->destino= data_arguments.destino;
	ptrData->arg1= data_arguments.arg1;
	ptrData->arg2= data_arguments.arg2;
	ptrData->arg3= data_arguments.arg3;
	ptrData->arg4= data_arguments.arg4;
	ptrData->arg5= data_arguments.arg5;
	ptrData->arg6= data_arguments.arg6;

	return flag_data_proccess;
}

void usrcmd_setStatus (void)
{
	DATA_PROCCESS_END();
}

