/*
 * ciaaLED.c
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

//#include "ciaaGPIO_def.h"
#include "lpc_types.h"

#include "api_GPIO.h"
#include "ciaaPORT.h"

void ciaaLED_Init (void)
{
	uint8_t id;

	for ( id=LED_R; LED_3>=id; id++)
	{
		GPIO_EnablePin( pin_config[id].pinPort, pin_config[id].pinNumber, pin_config[id].function,
						pin_config[id].gpioPort, pin_config[id].gpioNumber, GPIO_OUT_MODE );

		//ciaaLED_Set( id, 0 );
	}
}

void ciaaLED_Set (uint8_t LEDNumber, bool stat)
{
	if ( LEDS_VALID(LEDNumber) )
	{
		GPIO_SetLevel( pin_config[LEDNumber].gpioPort, pin_config[LEDNumber].gpioNumber, stat );
	}
}

bool ciaaLED_Test (uint8_t LEDNumber)
{
	if ( LEDS_VALID(LEDNumber) )
	{
		return GPIO_GetLevel(  pin_config[LEDNumber].gpioPort, pin_config[LEDNumber].gpioNumber );
	}
	return false;
}

void ciaaLED_Toggle(uint8_t LEDNumber)
{
	if ( LEDS_VALID(LEDNumber) )
	{
		ciaaLED_Set( LEDNumber, !ciaaLED_Test( LEDNumber ) );
	}
}

