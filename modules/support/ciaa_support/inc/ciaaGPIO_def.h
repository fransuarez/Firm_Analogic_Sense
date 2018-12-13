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

#define SENSOR_VALID(X) 	( (THERMISTOR<=X) && (WATER_LEVEL>=X) )
#define EXT_INPUTS_TOTAL	( SL_MODE_FUNCTION-TERMOCUPLE+1 )
#define EXT_OUTPUTS_TOTAL	( SIGNAL_MODE-RELAY_VALVE+1 )

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
	ALARM_OVER_TIME,

	SIGNAL_PROCCESS,
	SIGNAL_COMPLETE,
	SIGNAL_MODE,

// AUXILIARES *****************
	NONE=99,

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
