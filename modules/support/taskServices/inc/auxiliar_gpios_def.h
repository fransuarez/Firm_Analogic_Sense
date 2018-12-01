/*
 * auxiliar_gpios_def.h
 *
 *  Created on: 12/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_SENSADOANALOGICO_
#define MODULES_SUPPORT_SENSADOANALOGICO_

#define REG_OPR_LOG	32	// Numero de registros por operacion de log. Equivale a una pagina de memoria con 32 words.
#define TYPE_OFFSET 5	//(1<<5)
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

} TypeLog_t;  //size= 1 byte

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

} TypePort_t; //size= 1 byte

typedef enum _modos_configuracion
{
	confg_gpio,
	confg_max_min,
	confg_enable,
	confg_calibration

} modoCfg_t;

typedef struct reg_sample_time
{
	uint8_t 	minutes;
	uint8_t		seconds;

} shTime_t; //size= 2 bytes

typedef struct _msg_type_aligned
{
	uint16_t 	size;
	uint16_t 	dummy;
	char* 		msg;
	//TickType_t ticktimes;
} genStr_t;

typedef struct _configurer_gpio
{
	uint16_t 	auxvalue1;
	uint16_t 	auxvalue2;

} GpioConf_t;

#define SIZE_BYTES_GPIO_REG		4
// Tamaño del registro 4 Bytes o 1 Word.
typedef struct _register_gpio
{
	uint8_t 	name;
	uint8_t		secTime;
	uint16_t 	value; 	// Esto despues se puede optimizar y dejar solo 4 char/bytes que representen un uint32

} GpioReg_t;

typedef struct _data_log_pack
{
	uint8_t	 	cmd;
	uint8_t		nReg;
	uint8_t		hourSamples;
	uint8_t		minuSamples;
	GpioReg_t* 	data;
	//TickType_t ticktimes;
} dlogPack_t;

typedef struct _data_input_queue
{
	uint8_t		mode; // es del tipo TypePort_t -> ver que tamaño tiene en bytes sino lo fuerzo a uint8_t
	uint8_t		gpio; // es del tipo perif_t pero como esta definidio en otro modulo lo fuerzo a uint8_t.
	shTime_t	sTime;
	union
	{
		GpioReg_t  	data;
		GpioConf_t  conf;
	};

} dInOutQueue_t; //size= 8 bytes

typedef dInOutQueue_t dOutputQueue_t;
typedef dInOutQueue_t dInputQueue_t;


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
