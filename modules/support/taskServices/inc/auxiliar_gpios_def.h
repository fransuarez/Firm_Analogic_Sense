/*
 * auxiliar_gpios_def.h
 *
 *  Created on: 12/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_SENSADOANALOGICO_
#define MODULES_SUPPORT_SENSADOANALOGICO_

#define REG_OPR_LOG	30	// Numero de registros por operacion de lgo.

/*==================[typedef]================================================*/
typedef uint8_t		statPin_t;
typedef uint8_t		type_t;

typedef enum _type_log
{
	writeByte,
	writeWord,
	writePage,
	readByte,
	readWord,
	readPage
} TypeLog_t;

typedef enum _type_port
{
	inputDigital,
	inputAnalog,
	inputTecl,
	outputDigital,
	outputAnalog,
	outputLed

} TypePort_t;

typedef struct
{
	uint16_t 	size;
	uint16_t 	dummy;
	char* 		msg;
	//TickType_t ticktimes;
} genStr_t;


typedef struct _data_regi
{
	type_t 		type;
	uint8_t 	id;
	uint16_t 	value; 	// Esto despues se puede optimizar y dejar solo 4 char/bytes que representen un uint32
} GpioReg_t;			// Tamaño del registro 4 Bytes o 1 Word.

typedef struct
{
	uint8_t	 	cmd;
	uint8_t		nReg;
	uint16_t	dummy;
	GpioReg_t* 	data;
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
