/*
 * auxiliar_gpios_def.h
 *
 *  Created on: 12/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_SENSADOANALOGICO_
#define MODULES_SUPPORT_SENSADOANALOGICO_

#define REG_OPR_LOG	30	// Numero de registros por operacion de lgo.
#define TYPE_OFFSET 5
#define MASK_OFFSET 0x1F

#define GET_ID(X)	( MASK_OFFSET & X )
#define GET_TYPE(X)	( ~MASK_OFFSET & X )
/*==================[typedef]================================================*/
typedef uint8_t		statPin_t;
typedef uint8_t		type_t;
typedef uint8_t		cfgMode_t;


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
	inputConfig =(0<<TYPE_OFFSET),
	inputDigital=(1<<TYPE_OFFSET),
	inputAnalog =(2<<TYPE_OFFSET),
	inputTecl	=(3<<TYPE_OFFSET),
	outputDigital=(4<<TYPE_OFFSET),
	outputAnalog=(5<<TYPE_OFFSET),
	outputLed	=(6<<TYPE_OFFSET),
	outputConfig=(7<<TYPE_OFFSET)

} TypePort_t;

typedef enum _modos_configuracion
{
	confg_gpio,
	confg_max_min,
	confg_enable,
	confg_calibration

} modoCfg_t;

typedef struct _msg_type_aligned
{
	uint16_t 	size;
	uint16_t 	dummy;
	char* 		msg;
	//TickType_t ticktimes;
} genStr_t;

// Tamaño del registro 4 Bytes o 1 Word.
typedef struct _register_gpio
{
	uint16_t	offsetSec;
	uint16_t 	value; 	// Esto despues se puede optimizar y dejar solo 4 char/bytes que representen un uint32

} GpioReg_t;

typedef struct _configurer_gpio
{
	uint16_t 	auxvalue1;
	uint16_t 	auxvalue2;

} GpioConf_t;

typedef struct _data_log_pack
{
	uint8_t	 	cmd;
	uint8_t		nReg;
	uint8_t		minutes;
	uint8_t		seconds;
	GpioReg_t* 	data;
	//TickType_t ticktimes;
} dlogPack_t;

typedef struct _data_input_queue
{
	uint8_t		mode;
	uint8_t 	gpio;
	union
	{
		GpioConf_t  conf;
		GpioReg_t  	data;
	};

} dInputQueue_t;

typedef struct _data_output_queue
{
	uint8_t		mode;
	uint8_t 	gpio;
	GpioReg_t  	data;

} dOutputQueue_t;

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
