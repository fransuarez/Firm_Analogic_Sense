/*
 * auxiliar_gpios_def.h
 *
 *  Created on: 12/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_SENSADOANALOGICO_INC_AUXILIAR_GPIOS_DEF_H_
#define MODULES_SUPPORT_SENSADOANALOGICO_INC_AUXILIAR_GPIOS_DEF_H_

/*==================[macros]=================================================*/

#define FIRSTLED	   	2
#define SELECT_LED(A) (A+FIRSTLED)

/*==================[typedef]================================================*/

typedef enum
{
	DINP,
	DOUT,
	AINP,
	AOUT
} type_t;

typedef enum
{
	HIGH,
	LOW
} statPin_t;

typedef enum
{
	LED_1=1,
	LED_2=2,
	LED_3=4,
	LED_4=8
} led_t;

typedef enum
{
	TECL1=0,
	TECL2,
	TECL3,
	TECL4
} tec_t;


typedef struct
{
	uint8_t    led;
	uint16_t  *readVal;
} signal_t;

typedef struct
{
	tec_t      key;
	statPin_t  state;
	//TickType_t ticktimes;
} queue_t;

typedef enum
{
	MENOR11,
	MENOR21,
	MAYOR20
} LedOut;

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

#endif /* MODULES_SUPPORT_SENSADOANALOGICO_INC_AUXILIAR_GPIOS_DEF_H_ */
