/*
 * ciaaRTC.c
 *
 *  Created on: 13/9/2018
 *      Author: fran
 */

#include <time.h>
#include "api_RTC.h"

//#include "chip.h"
// Despues debo sacar estas funciones de aca lo dejo por ahora por comodidad.
//================================================[RTC Management]========================================================

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
static time_t timer;
struct tm * sysRTC;

void RTC_Init( void )
{
	if( !RTC_GET_INIT() )
	{
		RTC_SET_INIT();
	}
}

void RTC_setTime( RTC_t * pRTC )
{
    RTC_SET_IN_TIMED();

	ctRTC.sec = pRTC->sec;
	ctRTC.min = pRTC->min;
	ctRTC.hour = pRTC->hour;
	ctRTC.mday = pRTC->mday;
	ctRTC.month = pRTC->month;
	ctRTC.year = pRTC->year;
	ctRTC.wday = pRTC->wday;
}

void RTC_getTime( RTC_t * pRTC )
{
	if( RTC_GET_INIT() )
	{
		time (&timer);
		sysRTC= localtime (&timer);
	}

	ctRTC.sec = sysRTC->tm_sec;
	ctRTC.min = sysRTC->tm_min;
	ctRTC.hour = sysRTC->tm_hour;
	ctRTC.mday = sysRTC->tm_mday;
	ctRTC.month = sysRTC->tm_mon;
	ctRTC.year = sysRTC->tm_year;
	ctRTC.wday = sysRTC->tm_wday;

	*pRTC= ctRTC;
}



