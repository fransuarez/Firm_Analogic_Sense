/*
 * ciaaLED.h
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAALED_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAALED_H_

// DEFINICIONES GENERALES
#define LED1_GPIO_PORT_NUM              1
#define LED1_GPIO_BIT_NUM               11
#define LED2_GPIO_PORT_NUM              1
#define LED2_GPIO_BIT_NUM               12

#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_LED3           0x04
#define LEDS_LED4           0x08
#define LEDS_NO_LEDS        0x00

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
