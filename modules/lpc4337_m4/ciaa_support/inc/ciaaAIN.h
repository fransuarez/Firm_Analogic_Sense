/*
 * ciaaAIN.h
 *
 *  Created on: Nov 20, 2018
 *      Author: fran
 */

#ifndef MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAAIN_H_
#define MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAAIN_H_


void 	ciaaAIN_Init	(void);
uint8_t ciaaAIN_Config	( uint16_t val_1, uint16_t val_2, uint8_t mode, uint8_t id );
uint8_t ciaaAIN_Update	(void);
void 	ciaaAIN_Reset	(void);



#endif /* MODULES_LPC4337_M4_CIAA_SUPPORT_INC_CIAAAIN_H_ */
