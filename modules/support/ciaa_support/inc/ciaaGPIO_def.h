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

#define ADC_INPUTS			( 3 )
#define ADC_REGISTERS		( 10 )

#define AIN_CONVER_ERR		( 0x01 )
#define AIN_NIVEL_MIN		( 0x02 )
#define AIN_NIVEL_MAX		( 0x04 )
#define AIN_NIVEL_VARIO		( 0x08 )
#define AIN_MASK_FLAGS		( 0x0F )

// * MACROS **********************************************************
#define TECL_VALID(X) 		( (TECL1<=X) && (TECL4>=X) )
#define LEDS_VALID(X) 		( (LED_R<=X) && (LED_3>=X) )
#define AINP_VALID(X) 		( (AIN_1<=X) && (AIN_3>=X) )
#define LEDS_INDEX(X)		( X-LED_R )
#define TECL_INDEX(X)		( X-TECL1 )
#define AINP_INDEX(X)		( X-AIN_1 )
#define SENSOR_VALID(X) 	( (THERMISTOR<=X) && (WATER_LEVEL>=X) )

#define BIT_ON_REG4(X,Y,Z)  ( X |= (Z<<(Y*4)) )
#define BIT_OFF_REG4(X,Y,Z) ( X &= ~(Z<<(Y*4)) )
#define BIT_SEL_REG4(X,Y)	( X = (X>>(Y*4)) & AIN_MASK_FLAGS )

#define AINP_SET_ERR(X,Y) 	( BIT_ON_REG4( X,Y,AIN_CONVER_ERR ))
#define AINP_SET_MIN(X,Y) 	( BIT_ON_REG4( X,Y,AIN_NIVEL_MIN ))
#define AINP_SET_MAX(X,Y)	( BIT_ON_REG4( X,Y,AIN_NIVEL_MAX ))
#define AINP_SET_VAR(X,Y)	( BIT_ON_REG4( X,Y,AIN_NIVEL_VARIO ))

#define AINP_RESET_ERR(X,Y) ( BIT_OFF_REG4( X,Y,AIN_CONVER_ERR ))
#define AINP_RESET_MIN(X,Y) ( BIT_OFF_REG4( X,Y,AIN_NIVEL_MIN ))
#define AINP_RESET_MAX(X,Y)	( BIT_OFF_REG4( X,Y,AIN_NIVEL_MAX ))
#define AINP_RESET_VAR(X,Y)	( BIT_OFF_REG4( X,Y,AIN_NIVEL_VARIO ))

#define AINP_GET_ERR(X,Y) 	( BIT_SEL_REG4(X,Y) & AIN_CONVER_ERR )
#define AINP_GET_MIN(X,Y) 	( BIT_SEL_REG4(X,Y) & AIN_NIVEL_MIN )
#define AINP_GET_MAX(X,Y) 	( BIT_SEL_REG4(X,Y) & AIN_NIVEL_MAX )
#define AINP_GET_VAR(X,Y) 	( BIT_SEL_REG4(X,Y) & AIN_NIVEL_VARIO )
#define AINP_GET_ALL(X,Y) 	( BIT_SEL_REG4(X,Y) )


// **********************************************************************
typedef enum _perifericos_edu_ciaa
{
	LED_R= 0,
	LED_G,
	LED_B,
	LED_1,
	LED_2,
	LED_3,

	TECL1,
	TECL2,
	TECL3,
	TECL4,
	TECL1_4,

	AIN_1,
	AIN_2,
	AIN_3,

	DIN_0,
	DIN_1,
	DIN_2,
	DIN_3,
	DIN_4,
	DIN_5,
	DIN_6,
	DIN_7,
	DIN_8

} perif_t; //size= 1 byte

// No modificar el orden!!!!
typedef enum _external_objects_types
{
	TERMOCUPLE=0,
	THERMISTOR,
	AMPERIMETER,
	CONDUCTIMETER,
	WATER_LEVEL,

	SW_START_STOP,
	SW_INTERRUPT,

	SL_OBJECT_DETECT,
	SL_MODE_FUNCTION,

	// TODO Falta definir los objetos de salida!!!!
	EXT_INPUTS_TOTAL

} externId_t;

typedef enum _unit_types
{
	mVOLTS=0,
	uVOLTS,
	OHMS,
	mOHMS,
	mFARADS,
	uFARADS,
	cMETERS,
	mMETERS,
	CELSIUS,
	AMPERS,
	mAMPERS,
	mgSQRMTR,
	LEVEL

} unit_t; //size= 1 byte

typedef enum _ciaa_AIN_ajustes
{
	ain_cotas_conversion,
	ain_calibracion,
	ain_habilitacion,
	ain_reasignacion

} ainCfg_t;

typedef enum _tec_stat
{
	TECL_RELEASE = 0x00,
	TEC1_PRESSED = 0x01,
	TEC2_PRESSED = 0x02,
	TEC3_PRESSED = 0x04,
	TEC4_PRESSED = 0x08,

} tecReg_t;

typedef struct _report_state
{
	//uint8_t 	change;
	externId_t	name;  // Es tamaño byte???
	uint8_t 	enable;
	uint8_t 	dummy;
	unit_t  	unit;
	int32_t 	value;

} repAnStat_t;

typedef struct
{
	uint8_t 	pinPort;
	uint8_t 	pinNumber;

} io_port_t;

typedef struct
{
	uint8_t 	pinPort;
	uint8_t 	pinNumber;
	uint16_t 	function;

} dig_port_t;

#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_ */
