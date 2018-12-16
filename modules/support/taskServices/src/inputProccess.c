/*
 * inputProccess.c
 *
 *  Created on: Dec 12, 2018
 *      Author: fran
 */

//#include "auxiliar_gpios_def.h"
#include "inputProccess.h"


#define INP_INF_COTA(X) 	( X->smt_min >= X->lastValue )
#define INP_SUP_COTA(X) 	( X->smt_max <= X->lastValue )

#define INP_DEFINE(X) 		( NONE != X.id )
#define INP_ACTION(X,Y)		( (INP_DEFINE(X))? (X.action= Y): X.action )
#define INP_STATUS(X,Y)		( (INP_DEFINE(X))? (Y == X.status): 0 )
#define INP_STATUS_SET(X)	( (INP_DEFINE(X))? (X.status= act_ON): X.status )
#define INP_STATUS_RESET(X)	( (INP_DEFINE(X))? (X.status= act_OFF): X.status )
// Verifica antes que este en estado bajo, que este definida, y que la accion definida sea modificar o encender antes de ejecutar.
#define INP_CMP_SET(X)		( (INP_STATUS(X,act_OFF)) ? INP_STATUS_SET(X): X.status )
// Verifica antes que este en estado alto, que este definida, y que la accion definida sea modificar o bajar.
#define INP_CMP_RESET(X) 	( (INP_STATUS(X,act_ON))? INP_STATUS_RESET(X): X.status )

#define INP_TMR_DEFINE(X) 	( INP_DEFINE(X->timer_control) )
#define INP_TMR_STATUS(X,Y)	( INP_STATUS(X->timer_control,Y) )
#define INP_TMR_ACTION(X,Y)	( INP_ACTION(X->timer_control,Y) )
#define INP_TMR_SET(X) 		( INP_CMP_SET(X->timer_control) )
#define INP_TMR_RESET(X) 	( INP_CMP_RESET(X->timer_control) )

#define INP_ALR_DEFINE(X) 	( INP_DEFINE(X->alarm_signal) )
#define INP_ALR_STATUS(X,Y)	( INP_STATUS(X->alarm_signal,Y) )
#define INP_ALR_ACTION(X,Y)	( INP_ACTION(X->alarm_signal,Y) )
#define INP_ALR_SET(X) 		( INP_CMP_SET(X->alarm_signal) )
#define INP_ALR_RESET(X) 	( INP_CMP_RESET(X->alarm_signal) )

#define INP_OUT_DEFINE(X) 	( INP_DEFINE(X->output_signal) )
#define INP_OUT_STATUS(X,Y)	( INP_STATUS(X->output_signal,Y) )
#define INP_OUT_ACTION(X,Y)	( INP_ACTION(X->output_signal,Y) )
#define INP_OUT_SET(X) 		( INP_CMP_SET(X->output_signal) )
#define INP_OUT_RESET(X) 	( INP_CMP_RESET(X->output_signal) )


void control_Termocuple (actions_t* pAct )
{
	INP_ALR_ACTION(pAct, act_OFF);
	INP_OUT_ACTION(pAct, act_OFF);
	INP_TMR_ACTION(pAct, act_OFF);

	if( INP_INF_COTA(pAct) )
	{
		if( INP_OUT_STATUS(pAct, act_OFF) )
		{
			INP_OUT_SET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_TMR_STATUS(pAct, act_OFF) )
		{
			INP_TMR_SET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
	}

	else if( INP_SUP_COTA(pAct) )
	{
		if( INP_OUT_STATUS(pAct, act_ON) )
		{
			INP_OUT_RESET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_TMR_STATUS(pAct, act_OFF) )
		{
			INP_TMR_SET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
	}

	else
	{
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_RESET(pAct);
			INP_ALR_ACTION(pAct, act_ON);
		}
		if( INP_TMR_STATUS(pAct, act_ON) )
		{
			INP_TMR_RESET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
		/* Para que haya una bandgamp de accion del heater no conviene apagarlo hasta que no llegue a la banda superior.
		if( INP_OUT_STATUS(pAct, act_ON) )
		{
			INP_OUT_RESET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		*/
	}
}

void control_Amperimeter (actions_t* pAct )
{
	INP_ALR_ACTION(pAct, act_OFF);
	INP_OUT_ACTION(pAct, act_OFF);
	INP_TMR_ACTION(pAct, act_OFF);

	if( INP_SUP_COTA(pAct) )
	{
		if( INP_OUT_STATUS(pAct, act_OFF) )
		{
			INP_OUT_SET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_TMR_STATUS(pAct, act_OFF) )
		{
			INP_TMR_SET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
	}
	else
	{
		if( INP_OUT_STATUS(pAct, act_ON) )
		{
			INP_OUT_RESET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_RESET(pAct);
			INP_ALR_ACTION(pAct, act_ON);
		}
	}
}

void control_Conductimeter (actions_t* pAct )
{
	INP_ALR_ACTION(pAct, act_OFF);
	INP_OUT_ACTION(pAct, act_OFF);
	INP_TMR_ACTION(pAct, act_OFF);

	if( INP_SUP_COTA(pAct) )
	{
		if( INP_OUT_STATUS(pAct, act_OFF) )
		{
			INP_OUT_SET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_TMR_STATUS(pAct, act_OFF) )
		{
			INP_TMR_SET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
	}
	else
	{
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_RESET(pAct);
			INP_ALR_ACTION(pAct, act_ON);
		}
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_RESET(pAct);
			INP_ALR_ACTION(pAct, act_ON);
		}
	}
}

void control_Level (actions_t* pAct)
{
	INP_ALR_ACTION(pAct, act_OFF);
	INP_OUT_ACTION(pAct, act_OFF);
	INP_TMR_ACTION(pAct, act_OFF);

	if( INP_SUP_COTA(pAct) )
	{
		if( INP_OUT_STATUS(pAct, act_OFF) )
		{
			INP_OUT_SET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}

		if( INP_TMR_STATUS(pAct, act_OFF) )
		{
			INP_TMR_SET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
	}
	else
	{
		if( INP_OUT_STATUS(pAct, act_ON) )
		{
			INP_OUT_RESET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_RESET(pAct);
			INP_ALR_ACTION(pAct, act_ON);
		}
	}
}

void control_ProcessTimer (actions_t* pAct)
{
	INP_ALR_ACTION(pAct, act_OFF);
	INP_OUT_ACTION(pAct, act_OFF);
	INP_TMR_ACTION(pAct, act_OFF);

	if( INP_SUP_COTA(pAct) )
	{
		if( INP_OUT_STATUS(pAct, act_OFF) )
		{
			INP_OUT_SET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_TMR_STATUS(pAct, act_OFF) )
		{
			INP_TMR_SET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
	}
	else
	{
		if( INP_OUT_STATUS(pAct, act_ON) )
		{
			INP_OUT_RESET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_SET(pAct);
			INP_ALR_ACTION(pAct, act_OFF);
		}
		if( INP_TMR_STATUS(pAct, act_ON) )
		{
			INP_TMR_RESET(pAct);
			INP_TMR_ACTION(pAct, act_ON);
		}
	}
}

void control_ObjectDetect (actions_t* pAct)
{
	INP_ALR_ACTION(pAct, act_OFF);

	if( INP_INF_COTA(pAct) )
	{
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_SET(pAct);
			INP_ALR_ACTION(pAct, act_OFF);
		}
	}
	else
	{
		if( INP_ALR_STATUS(pAct, act_ON) )
		{
			INP_ALR_RESET(pAct);
			INP_ALR_ACTION(pAct, act_ON);
		}
	}
}

void control_ModeFunction (actions_t* pAct)
{
	INP_OUT_ACTION(pAct, act_OFF);

	if( INP_INF_COTA(pAct) )
	{
		if( INP_OUT_STATUS(pAct, act_OFF) )
		{
			INP_OUT_SET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
	}
	else
	{
		if( INP_OUT_STATUS(pAct, act_ON) )
		{
			INP_OUT_RESET(pAct);
			INP_OUT_ACTION(pAct, act_ON);
		}
	}
	// Agregar todo lo referido a cambio de funcionamiento.
}
