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
#include <stdio.h>
#include "ciaaAIN.h"


/*==================[macros and definitions]=================================*/
#define MAX_UINT32  (2^32)-1

#define TWAIT	   	500
#define PERIODO	  	1000
#define NUM_TASK	3

xTaskHandle pxCreatedTask1;
xTaskHandle pxCreatedTask2;
xTaskHandle pxCreatedTask3;
/*==================[internal data declaration]==============================*/
xQueueHandle queueKeyPad;
xQueueHandle queueSigLed;
xSemaphoreHandle mutexConsola;

// Arreglo con el stack disponible de las tareas creadas en main
unsigned long stacktareas[NUM_TASK];
unsigned long *ptrstack= &stacktareas[0];
/*==================[internal functions declaration]=========================*/
static void taskControlLed(void * a)
{
	signal_t dataRecLed;
	//int32_t  tOnLeds [4];
	char sToSend[30]="Aun esta vacio";
	uint8_t  i, aux;

	while (1)
	{
		*(ptrstack+0)= uxTaskGetStackHighWaterMark( pxCreatedTask1 );

		if( pdTRUE == xQueueReceive( queueSigLed, &dataRecLed, TWAIT ))
		{
			for(i=0; i<4; i++)
			{
				aux= (1<<i);
				aux &= dataRecLed.led;
				if( aux )
				{
					Board_LED_Set( SELECT_LED(i), true );

					sprintf(sToSend, "[AI%d = %i uni]\r\n", i, dataRecLed.readVal[i]);
					printConsola(sToSend, MP_DEB);
				}
			}
		}
		else
		{
			for(i=0; i<4; i++)
			{
				Board_LED_Set(SELECT_LED(i), false );
			}
		}
	}
}

static void taskDetectPulse(void * a)
{
	queue_t dataRecKey;
	signal_t dataToSend;
	uint16_t valAnalogicInp [4];
	//TickType_t tOnKeys [4];

	dataToSend.led= (LED_1 | LED_2 | LED_3);
	dataToSend.readVal= valAnalogicInp;

	while (1)
	{
		*(ptrstack+1)= uxTaskGetStackHighWaterMark( pxCreatedTask2 );

		if( pdTRUE == xQueueReceive( queueKeyPad, &dataRecKey, portMAX_DELAY ))
		{
			// leo el termistor:
			valAnalogicInp[0]= ciaaAINRead(0);

			// leo la termocupla:
			valAnalogicInp[1]= ciaaAINRead(1);

			// leo el auxiliar 1:
			valAnalogicInp[2]= ciaaAINRead(2);

			if( LOW == dataRecKey.state )
			{
				xQueueSend(queueSigLed, &dataToSend, portMAX_DELAY);
			}
		}

	}
}

/*==================[external functions definition]==========================*/
static void initHardware(void)
{
    SystemCoreClockUpdate();

    Board_Init();

    ciaaAINInit();

    //Board_Buttons_Init();
    Chip_PININT_Init( LPC_GPIO_PIN_INT );
    Chip_SCU_GPIOIntPinSel( ID_IRQ_PIN_INT0, ID_PORT_TEC1, ID_PIN_TEC1 );
    //Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH0);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH0 );
	/* Set lowest priority for RIT */
	NVIC_SetPriority(PIN_INT0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

	/* Enable IRQ for RIT */
	NVIC_EnableIRQ(PIN_INT0_IRQn);
}

int main(void)
{
	if(xTaskCreate( taskControlLed,(const char *) "Tarea de control de leds por cola.",
				   (configMINIMAL_STACK_SIZE*2), NULL, tskIDLE_PRIORITY+2, &pxCreatedTask1 ) !=pdPASS)
		// Con esta cantidad de stack le quedan libres 101 Bytes. Le sobran < 1*STACK_SIZE= 128
		while(1);

	if(xTaskCreate( taskDetectPulse,(const char *) "Tarea deteccion de flancos de teclas.",
				   (configMINIMAL_STACK_SIZE), NULL, tskIDLE_PRIORITY+1, &pxCreatedTask2 )!=pdPASS)
		// Con esta cantidad de stack le quedan libres 79 Bytes. Le sobran < 1*STACK_SIZE =128
		while(1);

	if(xTaskCreate( taskConsola, (const char *) "Tarea control de comunicacion con terminal.",
				   (configMINIMAL_STACK_SIZE*4), NULL, tskIDLE_PRIORITY+1, &pxCreatedTask3 )!=pdPASS)
		// Con esta cantidad de stack le quedan libres 119 Bytes x ahora. Le sobran < 1*STACK_SIZE =128
		while(1);

	queueKeyPad= xQueueCreate(2, sizeof(queue_t));
	queueSigLed= xQueueCreate(2, sizeof(signal_t));
	mutexConsola = xSemaphoreCreateMutex();

	initHardware();
	vTaskStartScheduler();

	while (1) {
	}
}


/*==================[irq handlers functions ]=========================*/

void GPIO0_IRQHandler(void)
{
	uint32_t countFall, countRise;
	queue_t dataToSend;
	portBASE_TYPE xSwitchRequired;

	countFall= Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT);
	if( PININTCH0 == countFall ) {
		dataToSend.key= TECL1;
		dataToSend.state= HIGH;
		dataToSend.ticktimes= 0;
		//xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT, countFall);
	}
	countRise= Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT);
	if( PININTCH0 == countRise) {
		dataToSend.key= TECL1;
		dataToSend.state= LOW;
		dataToSend.ticktimes= 0;
		xQueueSendFromISR(queueKeyPad, &dataToSend, &xSwitchRequired);

		Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT, countRise);
	}
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);

	portEND_SWITCHING_ISR( xSwitchRequired );
}

void ADC0_IRQHandler(void)
{
	//portBASE_TYPE xSwitchRequired;
	ADC0_IRQHandler_Suport();
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	//portEND_SWITCHING_ISR( xSwitchRequired );
}
//void* _sbrk() { return 0; }
/** @} doxygen end group definition */

/*==================[end of file]============================================*/
