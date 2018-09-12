/*
 * auxiliar_gpios_def.h
 *
 *  Created on: 12/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_SENSADOANALOGICO_INC_AUXILIAR_GPIOS_DEF_H_
#define MODULES_SUPPORT_SENSADOANALOGICO_INC_AUXILIAR_GPIOS_DEF_H_

/*==================[macros]=================================================*/
#define ID_IRQ_PIN_INT0	0
#define ID_IRQ_PIN_INT1	1
#define ID_IRQ_PIN_INT2	2
#define ID_IRQ_PIN_INT3	3

#define ID_PORT_TEC1	0
#define ID_PORT_TEC2	0
#define ID_PORT_TEC3	0
#define ID_PORT_TEC4	1

#define ID_PIN_TEC1		4
#define ID_PIN_TEC2		8
#define ID_PIN_TEC3		9
#define ID_PIN_TEC4		9

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
