/*
 * auxiliar_gpios_def.h
 *
 *  Created on: 12/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_SENSADOANALOGICO_
#define MODULES_SUPPORT_SENSADOANALOGICO_

/*==================[macros]=================================================*/

#define FIRSTLED	   	2
#define SELECT_LED(A) (A+FIRSTLED)

/*==================[typedef]================================================*/
typedef uint8_t	led_t;
typedef uint8_t	statPin_t;
typedef uint8_t	tec_t;
typedef uint8_t	type_t;

typedef struct
{
	led_t     led;
	uint16_t  *readVal;
} signal_t;

typedef struct
{
	tec_t      key;
	statPin_t  state;
	//TickType_t ticktimes;
} queue_t;


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
