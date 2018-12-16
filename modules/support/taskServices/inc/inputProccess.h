/*
 * inputProccess.h
 *
 *  Created on: Dec 12, 2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_TASKSERVICES_INC_INPUTPROCCESS_H_
#define MODULES_SUPPORT_TASKSERVICES_INC_INPUTPROCCESS_H_

#include "lpc_types.h"
#include "ciaaGPIO_def.h"

typedef enum actions_status
{
	act_OFF=0,
	act_ON=1,
	act_SWAP=2

} actStat_t;

typedef struct op_register
{
	externId_t	id;
	actStat_t	status;
	actStat_t	action;

} opAct_t;

typedef struct actions_register
{
	int32_t	lastValue;
	int32_t	smt_max;
	int32_t	smt_min;

	opAct_t alarm_signal;
	opAct_t output_signal;
	opAct_t timer_control;

} actions_t;

void control_Termocuple (actions_t*);
void control_Amperimeter (actions_t*);
void control_Conductimeter (actions_t*);
void control_Level (actions_t*);
void control_ProcessTimer (actions_t*);
void control_ObjectDetect (actions_t*);
void control_ModeFunction (actions_t*);


#endif /* MODULES_SUPPORT_TASKSERVICES_INC_INPUTPROCCESS_H_ */
