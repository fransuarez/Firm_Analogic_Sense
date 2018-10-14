/*
 * api_GPIO.h
 *
 *  Created on: Oct 11, 2018
 *      Author: fran
 */

#ifndef MODULES_API_GPIO_H_
#define MODULES_API_GPIO_H_

#include "lpc_types.h"

#define HI_LEVEL 1
#define LO_LEVEL 0

uint8_t GPIO_IRQHandler_CH0 ( void );
void 	GPIO_EnableIRQ_CH0  ( uint8_t, uint8_t );



#endif /* MODULES_API_GPIO_H_ */
