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


typedef enum
{
	TECL1=0,
	TECL2,
	TECL3,
	TECL4
} tecls_t;

void 	 ciaaKEY_EnableIRQ 	(void);
void 	 ciaaTEC_Init		(void);
uint32_t ciaaTEC_Status		(void);


void PULSE_COUNT_TASK_Init(void);
uint8_t PULSE_COUNT_TASK_UpdateFallings(void);

uint8_t LEVEL_KEY_Detec_From_ISR ( uint8_t );


#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAAKEY_H_ */
