/*
 * ciaaAPI.h
 *
 *  Created on: 13/9/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_

//#include "chip.h"
#include "lpc_types.h"

// Definiciones de Hardware:
// TECLAS ***************************************************
// Definiciones de PIN para el SCU:
#define PIN_PORT_TEC1		1
#define PIN_PORT_TEC2		1
#define PIN_PORT_TEC3		1
#define PIN_PORT_TEC4		1

#define PIN_NUMB_TEC1		0
#define PIN_NUMB_TEC2		1
#define PIN_NUMB_TEC3		2
#define PIN_NUMB_TEC4		6

// Definiciones de GPIO para api GPIO:
#define GPIO_PORT_TEC1		0
#define GPIO_PORT_TEC2		0
#define GPIO_PORT_TEC3		0
#define GPIO_PORT_TEC4		1

#define GPIO_NUMB_TEC1		4
#define GPIO_NUMB_TEC2		8
#define GPIO_NUMB_TEC3		9
#define GPIO_NUMB_TEC4		9

// LEDS ***************************************************
// Definiciones de PIN para el SCU:
#define PIN_PORT_LEDR		2
#define PIN_PORT_LEDG		2
#define PIN_PORT_LEDB		2
#define PIN_PORT_LED1		2
#define PIN_PORT_LED2		2
#define PIN_PORT_LED3		2

#define PIN_NUMB_LEDR		0
#define PIN_NUMB_LEDG		1
#define PIN_NUMB_LEDB		2
#define PIN_NUMB_LED1		10
#define PIN_NUMB_LED2		11
#define PIN_NUMB_LED3		12

// Definiciones de GPIO para api GPIO:
#define GPIO_PORT_LEDR		5
#define GPIO_PORT_LEDG		5
#define GPIO_PORT_LEDB		5
#define GPIO_PORT_LED1		0
#define GPIO_PORT_LED2		1
#define GPIO_PORT_LED3		1

#define GPIO_NUMB_LEDR		0
#define GPIO_NUMB_LEDG		1
#define GPIO_NUMB_LEDB		2
#define GPIO_NUMB_LED1		14
#define GPIO_NUMB_LED2		11
#define GPIO_NUMB_LED3		12

#define PIN_FUNCION_0		0
#define PIN_FUNCION_4		4
#define GPIO_OUT_MODE		1
#define GPIO_INP_MODE		0

typedef struct
{
	uint8_t pinPort;
	uint8_t pinNumber;

} io_port_t;

typedef struct
{
	uint8_t pinPort;
	uint8_t pinNumber;
	uint16_t function;
} dig_port_t;

#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_ */
