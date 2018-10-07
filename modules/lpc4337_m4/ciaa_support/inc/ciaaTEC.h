/*
 * ciaaKEY.h
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#ifndef CIAA_SUPORT_INC_CIAAKEY_H_
#define CIAA_SUPORT_INC_CIAAKEY_H_

#include "chip.h"

void ciaaKEY_EnableIRQ (void);
void ciaaKEYS_Init(void);
uint32_t ciaaKEYS_Status(void);


#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAAKEY_H_ */
