/*
 * ciaaRTC.c
 *
 *  Created on: 13/9/2018
 *      Author: fran
 */

#include "chip.h"
#include "api_RTC.h"
// Despues debo sacar estas funciones de aca lo dejo por ahora por comodidad.
//================================================[RTC Management]========================================================
static void (*rtcCallback) (void*);
static void* rtcCallbackArg;

uint8_t rtc_status=0;
static RTC_t ctRTC =
{
	.mday = 1,
	.month= 1,
	.year = 2018,
	.wday = 1,
	.hour = 0,
	.min  = 0,
	.sec  = 0
};

void RTC_Init( void )
{
	if( !RTC_GET_INIT() )
	{
		Chip_RTC_Init(LPC_RTC);
		rtcCallback=NULL;

		Chip_RTC_CntIncrIntConfig(LPC_RTC,0, DISABLE);

		NVIC_EnableIRQ( RTC_IRQn );
		RTC_SET_INIT();
	}
}

void RTC_setTime( RTC_t * pRTC )
{
        RTC_TIME_T rtc;
        /*
        static bool_t init;
        if( init ){
           //Already initialized
           ret_val = 0;
        } else {
        */
        rtc.time[RTC_TIMETYPE_SECOND] = pRTC->sec;
        rtc.time[RTC_TIMETYPE_MINUTE] = pRTC->min;
        rtc.time[RTC_TIMETYPE_HOUR] = pRTC->hour;
        rtc.time[RTC_TIMETYPE_DAYOFMONTH] = pRTC->mday;
        rtc.time[RTC_TIMETYPE_MONTH] = pRTC->month;
        rtc.time[RTC_TIMETYPE_YEAR] = pRTC->year;
        rtc.time[RTC_TIMETYPE_DAYOFWEEK] = pRTC->wday;

        Chip_RTC_SetFullTime(LPC_RTC, &rtc);
        Chip_RTC_Enable(LPC_RTC, ENABLE);
        RTC_SET_IN_TIMED();
        //}
}

void RTC_getTime( RTC_t * pRTC )
{
	RTC_TIME_T rtc;
	if( RTC_GET_INIT() )
	{
		Chip_RTC_GetFullTime(LPC_RTC, &rtc);
	}

	ctRTC.sec = rtc.time[RTC_TIMETYPE_SECOND];
	ctRTC.min = rtc.time[RTC_TIMETYPE_MINUTE];
	ctRTC.hour = rtc.time[RTC_TIMETYPE_HOUR];
	ctRTC.mday = rtc.time[RTC_TIMETYPE_DAYOFMONTH];
	ctRTC.month = rtc.time[RTC_TIMETYPE_MONTH];
	ctRTC.year = rtc.time[RTC_TIMETYPE_YEAR];
	ctRTC.wday = rtc.time[RTC_TIMETYPE_DAYOFWEEK];

	*pRTC= ctRTC;
}

void RTC_setAlarmTime( RTC_t * pRTC, uint32_t alarmMask )
{
        RTC_TIME_T rtc;
        rtc.time[RTC_TIMETYPE_SECOND] 	 = pRTC->sec;
        rtc.time[RTC_TIMETYPE_MINUTE] 	 = pRTC->min;
        rtc.time[RTC_TIMETYPE_HOUR] 	 = pRTC->hour;
        rtc.time[RTC_TIMETYPE_DAYOFMONTH]= pRTC->mday;
        rtc.time[RTC_TIMETYPE_MONTH] 	 = pRTC->month;
        rtc.time[RTC_TIMETYPE_YEAR] 	 = pRTC->year;
        rtc.time[RTC_TIMETYPE_DAYOFWEEK] = pRTC->wday;

        Chip_RTC_SetFullAlarmTime(LPC_RTC, &rtc);
        Chip_RTC_AlarmIntConfig(LPC_RTC,alarmMask, ENABLE);
        RTC_SET_ALARM( TRUE );
}

void RTC_getAlarmTime( RTC_t * pRTC )
{
	RTC_TIME_T rtc;
	Chip_RTC_GetFullAlarmTime(LPC_RTC, &rtc);

	pRTC->sec  = rtc.time[RTC_TIMETYPE_SECOND];
	pRTC->min  = rtc.time[RTC_TIMETYPE_MINUTE];
	pRTC->hour = rtc.time[RTC_TIMETYPE_HOUR];
	pRTC->mday = rtc.time[RTC_TIMETYPE_DAYOFMONTH];
	pRTC->month= rtc.time[RTC_TIMETYPE_MONTH];
	pRTC->year = rtc.time[RTC_TIMETYPE_YEAR];
	pRTC->wday = rtc.time[RTC_TIMETYPE_DAYOFWEEK];
}

void RTC_disableAlarm (void)
{
	Chip_RTC_AlarmIntConfig( LPC_RTC,0xFF, DISABLE );
    RTC_SET_ALARM( FALSE );
}

void RTC_setAlarmCallback (void(*function)(void*), void* arg)
{
	rtcCallback = function;
	rtcCallbackArg = arg;
}

void RTC_calibration( uint32_t value )
{
	uint8_t calibDir = RTC_CALIB_DIR_FORWARD;

	if(value<0)
	{
		calibDir = RTC_CALIB_DIR_BACKWARD;
		value=value*(-1);
	}

	if( value>131072 )
		value=131072;

	Chip_RTC_CalibConfig(LPC_RTC, value, calibDir);
	Chip_RTC_CalibCounterCmd(LPC_RTC, ENABLE);
	Chip_RTC_Enable(LPC_RTC, ENABLE);
	RTC_SET_CALIBRATE();
}

void RTC_writeBkpRegister( uint8_t address,uint32_t value )
{
	if(address<64)
	{
		Chip_REGFILE_Write( LPC_REGFILE, address, value );
	}
}

uint32_t RTC_readBkpRegister( uint8_t address )
{
	if(address<64)
	{
		return Chip_REGFILE_Read(LPC_REGFILE, address);
	}
	return 0;
}
//==========================================
// Alarm
void RTC_IRQHandler (void)
{
	if( 1 == Chip_RTC_GetIntPending( LPC_RTC,RTC_INT_ALARM) )
	{
		if( NULL != rtcCallback )
		{
			rtcCallback( rtcCallbackArg );
		}
		Chip_RTC_ClearIntPending( LPC_RTC, RTC_INT_ALARM );
	}
	else
	{
		Chip_RTC_ClearIntPending( LPC_RTC, RTC_INT_COUNTER_INCREASE );
	}
}
