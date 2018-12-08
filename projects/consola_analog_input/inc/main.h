/* Copyright 2015, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of lpc1769_template.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _MAIN_H_
#define _MAIN_H_

/** \addtogroup rtos_blink FreeRTOS blink example
 ** @{ */

/*==================[inclusions]=============================================*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "terminalManager.h"
#include "filesManager.h"
#include "inputManager.h"
#include "outputManager.h"


// -------------------- Definiciones tarea 1  ---------------------
#define  TASK_N1				taskControlOutputs
#define  TASK_N1_DECRIPT		"DIGITAL Outputs Driver Task."
// Con esta cantidad de stack le quedan libres 101 Bytes. Le sobran < 1*STACK_SIZE= 128
#define  TASK_N1_STACK			(configMINIMAL_STACK_SIZE*2)
#define  TASK_N1_PRIORITY		tskIDLE_PRIORITY+2

// -------------------- Definiciones tarea 2  ---------------------
#define  TASK_N2				taskControlInputs
#define  TASK_N2_DECRIPT		"DIG/AN Inputs Driver Task."
// Con esta cantidad de stack le quedan libres 79 Bytes. Le sobran < 1*STACK_SIZE =128
#define  TASK_N2_STACK			(configMINIMAL_STACK_SIZE*2)
#define  TASK_N2_PRIORITY		tskIDLE_PRIORITY+2

// -------------------- Definiciones tarea 3  ---------------------
#define  TASK_N3				taskTerminal
#define  TASK_N3_DECRIPT		"Terminal Shell Task."
// Con esta cantidad de stack le quedan libres 119 Bytes x ahora. Le sobran < 1*STACK_SIZE =128
#define  TASK_N3_STACK			(configMINIMAL_STACK_SIZE*4)
#define  TASK_N3_PRIORITY		tskIDLE_PRIORITY+1

// -------------------- Definiciones tarea 4  ---------------------
#define  TASK_N4				dataLog_Service
#define  TASK_N4_DECRIPT		"Logs Eeprom Driver Task."
// Con esta cantidad de stack le quedan libres 119 Bytes x ahora. Le sobran < 1*STACK_SIZE =128
#define  TASK_N4_STACK			(configMINIMAL_STACK_SIZE*2)
#define  TASK_N4_PRIORITY		tskIDLE_PRIORITY+1

// -------------------- Definiciones timers  ---------------------

#define  TIMER_1_NAME			"Envio Reportes Timer"
#define  TIMER_1_PER			( 1000 / portTICK_PERIOD_MS )
#define  TIMER_1_RELOAD			pdTRUE
#define  TIMER_1_IDn			(void *) 1
#define  TIMER_1_CALLBACK		timerCallbackReport

#define  TIMER_2_NAME			"AnalogInputs Update"
#define  TIMER_2_PER			( 3000 / portTICK_PERIOD_MS )
#define  TIMER_2_RELOAD			pdTRUE
#define  TIMER_2_IDn			(void *) 2
#define  TIMER_2_CALLBACK		timerCallbackAnalog

#define  TIMER_3_NAME			"Desactiva debounce teclas"
#define  TIMER_3_PER			( 10000 / portTICK_PERIOD_MS )
#define  TIMER_3_RELOAD			pdFALSE
#define  TIMER_3_IDn			(void *) 3
#define  TIMER_3_CALLBACK		timerCallbackTeclas

#define  TIMER_4_NAME			"Sample debounce teclas"
#define  TIMER_4_PER			( 200 / portTICK_PERIOD_MS )
#define  TIMER_4_RELOAD			pdTRUE
#define  TIMER_4_IDn			(void *) 4
#define  TIMER_4_CALLBACK		timerCallbackDebounce

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

int tasks_create (void);
int queues_create (void);
int timers_create (void);


/*==================[external data declaration]==============================*/
/*==================[external functions declaration]=========================*/

/** @brief main function
 * @return main function should never return
 */
//int main(void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */
