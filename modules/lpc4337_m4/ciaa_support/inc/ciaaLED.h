/*
 * ciaaLED.h
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAALED_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAALED_H_

#include "lpc_types.h"

typedef enum
{
	LED_1=1,
	LED_2=2,
	LED_3=4,
	LED_4=8

} leds_t;

void ciaaLED_Init 	(void);

// API LED
/**
 * @brief	Sets the state of a board LED to on or off
 * @param	LEDNumber	: LED number to set state for
 * @param	State		: true for on, false for off
 * @return	None
 */
void ciaaLED_Set(uint8_t LEDNumber, bool State);

/**
 * @brief	Returns the current state of a board LED
 * @param	LEDNumber	: LED number to set state for
 * @return	true if the LED is on, otherwise false
 */
bool ciaaLED_Test(uint8_t LEDNumber);

/**
 * @brief	Toggles the current state of a board LED
 * @param	LEDNumber	: LED number to change state for
 * @return	None
 */
void ciaaLED_Toggle(uint8_t LEDNumber);


#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAALED_H_ */
