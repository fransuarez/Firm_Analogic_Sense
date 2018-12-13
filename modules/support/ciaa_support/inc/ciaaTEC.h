/*
 * ciaaKEY.h
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#ifndef CIAA_SUPORT_INC_CIAAKEY_H_
#define CIAA_SUPORT_INC_CIAAKEY_H_

//#include "lpc_types.h"


#define ERROR_ID_TECLA 		99
#define TECLAS_TOTAL 		4
#define TECL_PUSH 			0
#define TECL_FREE 			1

#define GPIO_HIGH_LEVEL		1
#define GPIO_LOW_LEVEL		0

#define TECL_PRESS(T,P,A)	( (T & P) && !(T & A) )
#define TECL_RELEASE(T,P,A)	( !(T & P) && (T & A) )

typedef enum _tec_stat
{
	TECL_RELEASE = 0,
	TEC1_PRESSED = 1<<0,
	TEC2_PRESSED = 1<<1,
	TEC3_PRESSED = 1<<2,
	TEC4_PRESSED = 1<<3,

} tecReg_t;

void 	ciaaTEC_Init		(void);
tecReg_t ciaaTEC_Status		(perif_t);

void 	ciaaTEC_EnableIRQ  	(perif_t, irqChId_t);
void 	ciaaTEC_DisableIRQ 	(perif_t);

void 	ciaaTEC_DebounInit	(perif_t);
tecReg_t ciaaTEC_DebounStatus(perif_t);

tecReg_t ciaaTEC_Level_ISR 	(perif_t);

int ciaaGPIO_EnablePin (perif_t gpio_id, int mode);
int ciaaGPIO_EnableIRQ (perif_t gpio_id, irqChId_t irq_id);
int ciaaGPIO_GetLevelIRQ (perif_t inp_id, irqChId_t irq_id);

// Aplican si la GPIO funciona como salida:
void ciaaGPIO_SetLevel (perif_t inp_id, uint8_t value);
void ciaaGPIO_Toggle (perif_t gpio_id);
int ciaaGPIO_GetLevel (perif_t gpio_id);


#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAAKEY_H_ */
