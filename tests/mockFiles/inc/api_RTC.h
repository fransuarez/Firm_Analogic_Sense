/*
 * ciaaRTC.h
 *
 *  Created on: 13/9/2018
 *      Author: fran
 */

#ifndef CIAA_SUPORT_INC_CIAARTC_H_
#define CIAA_SUPORT_INC_CIAARTC_H_

//#include "chip.h"
#include "api_TYPES.h"

extern  uint8_t rtc_status;
#define RTC_MASK_INIT		( 0x01 )
#define RTC_MASK_CALIB		( 0x02 )
#define RTC_MASK_TIMED		( 0x04 )
#define RTC_MASK_ALARM		( 0x08 )

#define RTC_SET_INIT()		( rtc_status |= RTC_MASK_INIT )
#define RTC_GET_INIT()		( rtc_status & RTC_MASK_INIT  )
#define RTC_SET_CALIBRATE()	( rtc_status |= RTC_MASK_CALIB )
#define RTC_GET_CALIBRATE()	( rtc_status & RTC_MASK_CALIB )
#define RTC_SET_IN_TIMED()	( rtc_status |= RTC_MASK_TIMED )
#define RTC_GET_IN_TIMED()	( rtc_status & RTC_MASK_TIMED )


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




#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAARTC_H_ */
