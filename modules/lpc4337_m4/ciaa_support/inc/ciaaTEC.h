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

typedef enum _tec_id
{
	TECL1= 0,
	TECL2,
	TECL3,
	TECL4,
	TECL1_4
} tecId_t;

typedef enum _tec_stat
{
	TECL_RELEASE = 0x00,
	TEC1_PRESSED = 0x01,
	TEC2_PRESSED = 0x02,
	TEC3_PRESSED = 0x04,
	TEC4_PRESSED = 0x08,

} tecStat_t;


void 	ciaaTEC_Init		(void);
uint8_t ciaaTEC_Status		(tecId_t);

void 	ciaaKEY_EnableIRQ  	(tecId_t);
void 	ciaaKEY_DisableIRQ 	(tecId_t);

void 	ciaaTEC_DebounInit	(tecId_t);
uint8_t ciaaTEC_DebounStatus(tecId_t);

uint8_t ciaaTEC_Level_ISR 	(tecId_t);


#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAAKEY_H_ */
