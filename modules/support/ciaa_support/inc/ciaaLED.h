/*
 * ciaaLED.h
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAALED_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAALED_H_

#include "lpc_types.h"

/*==================[macros]=================================================*/
#define FIRSTLED	   2
#define SELECT_LED(A) (A+FIRSTLED)

#define LED_SET		TRUE
#define LED_RESET	FALSE

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
