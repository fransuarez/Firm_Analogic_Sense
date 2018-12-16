/*
 * IO_Manager.h
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_TASKSERVICES_INC_INPUTMANAGER_H_
#define MODULES_SUPPORT_TASKSERVICES_INC_INPUTMANAGER_H_


void timerCB_Report 		(void *);
void timerCB_Analog 		(void *);
void timerCB_KeysRead 		(void *);
void timerCB_KeysDebounce 	(void *);
void timerCB_Proccess 		(void *);
void timerCB_InpDebounce 	(void *);

void taskControlInputs  	(void *);

/*
void GPIO0_IRQHandler 		(void);

void GPIO1_IRQHandler 		(void);
void GPIO2_IRQHandler 		(void);
void GPIO3_IRQHandler 		(void);

void ADC0_IRQHandler  		(void);
*/
#endif /* MODULES_SUPPORT_TASKSERVICES_INC_INPUTMANAGER_H_ */

