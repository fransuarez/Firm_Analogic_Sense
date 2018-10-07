/*
 * ciaaRTC.h
 *
 *  Created on: 13/9/2018
 *      Author: fran
 */

#ifndef CIAA_SUPORT_INC_CIAARTC_H_
#define CIAA_SUPORT_INC_CIAARTC_H_

#include "chip.h"

/*==================[typedef]================================================*/

typedef struct {
   uint16_t year;	 /* 1 to 4095 */
   uint8_t  month; /* 1 to 12   */
   uint8_t  mday;	 /* 1 to 31   */
   uint8_t  wday;	 /* 1 to 7    */
   uint8_t  hour;	 /* 0 to 23   */
   uint8_t  min;	 /* 0 to 59   */
   uint8_t  sec;	 /* 0 to 59   */
} RTC_t;

/**
 * @brief       Initialize RTC module
 * @return      void
 */
void RTC_Init(void);

/**
 * @brief       Calibrate RTC module
 * @param       value : Calibration value, should be in range from -131,072 to 131,072
 * @return      void
 */
void RTC_calibration(uint32_t value);

/**
 * @brief       Return current RTC module datetime
 * @param       hr : Pointer to variable where Hour (24hs format) will be stored
 * @param       min : Pointer to variable where Minutes will be stored
 * @param       sec : Pointer to variable where Seconds will be stored
 * @param       day : Pointer to variable where Day of Month will be stored
 * @param       mon : Pointer to variable where Month will be stored
 * @param       yr : Pointer to variable where Year will be stored
 * @param       dayOfWeek : Pointer to variable where Day of Week will be stored
 * @return      void
 */
void RTC_getTime( RTC_t * rtc );


/**
 * @brief       Set current RTC module datetime
 * @param       hr : Hour (24hs format)
 * @param       min : Minutes
 * @param       sec : Seconds
 * @param       day : Day
 * @param       mon : Month
 * @param       yr : Year
 * @param       dayOfWeek : Day of Week
 * @return      void
 */
void RTC_setTime( RTC_t * rtc );


/**
 * @brief       Write a value in RTC Backup registers space
 * @param       address : Back up register address (o to 63)
 * @param       value : 32-bit value to be written
 * @return      void
 */
void RTC_writeBkpRegister( uint8_t address, uint32_t value );

/**
 * @brief       Read a value from RTC Backup registers space
 * @param       address : Back up register address (o to 63)
 * @return      32-bit value
 */
uint32_t RTC_readBkpRegister( uint8_t address );


/**
 * @brief       Set Alarm for RTC module
 * @param       hr : Hour (24hs format)
 * @param       min : Minutes
 * @param       sec : Seconds
 * @param       day : Day
 * @param       mon : Month
 * @param       yr : Year
 * @param       dayOfWeek : Day of Week
 * @param       alarmMask :  Or'ed bit values for time types:
 * 				bit 0 : second
 * 				bit 1 : minute
 * 				bit 2 : hour
 * 				bit 3 : day of month
 * 				bit 4 : day of week
 * 				bit 5 : always 0 (not implemented)
 * 				bit 6 : month
 * 				bit 7 : year
 *
 * @return      void
 */
void RTC_setAlarmTime( RTC_t * rtc, uint32_t alarmMask );

/**
 * @brief       Return Alarm RTC module datetime
 * @param       hr : Pointer to variable where Hour (24hs format) will be stored
 * @param       min : Pointer to variable where Minutes will be stored
 * @param       sec : Pointer to variable where Seconds will be stored
 * @param       day : Pointer to variable where Day of Month will be stored
 * @param       mon : Pointer to variable where Month will be stored
 * @param       yr : Pointer to variable where Year will be stored
 * @param       dayOfWeek : Pointer to variable where Day of Week will be stored
 * @return      void
 */
void RTC_getAlarmTime( RTC_t * rtc );

/**
 * @brief       Disable RTC Alarm
 * @return      void
 */
void RTC_disableAlarm (void);

/**
 * @brief       Sets the function that will be called by RTC Alarm interupt
 * @param       function : calback to be called. prototype: void function(void* arg);
 * @param       arg : argument passed to callback function when it is called
 * @return      void
 */
void RTC_setAlarmCallback( void(*function)(void*), void* arg);


#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAARTC_H_ */
