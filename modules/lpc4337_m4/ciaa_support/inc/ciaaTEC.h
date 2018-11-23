/*
 * ciaaKEY.h
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#ifndef CIAA_SUPORT_INC_CIAAKEY_H_
#define CIAA_SUPORT_INC_CIAAKEY_H_

#include "lpc_types.h"

#define ERROR_ID_TECLA 99
#define TECL_PUSH 1
#define TECL_FREE 0


void 	ciaaTEC_Init		(void);
uint8_t ciaaTEC_Status		(uint8_t);

void 	ciaaTEC_EnableIRQ  	(uint8_t);
void 	ciaaTEC_DisableIRQ 	(uint8_t);

void 	ciaaTEC_DebounInit	(uint8_t);
uint8_t ciaaTEC_DebounStatus(uint8_t);

uint8_t ciaaTEC_Level_ISR 	(uint8_t);


#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAAKEY_H_ */
