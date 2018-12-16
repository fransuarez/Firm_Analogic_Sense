/*
 * ciaaKEY.h
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#ifndef CIAA_SUPORT_INC_CIAAKEY_H_
#define CIAA_SUPORT_INC_CIAAKEY_H_

//#include "lpc_types.h"
#include "ciaaPORT.h"
#include "ciaaGPIO_def.h"


#define ERROR_ID_TECLA 		99
#define TECLAS_TOTAL 		4
#define TECL_PUSH 			0
#define TECL_FREE 			1

#define TECL_PRESSED(X)		( 1<<X )
#define TECL_RELEASE(X)		( 0<<X )
#define TECL_STATUS(X,Y)	( X&(1<<(Y-TECL1)) )

typedef enum _tec_stat
{
	TECL_RELEASE = 0,
	TECL_INVALID = 1<<6,
	TEC1_PRESSED = 1<<0,
	TEC2_PRESSED = 1<<1,
	TEC3_PRESSED = 1<<2,
	TEC4_PRESSED = 1<<3

} tecReg_t;

typedef enum _gpio_input_stat
{
	input_state_uninit=0,
	input_state_equal,
	input_state_high,
	input_state_low

} debInput_t;

void 	ciaaTEC_Init		(void);
tecReg_t ciaaTEC_Status		(perif_t);
void 	ciaaTEC_EnableIRQ  	(perif_t, irqChId_t);
void 	ciaaTEC_DisableIRQ 	(perif_t);
void 	ciaaTEC_DebounInit	(perif_t);
tecReg_t ciaaTEC_DebounStatus(perif_t);
tecReg_t ciaaTEC_Level_ISR 	(perif_t);

int 	ciaaGPIO_EnablePin 	(perif_t gpio_id, int mode);
int	 	ciaaGPIO_EnableIRQ 	(perif_t gpio_id, irqChId_t irq_id);
int 	ciaaGPIO_GetLevelIRQ (perif_t inp_id, irqChId_t irq_id);

// Aplican si la GPIO funciona como salida:
void 	ciaaGPIO_SetLevel 	(perif_t inp_id, uint8_t value);
void 	ciaaGPIO_Toggle 	(perif_t gpio_id);
int 	ciaaGPIO_GetLevel 	(perif_t gpio_id);
// Aplican si la GPIO funciona como entrada::
debInput_t ciaaGPIO_DebounStatus (perif_t key_id);
void 	ciaaGPIO_DebounInit (perif_t key_id);

#endif /* MODULES_LPC4337_M4_CIAA_SUPORT_INC_CIAAKEY_H_ */
