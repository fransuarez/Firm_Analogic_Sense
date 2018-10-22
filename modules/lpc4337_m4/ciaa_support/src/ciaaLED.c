/*
 * ciaaLED.c
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#include "ciaaGPIO_def.h"
#include "api_GPIO.h"

#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_LED3           0x04
#define LEDS_LED4           0x08
#define LEDS_NO_LEDS        0x00
#define LEDS_TOTAL 		    6


static const io_port_t gpioLEDBits[LEDS_TOTAL] =
{
	{GPIO_PORT_LEDR, GPIO_NUMB_LEDR},  // 0: Led Rojo
	{GPIO_PORT_LEDG, GPIO_NUMB_LEDG},  // 1: Led Verde
	{GPIO_PORT_LEDB, GPIO_NUMB_LEDB},  // 2: Led Azul
	{GPIO_PORT_LED1, GPIO_NUMB_LED1},  // 3: Led 1
	{GPIO_PORT_LED2, GPIO_NUMB_LED2},  // 4: Led 2
	{GPIO_PORT_LED3, GPIO_NUMB_LED3}   // 5: Led 3
};

void ciaaLED_Init (void)
{
	uint8_t i;
	const dig_port_t pinConfig[LEDS_TOTAL]=
	{
		{PIN_PORT_LEDR, PIN_NUMB_LEDR, PIN_FUNCION_4},
		{PIN_PORT_LEDG, PIN_NUMB_LEDG, PIN_FUNCION_4},
		{PIN_PORT_LEDB, PIN_NUMB_LEDB, PIN_FUNCION_4},
		{PIN_PORT_LED1, PIN_NUMB_LED1, PIN_FUNCION_0},
		{PIN_PORT_LED2, PIN_NUMB_LED2, PIN_FUNCION_0},
		{PIN_PORT_LED3, PIN_NUMB_LED3, PIN_FUNCION_0},
	};

	for ( i = 0; i < LEDS_TOTAL; i++)
	{
		GPIO_EnablePin( pinConfig[i].pinPort, pinConfig[i].pinNumber, pinConfig[i].function,
						gpioLEDBits[i].pinPort, gpioLEDBits[i].pinNumber, GPIO_OUT_MODE );
	}
}

void ciaaLED_Set (uint8_t LEDNumber, bool stat)
{
	if (LEDNumber < LEDS_TOTAL )
	{
		GPIO_SetLevel( gpioLEDBits[LEDNumber].pinPort, gpioLEDBits[LEDNumber].pinNumber, stat );
	}
}

bool ciaaLED_Test (uint8_t LEDNumber)
{
	if (LEDNumber < LEDS_TOTAL )
	{
		return GPIO_GetLevel( gpioLEDBits[LEDNumber].pinPort, gpioLEDBits[LEDNumber].pinNumber );
	}
	return false;
}

void ciaaLED_Toggle(uint8_t LEDNumber)
{
	ciaaLED_Set(LEDNumber, !ciaaLED_Test(LEDNumber));
}
