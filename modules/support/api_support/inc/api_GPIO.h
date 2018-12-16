/*
 * api_GPIO.h
 *
 *  Created on: Oct 11, 2018
 *      Author: fran
 */

#ifndef MODULES_API_GPIO_H_
#define MODULES_API_GPIO_H_

#include "lpc_types.h"

typedef struct type_gpio_input
{



} gpioIRQ_t;

#define IRQ_MODE_PULSADOR	1
#define IRQ_MODE_LEVEL		2

void 	GPIO_EnablePin  	(uint8_t pinPort, uint8_t pinNumber, uint8_t func, uint8_t gpioPort, uint8_t gpioNumber, uint8_t mode);
void 	GPIO_SetLevel 		(uint8_t gpioPort, uint8_t gpioPin, bool level);
bool 	GPIO_GetLevel 		(uint8_t gpioPort, uint8_t gpioPin);

void 	GPIO_InputIRQEnable  (uint8_t port, uint8_t pin, uint8_t chId, int mode);
void 	GPIO_InputIRQDisable (uint8_t chId);
uint8_t GPIO_InputIRQHandler (uint8_t chId);


#endif /* MODULES_API_GPIO_H_ */
