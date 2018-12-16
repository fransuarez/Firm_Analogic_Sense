/*
 * ciaaAPI.h
 *
 *  Created on: 13/9/2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_

//#include "chip.h"
//#include "lpc_types.h"
//#include "ciaaPORT.h"
#define GPIO_HIGH_LEVEL		1
#define GPIO_LOW_LEVEL		0
#define OUT_STATUS_SET		( GPIO_HIGH_LEVEL )
#define OUT_STATUS_RESET	( GPIO_LOW_LEVEL )


#define SENSOR_VALID(X) 	( (THERMISTOR<=X) && (WATER_LEVEL>=X) )
#define EXT_INPUTS_TOTAL	( SL_MODE_FUNCTION-TERMOCUPLE+1 )
#define EXT_OUTPUTS_TOTAL	( SIGNAL_MODE-RELAY_VALVE+1 )

#define EXT_INP_ANG_FIRST	( TERMOCUPLE )
#define EXT_INP_ANG_LAST	( CONDUCTIMETER )

#define EXT_INP_DIG_FIRST	( WATER_LEVEL )
#define EXT_INP_DIG_LAST	( SL_MODE_FUNCTION )

#define EXT_OUT_DIG_FIRST	( RELAY_VALVE )
#define EXT_OUT_DIG_LAST	( SIGNAL_MODE )
//#define EXT_OUT_ANG_FIRST	(  )
//#define EXT_OUT_ANG_LAST	(  )

// **********************************************************************
// No modificar el orden!!!!
typedef enum _external_objects_types
{
// ENTRADAS ********************
	TERMOCUPLE=0,
	THERMISTOR,
	AMPERIMETER,
	CONDUCTIMETER,

	WATER_LEVEL,
	SW_START_STOP,
	SL_OBJECT_DETECT,
	SL_MODE_FUNCTION,

// SALIDAS *********************
	RELAY_VALVE,
	RELAY_HEATER,

	ALARM_TEMPER,
	ALARM_LEVEL,
	ALARM_CONDUCT,
	ALARM_INTERRUPT,
	ALARM_TIME,

	SIGNAL_START,
	SIGNAL_COMPLETE,
	SIGNAL_STOP,
	SIGNAL_MODE,

// AUXILIARES *****************
	TIMER_ALARM_TEMP,
	TIMER_ALARM_LEVEL,
	TIMER_ALARM_CONDUCT,
	TIMER_ALARM_INTERRUPT,
	TIMER_ALARM_TIME,
	TIMER_PROCCESS_END,
	NONE,

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



#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAGPIO_DEF_H_ */
