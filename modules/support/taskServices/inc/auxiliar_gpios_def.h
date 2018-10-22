/*
 * auxiliar_gpios_def.h
 *
 *  Created on: 12/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_SENSADOANALOGICO_
#define MODULES_SUPPORT_SENSADOANALOGICO_

/*==================[typedef]================================================*/
typedef uint16_t	led_t;
typedef uint8_t		statPin_t;
typedef uint8_t		tec_t;
typedef uint8_t		type_t;

typedef struct
{
	led_t      led;
	uint16_t  *readVal;

} ledStat_t;

typedef struct
{
	tec_t       key;
	statPin_t   state;
	uint16_t 	dummy;
	//TickType_t ticktimes;
} tecStat_t;

typedef struct
{
	uint16_t 	size;
	uint8_t 	mode;
	uint8_t 	cmdShell;
	char* 		msg;
	//TickType_t ticktimes;
} terMsg_t;

typedef struct
{
	uint16_t 	size;
	uint16_t 	dummy;
	char* 		msg;
	//TickType_t ticktimes;
} dlogPack_t;

/*

typedef struct GPIO_digital
{
	statPin_t state_last;
	statPin_t state_prew;
	union
	{
		tec_t input;
		led_t output;
	};

} gpio_digital_t;

typedef struct GPIO_values
{
	type_t 	clase;
	bool 	enable;
	bool	error;

} gpio_values_t;

*/
#endif /* MODULES_SUPPORT_SENSADOANALOGICO_ */
