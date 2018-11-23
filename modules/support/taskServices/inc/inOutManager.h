/*
 * IO_Manager.h
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_TASKSERVICES_INC_INOUTMANAGER_H_
#define MODULES_SUPPORT_TASKSERVICES_INC_INOUTMANAGER_H_

void taskControlInputs  (void *);
void taskControlOutputs (void *);

void GPIO0_IRQHandler (void);
void GPIO1_IRQHandler (void);

void ADC0_IRQHandler  (void);

#endif /* MODULES_SUPPORT_TASKSERVICES_INC_INOUTMANAGER_H_ */

