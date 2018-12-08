/*
 * ciaaAIN.h
 *
 *  Created on: Nov 20, 2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAAIN_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAAIN_H_

#include "lpc_types.h"
#include "ciaaGPIO_def.h"

#define AIN_CONVER_ERR		( 0x01 )
#define AIN_NIVEL_MIN		( 0x02 )
#define AIN_NIVEL_MAX		( 0x04 )
#define AIN_NIVEL_VARIO		( 0x08 )
#define AIN_MASK_FLAGS		( 0x0F )

// * MACROS **********************************************************

#define BIT_ON_REG4(X,Y,Z)  ( X |= (Z<<(Y*4)) )
#define BIT_OFF_REG4(X,Y,Z) ( X &= ~(Z<<(Y*4)) )
#define BIT_SEL_REG4(X,Y)	( X = (AIN_MASK_FLAGS & (X>>(Y*4))) )

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

typedef struct _report_state
{
	//uint8_t 	change;
	externId_t	name;  // Es tamaño byte???
	uint8_t 	enable;
	uint8_t 	dummy;
	unit_t  	unit;
	int32_t 	value;

} repAnStat_t;


void 	ciaaAIN_Init	(void);
void 	ciaaAIN_Reset	(void);

uint8_t ciaaAIN_Config	( uint16_t val_1, uint16_t val_2, uint8_t mode, uint8_t id );
uint16_t ciaaAIN_Update	( repAnStat_t *);


#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAAIN_H_ */
