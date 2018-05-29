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
#include "ciaaUART.h"
#include <stdlib.h>

//extern sch_task_t schedTask[LENG_TASK];
//extern xSemaphoreHandle mutexSheduler;
//extern bool	printEstado;

//static char	aux_data_buff[BUFF_SIZE];


static int cmd_statSystem 		( int argc, char **argv );
static int usrcmd_help			( int argc, char **argv );
static int usrcmd_info			( int argc, char **argv );

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int checkandSet(char * val, int min, int max);

typedef int (*USRCMDFUNC)(int argc, char **argv);

typedef struct {
    char *cmd;
    char *desc;
    USRCMDFUNC func;
} cmd_table_t;

static const cmd_table_t cmdlist[] = {
    { "help", "This is a description text string for help command.", usrcmd_help },
    { "info", "This is a description text string for info command.", usrcmd_info },
	{ "stat"	, "Muestra el estado general del sistema"	, cmd_statSystem 		},
};

int usrcmd_execute(const char *text)
{
    return ntopt_parse(text, usrcmd_ntopt_callback, 0);
}

/**** Callback function for ntopt module ****/
static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    if (argc == 0) {
        return 0;
    }
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
        if (ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            return p->func(argc, argv);
        }
        p++;
    }
    uartSendStr("Unknown command found.\r\n");
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

static int usrcmd_help(int argc, char **argv)
{
    const cmd_table_t *p = &cmdlist[0];
    for (int i = 0; i < sizeof(cmdlist) / sizeof(cmdlist[0]); i++) {
    	uartSendStr(p->cmd);
    	uartSendStr("\t:");
    	uartSendStr(p->desc);
    	uartSendStr("\r\n");
        p++;
    }
    return 0;
}

static int usrcmd_info(int argc, char **argv)
{
    if (argc != 2) {
    	uartSendStr("info sys\r\n");
    	uartSendStr("info ver\r\n");
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "sys") == 0) {
    	uartSendStr("NXP LPC824 Monitor\r\n");
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "ver") == 0) {
    	uartSendStr("Version 0.0.0\r\n");
        return 0;
    }
    uartSendStr("Unknown sub command found\r\n");
    return -1;
}


/************************************************************/

static int cmd_statSystem 	( int argc, char **argv )
{
	//printEstado= true;
	return 0;
}

/******************************************************************/
static int checkandSet(char * val, int min, int max)
{
	int retval= 0, auxval;

	auxval=  atoi (val);
	if (auxval >= min && auxval <= max)
			retval= auxval;

	return retval;
}

