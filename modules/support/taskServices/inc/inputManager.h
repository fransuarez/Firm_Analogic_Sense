/*
 * IO_Manager.h
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_TASKSERVICES_INC_INPUTMANAGER_H_
#define MODULES_SUPPORT_TASKSERVICES_INC_INPUTMANAGER_H_

void taskControlInputs  (void *);

void timerCallbackReport (void * );
void timerCallbackAnalog (void * );
void timerCallbackTeclas (void * );
void timerCallbackDebounce (void * );


void GPIO0_IRQHandler (void);
void GPIO1_IRQHandler (void);
void GPIO2_IRQHandler (void);

void ADC0_IRQHandler  (void);

#endif /* MODULES_SUPPORT_TASKSERVICES_INC_INPUTMANAGER_H_ */

