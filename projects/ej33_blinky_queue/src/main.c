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

/** @brief Blinky using FreeRTOS.
 *
 *
 * NOTE: It's interesting to check behavior differences between standard and
 * tickless mode. Set @ref configUSE_TICKLESS_IDLE to 1, increment a counter
 * in @ref vApplicationTickHook and print the counter value every second
 * inside a task. In standard mode the counter will have a value around 1000.
 * In tickless mode, it will be around 25.
 *
 */

/** \addtogroup rtos_blink FreeRTOS blink example
 ** @{ */

/*==================[inclusions]=============================================*/

#include "main.h"

#include "FreeRTOSConfig.h"
#include "Pulse_Count_Task.h"
#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



/*==================[macros and definitions]=================================*/
#define MAX_UINT32  (2^32)-1
/*==================[internal data declaration]==============================*/
typedef enum{
	UNPRESS,
	PRESS,
	RELEASE
} statCount_t;

typedef struct{
	TickType_t  deltaTint;
	uint8_t		led;
} dataQ;

statCount_t state= UNPRESS;
TickType_t deltaT=0;
xQueueHandle queueKeyPad;

/*==================[internal functions declaration]=========================*/

/** @brief hardware initialization function
 *	@return none
 */
static void initHardware(void);

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)
{
    SystemCoreClockUpdate();

    Board_Init();
    Board_Buttons_Init();
    PULSE_COUNT_TASK_Init();
    Board_LED_Set(3, false);
    Board_LED_Set(4, false);
}
#define TWAIT	   20
#define PERIODO	 1000
#define LED_R		0
#define LED_1		3
#define LED_2		4


static void taskControlLed(void * a)
{
	 //* @return pdTRUE if an item was successfully received from the queue,
	 //* otherwise pdFALSE.
	dataQ infoLed;
	TickType_t deltaT=1;
	uint8_t led=LED_1;

	while (1)
	{
		if(pdTRUE == xQueueReceive( queueKeyPad, &infoLed, PERIODO )){
			deltaT= infoLed.deltaTint;
			led= infoLed.led;
		}
		Board_LED_Toggle(led);
		vTaskDelay(deltaT);
		Board_LED_Toggle(led);

		vTaskDelay(TWAIT);
	}
}

static void taskDetectPulse(void * a)
{
	TickType_t tStart, tEnd;
	dataQ infoLed;

	while (1)
	{
		switch(state) {
		case UNPRESS:
			if(1 == PULSE_COUNT_TASK_UpdateFallings()){
				tStart= xTaskGetTickCount();
				deltaT=0;
				state= PRESS;
			}
			break;
		case PRESS:
			if(2 == PULSE_COUNT_TASK_UpdateFallings()){
				tEnd= xTaskGetTickCount();
				state= RELEASE;
			}
			else{
				Board_LED_Toggle(LED_R);
			}
			break;
		case RELEASE:
			if(tEnd>tStart)
				deltaT= tEnd-tStart;
			else {
				tStart= (MAX_UINT32)-tStart;
				deltaT= tEnd+tStart;
			}
			Board_LED_Set(LED_R, false);
			state= UNPRESS;

			infoLed.deltaTint= deltaT;
			infoLed.led= LED_2;
			xQueueSend( queueKeyPad, &infoLed, portMAX_DELAY );
			break;
		}
		vTaskDelay(TWAIT);
	}
}

/*==================[external functions definition]==========================*/

int main(void)
{
	initHardware();

	xTaskCreate(taskControlLed, (const char *)"taskControlLed", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
	xTaskCreate(taskDetectPulse, (const char *)"taskDetectPulse", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
	queueKeyPad= xQueueCreate(1, sizeof(dataQ));

	vTaskStartScheduler();

	while (1) {
	}
}

/** @} doxygen end group definition */

/*==================[end of file]============================================*/
