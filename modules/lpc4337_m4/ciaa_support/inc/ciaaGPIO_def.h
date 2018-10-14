/*
 * ciaaAPI.h
 *
 *  Created on: 13/9/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_

#include "chip.h"
//#include "lpc_types.h"

// Definiciones de Hardware:

#define PORT_TEC1		0
#define PORT_TEC2		0
#define PORT_TEC3		0
#define PORT_TEC4		1

#define PIN_TEC1		4
#define PIN_TEC2		8
#define PIN_TEC3		9
#define PIN_TEC4		9

#define PORT_LEDR		5
#define PORT_LEDG		5
#define PORT_LEDB		5
#define PORT_LED1		0
#define PORT_LED2		1
#define PORT_LED3		1

#define PIN_LEDR		0
#define PIN_LEDG		1
#define PIN_LEDB		2
#define PIN_LED1		14
#define PIN_LED2		11
#define PIN_LED3		12

typedef struct
{
	uint8_t port;
	uint8_t pin;

} io_port_t;


#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_ */
