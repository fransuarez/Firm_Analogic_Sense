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
#include "FreeRTOSConfig.h"
//#include "Pulse_Count_Task.h"
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
//#include "sAPI.h"

#include "services_Shell.h"

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================[macros]=================================================*/
#define ID_IRQ_PIN_INT0	0
#define ID_IRQ_PIN_INT1	1
#define ID_IRQ_PIN_INT2	2
#define ID_IRQ_PIN_INT3	3

#define ID_PORT_TEC1	0
#define ID_PORT_TEC2	0
#define ID_PORT_TEC3	0
#define ID_PORT_TEC4	1

#define ID_PIN_TEC1		4
#define ID_PIN_TEC2		8
#define ID_PIN_TEC3		9
#define ID_PIN_TEC4		9

#define FIRSTLED	   	2

/*==================[typedef]================================================*/

typedef enum{
	HIGH,
	LOW
} statPin_t;

typedef enum{
	LED1=FIRSTLED,
	LED2,
	LED3,
	LED4
} led_t;

typedef enum{
	TECL1=0,
	TECL2,
	TECL3,
	TECL4
} tec_t;


typedef struct{
	led_t      led;
	TickType_t deltaT;
} signal_t;

typedef struct{
	tec_t      key;
	statPin_t  state;
	TickType_t ticktimes;
} queue_t;

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/** @brief main function
 * @return main function should never return
 */
int main(void);

/*==================[cplusplus]==============================================*/

#ifdef __cplusplus
}
#endif

/** @} doxygen end group definition */
/*==================[end of file]============================================*/
#endif /* #ifndef _MAIN_H_ */
