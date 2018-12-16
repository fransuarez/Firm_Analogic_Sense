/*
 * IO_Manager.c
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#include "services_config.h"
#include "api_RTC.h"

//#include "ciaaGPIO_def.h"
#include "ciaaPORT.h"
#include "ciaaTEC.h"
#include "ciaaAIN.h"
#include "inputProccess.h"
#include "terminalManager.h"

extern TimerHandle_t 		TIMER_1_OBJ;
extern TimerHandle_t 		TIMER_2_OBJ;
extern TimerHandle_t 		TIMER_3_OBJ;
extern TimerHandle_t 		TIMER_4_OBJ;
extern TimerHandle_t 		TIMER_5_OBJ;
extern TimerHandle_t 		TIMER_6_OBJ;
extern TimerHandle_t 		TIMER_7_OBJ;
extern TimerHandle_t 		TIMER_8_OBJ;
extern TimerHandle_t 		TIMER_9_OBJ;
extern TimerHandle_t 		TIMER_10_OBJ;

extern QueueHandle_t 		MGR_INPUT_QUEUE;
extern QueueHandle_t 		MGR_OUTPUT_QUEUE;
extern QueueHandle_t 		MGR_TERMINAL_QUEUE;
extern QueueHandle_t 		MGR_DATALOG_QUEUE;

extern SemaphoreHandle_t 	MGR_INPUT_MUTEX;

//extern TaskHandle_t 		MGR_INPUT_HANDLER;
//extern UBaseType_t*			STACKS_TAREAS;

//**************************************************************************************************
#define SIZE_BUFF_TERM		( 32 )
#define SIZE_MON_TERM		( 128 )

#define SAMPLE_ANALOG_MSEC	( 15*1000 ) //Viene de
#define SAMPLE_DIGITAL_MSEC	( 200 )
#define SAMPLE_DIGITAL_IRQ	( 0 )

#define SAMPLE_MAX_SEC		( 255 ) // Viene de (1<<7)-1 =255 == 4'15"
#define SAMPLE_MAX_MINUTES	( 4 ) 	// Viene de 2^7-1= 255/60= 4,25-> 4'
#define SAMPLE_MAX_SECONDS	( 15 ) 	// Viene de 60"+15"
#define SAMPLE_TECLAS_MAX	( 100 )

#define RET_VAL_NO_PROCESS  ( 0 )
#define RET_VAL_OK			( 1 )
#define RET_VAL_BUFF_FULL	( 2 )
#define RET_VAL_ERR_QUEUE	( 4 )

#define TIMER_REPORTS		TIMER_1_OBJ
#define TIMER_ANALOGS_UPD	TIMER_2_OBJ
#define TIMER_TECLAS_INIT	TIMER_3_OBJ
#define TIMER_DEBOUNCE		TIMER_4_OBJ
#define TIMER_DEBOUNCE_GPIO	TIMER_10_OBJ

#define TIMER_AL_TEMPER		TIMER_5_OBJ
#define TIMER_AL_CONDUCT	TIMER_6_OBJ
#define TIMER_AL_LEVEL		TIMER_7_OBJ
#define TIMER_AL_TIME_OUT	TIMER_8_OBJ
#define TIMER_END_PROCCESS	TIMER_9_OBJ

#define INP_TIMER_DEFINE(X)	( NULL != X->alarm_timer )
#define INP_TIMER_START(X) 	( (INP_TIMER_DEFINE(X))? (xTimerStart( X->alarm_timer, TIMER_WAIT_TOUT )): pdFAIL )
#define INP_TIMER_STOP(X) 	( (INP_TIMER_DEFINE(X))? (xTimerStop ( X->alarm_timer, TIMER_WAIT_TOUT )): pdFAIL )

#define TIMER_WAIT_TOUT		( 3 )
#define REG_INP_DIG_FIRST	( WATER_LEVEL )
#define REG_INP_DIG_LAST	( SL_MODE_FUNCTION )
//#define PERIF_VALID(X)		( X )

//**************************************************************************************************
typedef TimerHandle_t pTimer_t;

typedef struct periferics_ids
{
	externId_t 	id;
	const char * name;
	const char * unit;

} perInfo_t;

typedef struct periferics_register
{
	externId_t 	name;
	perif_t		id_gpio;
	uint8_t		enable;
	unit_t		unit;

	uint8_t		status;		// CHANGE, MAXIMO, MINIMO, ERROR > b0000
	shTime_t	time_value;
	actions_t*	actions;
	pTimer_t	alarm_timer;

} perReg_t;

//**************************************************************************************************

static char 			buffSendTerminal[SIZE_BUFF_TERM]= "";
static char 			buffMonTerminal[SIZE_MON_TERM]= "";
static GpioReg_t 		regToLog[REG_OPR_LOG];
static dlogPack_t 		dataToLog = { .cmd= writePage, .data= regToLog };
static RTC_t hdRTC;

static perInfo_t nombInputs[EXT_INPUTS_TOTAL]=
{
	{ .id= TERMOCUPLE		, .name= "T_CUPLA"	, .unit= "CELSIUS" 	},
	{ .id= THERMISTOR		, .name= "T_NTC" 	, .unit= "CELSIUS" 	},
	{ .id= AMPERIMETER		, .name= "C_AMP"	, .unit= "AMPERS"  	},
	{ .id= CONDUCTIMETER	, .name= "Q_ION"	, .unit= "mgSQRMTR"	},
	{ .id= WATER_LEVEL		, .name= "L_AGUA"	, .unit= "cMETERS" 	},
	{ .id= SW_START_STOP	, .name= "SW_START"	, .unit= "LEVEL" 	},
	{ .id= SL_OBJECT_DETECT	, .name= "SL_OBJECT", .unit= "LEVEL" 	},
	{ .id= SL_MODE_FUNCTION	, .name= "SL_MODE"	, .unit= "LEVEL" 	}
};
static actions_t actionsInputs[EXT_INPUTS_TOTAL]=
{
	{ // TERMOCUPLE
	.smt_min= 50,
	.smt_max= 100,
	.alarm_signal 	= { .id= ALARM_TEMPER, .status = act_OFF 		} ,
	.output_signal	= { .id= RELAY_HEATER, .status = act_OFF 		} ,
	.timer_control 	= { .id= TIMER_ALARM_TEMP, .status = act_OFF 	} ,
	},
	{ // THERMISTOR
	.smt_min= 10,
	.smt_max= 50,
	.alarm_signal.id = NONE	,
	.output_signal.id= NONE ,
	.timer_control.id= NONE
	},
	{ // AMPERIMETER:
	.smt_min= 0,
	.smt_max= 50000,
	.alarm_signal	= { .id= ALARM_TIME, .status = act_OFF 			} ,
	.output_signal	= { .id= SIGNAL_COMPLETE, .status = act_OFF 	},
	.timer_control 	= { .id= TIMER_ALARM_TIME, .status = act_OFF 	} ,
	},
	{ // CONDUCTIMETER:
	.smt_min= 0,
	.smt_max= 100,
	.alarm_signal 	= { .id= ALARM_CONDUCT ,.status = act_OFF 		} ,
	.output_signal	= { .id= RELAY_VALVE	 ,.status = act_OFF 	} ,
	.timer_control 	= { .id= TIMER_ALARM_CONDUCT, .status = act_OFF } ,
	},
	{ // WATER_LEVEL:
	.smt_min= OUT_STATUS_RESET,
	.smt_max= OUT_STATUS_SET,
	.alarm_signal 	= { .id= ALARM_LEVEL	 ,.status = act_OFF	 	} ,
	.output_signal	= { .id= RELAY_VALVE	 ,.status = act_OFF  	} ,
	.timer_control 	= { .id= TIMER_ALARM_LEVEL, .status = act_OFF 	}
	},
	{ // SW_START_STOP:
	.smt_min= OUT_STATUS_RESET,
	.smt_max= OUT_STATUS_SET,
	.alarm_signal 	= { .id= SIGNAL_STOP , .status = act_OFF 		} ,
	.output_signal	= { .id= SIGNAL_START, .status = act_OFF 		} ,
	.timer_control 	= { .id= TIMER_PROCCESS_END, .status = act_OFF 	}
	},
	{ // SL_OBJECT_DETECT:
	.smt_min= OUT_STATUS_RESET,
	.smt_max= OUT_STATUS_SET,
	.alarm_signal 	= { .id= ALARM_INTERRUPT , .status = act_OFF 	} ,
	.output_signal.id= NONE ,
	.timer_control.id= NONE
	},
	{ // SL_MODE_FUNCTION:
	.smt_min= OUT_STATUS_RESET,
	.smt_max= OUT_STATUS_SET,
	.alarm_signal.id= NONE ,
	.output_signal	= { .id= SIGNAL_MODE	, .status = act_OFF 	} ,
	.timer_control.id= NONE
	}
};

// No modificar orden!
static perReg_t stateInputs[EXT_INPUTS_TOTAL]=
{
	{
	.name= TERMOCUPLE, .id_gpio= AIN_1, .enable= TRUE, .unit= CELSIUS,
	.actions= &actionsInputs[TERMOCUPLE], .alarm_timer=NULL,
	},
	{
	.name= THERMISTOR, .id_gpio= AIN_2, .enable= TRUE, .unit= CELSIUS,
	.actions= &actionsInputs[THERMISTOR], .alarm_timer=NULL,
	},
	{
	.name= AMPERIMETER, .id_gpio= AIN_3, .enable= TRUE, .unit= AMPERS,
	.actions= &actionsInputs[AMPERIMETER], .alarm_timer=NULL,
	},
	{
	.name= CONDUCTIMETER, .id_gpio= AIN_2, .enable= FALSE, .unit= mgSQRMTR,
	.actions= &actionsInputs[CONDUCTIMETER], .alarm_timer=NULL,
	},
	{	// Switch 1:
	.name= WATER_LEVEL, .id_gpio= GPIO_5, .enable= TRUE, .unit= LEVEL,
	.actions= &actionsInputs[WATER_LEVEL], .alarm_timer=NULL,
	},
	{
	.name= SW_START_STOP, .id_gpio= TECL1, .enable= TRUE, .unit= LEVEL,
	.actions= &actionsInputs[SW_START_STOP], .alarm_timer=NULL,
	},
	{	// Switch 2:
	.name= SL_OBJECT_DETECT, .id_gpio= LCD_1, .enable= TRUE, .unit= LEVEL,
	.actions= &actionsInputs[SL_OBJECT_DETECT], .alarm_timer=NULL,
	},
	{	// Switch 3:
	.name= SL_MODE_FUNCTION, .id_gpio= GPIO_1, .enable= TRUE, .unit= LEVEL,
	.actions= &actionsInputs[SL_MODE_FUNCTION], .alarm_timer=NULL,
	}
};

//**************************************************************************************************
// solo para debug
uint8_t sizeTypes[6];
void _calcSizeTypes (uint8_t * retval)
{
	retval[0]= sizeof(TypeLog_t);
	retval[1]= sizeof(TypePort_t);
	retval[2]= sizeof(shTime_t);
	retval[3]= sizeof(perif_t);
	retval[4]= sizeof(dInOutQueue_t);
	retval[5]= sizeof(unit_t);
}

// TODO agregarla alguna libreria de manejo de tiempos. Lo unico que deberia de pasarle extra es 4' y 15".
static uint8_t sup_calculateTime (uint8_t initMin, uint8_t initSec, uint8_t endMin, uint8_t endSec)
{
	uint8_t sec_diff;
	uint8_t min_diff;

	min_diff= endMin-initMin;
	sec_diff= endSec-initSec;

	if( 59 < min_diff )
	{
		min_diff= ~min_diff;
	}
	if( SAMPLE_MAX_MINUTES < min_diff )
	{
		min_diff= SAMPLE_MAX_MINUTES;  // FIXME ver como arreglar este caso de forma correcta.
	}

	if( 59 < sec_diff )
	{
		sec_diff= ~sec_diff;
	}
	if( (SAMPLE_MAX_SECONDS<sec_diff) && (SAMPLE_MAX_MINUTES==min_diff) )
	{
		sec_diff= SAMPLE_MAX_SECONDS;
	}

	sec_diff+= min_diff*60;
	return sec_diff;
}

static uint8_t sup_historyRegister (dInOutQueue_t* paqInfoGPIO)
{
	uint8_t retval=RET_VAL_NO_PROCESS;
	GpioReg_t histRegGpio;

	static int idReg= 0;
	static uint8_t minute_init;
	static uint8_t second_init;

	if( !TOMAR_SEMAFORO( MGR_INPUT_MUTEX, TIMEOUT_MUTEX_INPUT) )
	{
		return RET_VAL_ERR_QUEUE;
	}
	else
	{
		if( !idReg )
		{
			if( RTC_GET_IN_TIMED() )
			{
				RTC_getTime( &hdRTC );
			}
			dataToLog.auxVal_1= hdRTC.hour;
			dataToLog.auxVal_2= hdRTC.min;
			minute_init= hdRTC.min;
			second_init= hdRTC.sec;
		}

		histRegGpio.name = paqInfoGPIO->data.name;
		histRegGpio.value= paqInfoGPIO->data.value;

		if( !histRegGpio.secTime )
		{
			histRegGpio.secTime= sup_calculateTime( minute_init, second_init, paqInfoGPIO->sTime.minutes, paqInfoGPIO->sTime.seconds );
		}

		//&regToLog[idReg] << Equivalente
		memcpy( (dataToLog.data)+idReg, &histRegGpio, sizeof(GpioReg_t));

		retval= RET_VAL_OK;
		// Si se completa el total de registros entonces reinicia desde el principio y
		// se debe escibir el registro completo en memoria.
		if( REG_OPR_LOG <= (++idReg) )
		{
			idReg= 0;
			dataToLog.nReg= REG_OPR_LOG;

			retval|= RET_VAL_BUFF_FULL;
		}
	}
	if( !LIBERAR_SEMAFORO( MGR_INPUT_MUTEX ) )
	{
		retval|= RET_VAL_ERR_QUEUE;
	}
	return retval;
}

static uint8_t sup_historyDInput (dInOutQueue_t * regDigital)
{
	uint8_t retval=RET_VAL_NO_PROCESS;
	int i= regDigital->data.name;

	// Se supone que si esta aca es porque vario su valor:
	// FIXME puedo agregarle una condiion extra, verificando si quiero historizar esta variable
	if( stateInputs[i].actions->lastValue )
	{
		retval= sup_historyRegister( regDigital );
	}

	return retval;
}

static uint8_t sup_historyAInput (repAnStat_t * regAnalogics, uint16_t ainStatus)
{
	uint8_t retval= RET_VAL_NO_PROCESS;
	dInOutQueue_t data_record;
	uint8_t i, j;

	if( RTC_GET_IN_TIMED() )
	{
		RTC_getTime( &hdRTC );
	}

	for (j = 0; j < ADC_TOTAL; ++j)
	{
		i= regAnalogics[j].name;

		if( FALSE == stateInputs[i].enable )
		{
			continue;
		}
		stateInputs[i].actions->lastValue= regAnalogics[j].value;
		stateInputs[i].status= (uint8_t) AINP_GET_ALL(ainStatus, i);
		stateInputs[i].time_value.minutes= hdRTC.min;
		stateInputs[i].time_value.seconds= hdRTC.sec;

		// FIXME esto no deberia de hacerlo aca, sino en una funcion de configuracion.
		if( (stateInputs[i].name != regAnalogics[j].name) || (stateInputs[i].unit != regAnalogics[j].unit) )
		{
			stateInputs[i].name = regAnalogics[j].name;
			stateInputs[i].unit = regAnalogics[j].unit;
			//stateInputs[i].id_gpio = regDigital->gpio;

		}

		// Solo grabo en el registro cuando la entrada varia su valor:
		// FIXME puedo agregarle una condiion extra, verificando si quiero historizar esta variable
		//if( AINP_GET_VAR(reg_status, i) )
		{
			//data_record.gpio= stateInputs[i].id_gpio; // No importa este valor
			data_record.sTime.minutes= hdRTC.min;
			data_record.sTime.seconds= hdRTC.sec;

			data_record.data.name= regAnalogics[j].name;
			data_record.data.value= regAnalogics[j].value;
			data_record.data.secTime=0;

			retval= sup_historyRegister( &data_record );
		}
	}
	return retval;
}

static uint8_t sup_convertReg (terMsg_t *msgToSend, uint8_t rId)
{
	uint8_t retval=0;

	if( EXT_INPUTS_TOTAL > rId )
	{
		sprintf( buffMonTerminal, "Inp: %s		V:%i	  U: %s 	T:%i'%i\"",
				 nombInputs[rId].name, stateInputs[rId].actions->lastValue, nombInputs[rId].unit,
				 stateInputs[rId].time_value.minutes, stateInputs[rId].time_value.seconds
		);
		msgToSend->msg= buffMonTerminal;
		msgToSend->size= strlen(buffMonTerminal);

		retval=1;
	}
	return retval;
}

static uint8_t sup_updateDInput (dInOutQueue_t * regDigital)
{
	uint8_t retval= RET_VAL_OK;
	int i= regDigital->data.name;

	if( RTC_GET_IN_TIMED() )
	{
		RTC_getTime( &hdRTC );
		regDigital->sTime.minutes= hdRTC.min;
		regDigital->sTime.seconds= hdRTC.sec;
	}

	if( TRUE == stateInputs[i].enable )
	{
		stateInputs[i].time_value.minutes= regDigital->sTime.minutes;
		stateInputs[i].time_value.seconds= regDigital->sTime.seconds;
		stateInputs[i].status= regDigital->data.value;
		stateInputs[i].actions->lastValue= stateInputs[i].status;

		// Si coiniciden los registros entonces actualiza el valor leido.
		if( (stateInputs[i].name != regDigital->data.name) || (LEVEL != stateInputs[i].unit) )
		{
			stateInputs[i].name = regDigital->data.name;
			stateInputs[i].unit = LEVEL;
			stateInputs[i].id_gpio = regDigital->gpio;
		}

	}
	return retval;
}

static uint8_t sup_integerCompare(actions_t* pAct)
{
	uint8_t retval= RET_VAL_NO_PROCESS;
	static int proccess=0;
	static int32_t counter=0;

	if( GPIO_LOW_LEVEL  == stateInputs[SL_OBJECT_DETECT].status )
	{
		if( !proccess )
		{
			proccess=1;
		}
	}
	//if( GPIO_LOW_LEVEL == stateInputs[SW_START_STOP].status )
	else
	{
		proccess=0;
		counter=0;
		pAct->lastValue=0;
	}
	/*
	else if( GPIO_HIGH_LEVEL == stateInputs[SL_OBJECT_DETECT].status )
	{
		counter=0;
		pAct->lastValue=0;
		retval= RET_VAL_OK;
	}
	 */
	if( proccess )
	{
		counter+= pAct->lastValue;
		pAct->lastValue= counter;
		retval= RET_VAL_OK;
	}
	return retval;
}

static uint8_t sup_changeMode (actions_t* pAct)
{
	uint8_t retval= RET_VAL_OK;

	if( GPIO_HIGH_LEVEL == pAct->lastValue )
	{
		stateInputs[AMPERIMETER].enable= FALSE;
		stateInputs[CONDUCTIMETER].enable= TRUE;
	}
	else
	{
		stateInputs[AMPERIMETER].enable= TRUE;
		stateInputs[CONDUCTIMETER].enable= FALSE;
	}
	return retval;
}

static uint8_t sup_statusProccess (actions_t* pAct)
{
	uint8_t retval= RET_VAL_OK;
	/*
	 * El unico caso critico es cuando se esta en proceso iniciado. Un deteccion de
	 * ausencia de objeto disparar un alarma. Casos contarios se ignora esta entrada.
	 */

	if( GPIO_HIGH_LEVEL == stateInputs[SW_START_STOP].status&&
		GPIO_LOW_LEVEL == pAct->lastValue	)
	{
		retval= RET_VAL_NO_PROCESS;
	}
	else
	{
		// Si no esta contando tiempo etonces fuerzo el cambio de estado para no tener
		// una alarma innecesaria.
		pAct->lastValue= GPIO_HIGH_LEVEL;
	}
	return retval;
}

//**************************************************************************************************

//static int initAnalog=0;
void timerCB_Report (TimerHandle_t pxTimer)
{
	static int i=0;
	dInOutQueue_t datatoSend;

	if( EXT_INPUTS_TOTAL <= i )
	{
		i= 0;
	}
	do
	{
		if( TRUE == stateInputs[i].enable )
		{
			datatoSend.mode= portReportStatus;
			datatoSend.gpio= i;
			xQueueSend( MGR_INPUT_QUEUE, &datatoSend, TIMEOUT_QUEUE_MSG_OUT );
			++i;
			break;
		}
	}
	while( FALSE == stateInputs[i++].enable && EXT_INPUTS_TOTAL > i);
}

void timerCB_Analog (TimerHandle_t pxTimer)
{
	dInOutQueue_t datatoSend;

	datatoSend.mode= portInputAnalog;
	xQueueSend( MGR_INPUT_QUEUE, &datatoSend, TIMEOUT_QUEUE_MSG_OUT );
	//initAnalog= 1;
}

void timerCB_KeysRead (TimerHandle_t pxTimer)
{
	//static int expired= 0;
/*
	if( tecStatus )
	{
		xTimerReset( pxTimer, TIMER_WAIT_TOUT );
		//expired= 0;
	}

	else
	{
	//	if( SAMPLE_TECLAS_MAX <= ++expired)
		{
			initDebounce=0;
			xTimerStop( TIMER_DEBOUNCE, TIMER_WAIT_TOUT );
		}
	}
	*/
}

void timerCB_KeysDebounce (TimerHandle_t pxTimer)
{
	static int initDebounce=0;
	tecReg_t tecStatus;

	dInOutQueue_t datatoSend={ .mode=portInputTecl, .data.secTime=0 };
	int j;

	if( !initDebounce )
	{
		ciaaTEC_DebounInit(TECL_ALL);
		initDebounce= 1;
	}

	tecStatus= ciaaTEC_DebounStatus(TECL_ALL);

	// Algoritmo de busqueda de asociacon de pulsador con perifericos.
	for( j=REG_INP_DIG_FIRST; REG_INP_DIG_LAST>=j; j++ )
	{
		if( TECL_VALID( stateInputs[j].id_gpio ) && stateInputs[j].enable )
		{
			datatoSend.gpio= stateInputs[j].id_gpio ;
			datatoSend.data.name= stateInputs[j].name;

			if( TECL_STATUS( tecStatus, stateInputs[j].id_gpio ) )
			{
				if( GPIO_HIGH_LEVEL == stateInputs[j].status )
				{
					datatoSend.data.value= GPIO_LOW_LEVEL;
				}
				else
				{
					datatoSend.data.value= GPIO_HIGH_LEVEL;
				}

				xQueueSend( MGR_INPUT_QUEUE, &datatoSend, TIMEOUT_QUEUE_MSG_OUT );
			}
			/*
			else if( TECL_RELEASE( TECL_FLAG_REG(id ), tecPrevStatus, tecStatus ) )
			{
				datatoSend.data.value= TECL_FREE;
				xQueueSend( MGR_INPUT_QUEUE, &datatoSend, TIMEOUT_QUEUE_MSG_OUT );
			}
			*/
		}
	}
}

void timerCB_InpDebounce (TimerHandle_t pxTimer)
{
	dInOutQueue_t datatoSend={ .mode=portInputDigital, .data.secTime=0 };
	static int setUp= 0;
	debInput_t gpioStat;
	int j;

	if( !setUp )
	{
		for( j=REG_INP_DIG_FIRST; REG_INP_DIG_LAST>=j; j++ )
		{
			ciaaGPIO_DebounInit( stateInputs[j].id_gpio );
		}
		setUp= 1;
	}

	// Algoritmo de busqueda de asociacon de pulsador con perifericos.
	for( j=REG_INP_DIG_FIRST; REG_INP_DIG_LAST>=j; j++ )
	{
		if( (FALSE == stateInputs[j].enable) && !GPIO_VALID( stateInputs[j].id_gpio ) )
		{
			continue;
		}

		gpioStat= ciaaGPIO_DebounStatus( stateInputs[j].id_gpio );
		if( gpioStat )
		{
			datatoSend.gpio= stateInputs[j].id_gpio ;
			datatoSend.data.name= stateInputs[j].name;

			if( input_state_low == gpioStat )
			{
				datatoSend.data.value= GPIO_LOW_LEVEL;
				xQueueSend( MGR_INPUT_QUEUE, &datatoSend, TIMEOUT_QUEUE_MSG_OUT );
			}
			else if( input_state_high == gpioStat )
			{
				datatoSend.data.value= GPIO_HIGH_LEVEL;
				xQueueSend( MGR_INPUT_QUEUE, &datatoSend, TIMEOUT_QUEUE_MSG_OUT );
			}
			else
			{
				// Entrada no cambio de estado
			}
		}
	}

}

void timerCB_Proccess (TimerHandle_t pxTimer)
{
	dInOutQueue_t datatoSend= {.mode= portOutputDigital, .data.value= OUT_STATUS_SET, .data.secTime=0};
	int i;
	//actions_t* pAct;

	for(i = 0; i < EXT_INPUTS_TOTAL; ++i)
	{
		if( stateInputs[i].alarm_timer == pxTimer )
		{
			datatoSend.data.name = stateInputs[i].actions->alarm_signal.id;
			stateInputs[i].actions->alarm_signal.status= act_ON;

			xQueueSend( MGR_OUTPUT_QUEUE, &datatoSend, TIMEOUT_QUEUE_OUTPUT );
			break;
		}
	}
}

//**************************************************************************************************
static void taskControl_InitPin (void)
{
	ciaaTEC_Init();
	ciaaAIN_Init();
	//ciaaTEC_EnableIRQ( TECL2 );

	ciaaGPIO_EnablePin( stateInputs[SL_OBJECT_DETECT].id_gpio, GPIO_INP_MODE );
	ciaaGPIO_EnablePin( stateInputs[SL_MODE_FUNCTION].id_gpio, GPIO_INP_MODE );
	ciaaGPIO_EnablePin( stateInputs[WATER_LEVEL].id_gpio, GPIO_INP_MODE );

	//ciaaTEC_EnableIRQ( TECL1, IRQ_GPIO_CH0 );
	//ciaaGPIO_EnableIRQ( stateInputs[SL_OBJECT_DETECT].id_gpio, IRQ_GPIO_CH1 );
	//ciaaGPIO_EnableIRQ( stateInputs[SL_MODE_FUNCTION].id_gpio, IRQ_GPIO_CH2 );
	//ciaaGPIO_EnableIRQ( stateInputs[WATER_LEVEL].id_gpio, IRQ_GPIO_CH3 );

	stateInputs[TERMOCUPLE].alarm_timer= TIMER_AL_TEMPER;
	stateInputs[AMPERIMETER].alarm_timer= TIMER_AL_TIME_OUT;
	stateInputs[CONDUCTIMETER].alarm_timer= TIMER_AL_CONDUCT;

	stateInputs[WATER_LEVEL].alarm_timer= TIMER_AL_LEVEL;
	stateInputs[SW_START_STOP].alarm_timer= TIMER_END_PROCCESS;

	xTimerStart( TIMER_ANALOGS_UPD, 0 );
	xTimerStart( TIMER_DEBOUNCE_GPIO, 2 );
	xTimerStart( TIMER_DEBOUNCE, 2 );

	/*
	xTimerStart( TIMER_ANALOGS_UPD, 0 );
	xTimerStart( TIMER_ANALOGS_UPD, 0 );
	xTimerStart( TIMER_ANALOGS_UPD, 0 );
	xTimerStart( TIMER_ANALOGS_UPD, 0 );
	xTimerStart( TIMER_ANALOGS_UPD, 0 );

#define TIMER_AL_TEMPER		TIMER_5_OBJ
#define TIMER_AL_CONDUCT	TIMER_6_OBJ
#define TIMER_AL_LEVEL		TIMER_7_OBJ
#define TIMER_AL_TIME_OUT	TIMER_8_OBJ
#define TIMER_END_PROCCESS	TIMER_9_OBJ
 */

}

static void taskControl_Actions (void)
{
	perReg_t * pR;
	actions_t* pAct;
	dInOutQueue_t outAction={.mode= portOutputDigital, .data.secTime=0};
	int i;

	for(i = 0; i < EXT_INPUTS_TOTAL; ++i)
	{
		pR= &stateInputs[i];
		pAct= pR->actions;

		if( !pR->enable )
		{
			continue;
		}
		// Deberia dejar acciones especificas para cada tarea adicionales.
		switch( pR->name )
		{
		case TERMOCUPLE:
			control_Termocuple(pAct);
			break;
		case AMPERIMETER:
			sup_integerCompare(pAct);
			control_Amperimeter(pAct);
			break;
		case CONDUCTIMETER:
			control_Conductimeter(pAct);
			break;
		case WATER_LEVEL:
			control_Level(pAct);
			break;
		case SW_START_STOP:
			control_ProcessTimer(pAct);
			break;
		case SL_OBJECT_DETECT:
			sup_statusProccess(pAct);
			control_ObjectDetect(pAct);
			break;
		case SL_MODE_FUNCTION:
			control_ModeFunction(pAct);
			sup_changeMode(pAct);
			break;
		default:
			break;
		}

		if( act_ON == pAct->alarm_signal.action && NONE != pAct->alarm_signal.id )
		{
			outAction.data.name= pAct->alarm_signal.id;
			if( act_OFF ==  pAct->alarm_signal.status )
			{
				outAction.data.value= OUT_STATUS_RESET;
				xQueueSend( MGR_OUTPUT_QUEUE, &outAction, TIMEOUT_QUEUE_OUTPUT );
			}
		}

		if( act_ON == pAct->output_signal.action && NONE != pAct->output_signal.id )
		{
			outAction.data.name= pAct->output_signal.id;
			if( act_ON ==  pAct->output_signal.status )
			{
				outAction.data.value= OUT_STATUS_SET;
			}
			else
			{
				outAction.data.value= OUT_STATUS_RESET;
			}
			xQueueSend( MGR_OUTPUT_QUEUE, &outAction, TIMEOUT_QUEUE_OUTPUT );
		}

		if( act_ON == pAct->timer_control.action && NONE != pAct->timer_control.id )
		{
			if( act_ON ==  pAct->timer_control.status )
			{
				INP_TIMER_START(pR);
			}
			else
			{
				INP_TIMER_STOP(pR);
			}
		}
	}
}

void taskControlInputs (void * a)
{
	//dOutputQueue_t dataToSend;
	dInOutQueue_t dataRecKey;
	terMsg_t msgToSend;
	int monitorMode=0;
	repAnStat_t reportAnalogInputs[ADC_TOTAL];
	uint16_t retAnalogUpdate;

	_calcSizeTypes( sizeTypes );

	taskControl_InitPin();

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_INPUT_QUEUE, &dataRecKey, TIMEOUT_QUEUE_INPUT ))
		{
			switch (dataRecKey.mode)
			{
				case portInputTecl:
					/*
					dataToSend.mode= portOutputLed;
					dataToSend.data.value=dataRecKey.data.value;
					dataToSend.data.secTime=dataRecKey.data.secTime;
					switch( dataRecKey.gpio ) // Solo a modo de prueba.
					{
						case TECL1: dataToSend.gpio= LED_R; break;
						case TECL2: dataToSend.gpio= LED_1; break;
						case TECL3: dataToSend.gpio= LED_2; break;
						case TECL4: dataToSend.gpio= LED_3; break;
						default: break;
					}
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );
					*/
				case portInputDigital:

					sup_updateDInput( &dataRecKey );

					if( RET_VAL_BUFF_FULL & sup_historyDInput( &dataRecKey ) )
					{
						xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
					}

					break;

				case portInputAnalog:

					retAnalogUpdate= ciaaAIN_Update( &reportAnalogInputs[0] );
					// Si se completa el registro de RAM se envia a la eeprom:
					if( RET_VAL_BUFF_FULL & sup_historyAInput( reportAnalogInputs, retAnalogUpdate ))
					{
						xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
					}
					break;

				case portParamtConfig:

					if( AINP_VALID(dataRecKey.gpio) )
					{
						//if(  func_conf_termistor == dataRecKey.mode ){}
						ciaaAIN_Config( dataRecKey.conf.auxvalue1, dataRecKey.conf.auxvalue2, dataRecKey.mode, dataRecKey.gpio);
					}
					else if( TECL_VALID(dataRecKey.gpio)  )
					{

					}
					break;

				case portReportConfig:
					(dataRecKey.gpio)? (monitorMode= 1): (monitorMode= 0);

					if( monitorMode )
					{
						if( pdPASS ==  xTimerStart( TIMER_REPORTS, TIMER_WAIT_TOUT )  )
						{
							Terminal_Take( &msgToSend );
							xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
						}
					}
					else
					{
						xTimerStop( TIMER_REPORTS, TIMER_WAIT_TOUT );
						Terminal_Release( &msgToSend );
						xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
					}
					break;

				case portReportStatus:

					if( sup_convertReg( &msgToSend, dataRecKey.gpio ) )
					{
						Terminal_Msg_Normal( &msgToSend, buffMonTerminal );
						xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
					}
					break;

				default:
					break;
			}
		}
		//else
		{
			/* A partir de los valores actuales de las entradas y sus configuraciones, verifica que
			 * acciones debe ejecutar en las salidas, alarmas y timers.
			 */
			taskControl_Actions();
		}

		//ACTUALIZAR_STACK( MGR_INPUT_HANDLER, MGR_INPUT_ID_STACK );
		vTaskDelay( MGR_INPUT_DELAY );
	}
}

/*==================[irq handlers functions ]=========================*/
// Utilizada para encender el debounce de las teclas:
/*
void GPIO0_IRQHandler (void)
{
	if( TEC1_PRESSED & ciaaTEC_Level_ISR( TECL1 ) )
	{
		xTimerStart( TIMER_TECLAS_INIT, 0 );
		xTimerStart( TIMER_DEBOUNCE, 0);
	}
}

// Utilizadas para las entradas digitales:
static uint16_t GPIO_IRQISR (externId_t perID, irqChId_t chID, dInOutQueue_t * objSend)
{
	objSend->gpio= stateInputs[perID].id_gpio;
	objSend->data.name= stateInputs[perID].name;

	objSend->data.value= ciaaGPIO_GetLevelIRQ( objSend->gpio, chID );

	return objSend->data.value;
}

void GPIO1_IRQHandler (void)
{
	dInOutQueue_t dataToSend= {.mode=portInputDigital};
	portBASE_TYPE xSwitchRequired;
	static uint16_t sample[2];

	sample[1] = sample[0];
	sample[0] = GPIO_IRQISR( SL_OBJECT_DETECT, IRQ_GPIO_CH1, &dataToSend );

	if ( sample[0] && sample[1] )
	{
		xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );
		portYIELD_FROM_ISR( xSwitchRequired );
	}
}

void GPIO2_IRQHandler (void)
{
	dInOutQueue_t dataToSend= {.mode=portInputDigital};
	portBASE_TYPE xSwitchRequired;
	static uint16_t sample[2];

	sample[1] = sample[0];
	sample[0] = GPIO_IRQISR( SL_MODE_FUNCTION, IRQ_GPIO_CH2, &dataToSend );

	if ( sample[0] && sample[1] )
	{
		xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );
		portYIELD_FROM_ISR( xSwitchRequired );
	}
}

void GPIO3_IRQHandler (void)
{
	dInOutQueue_t dataToSend= {.mode=portInputDigital};
	portBASE_TYPE xSwitchRequired;
	static uint16_t sample[2];

	sample[1] = sample[0];
	sample[0] = GPIO_IRQISR( WATER_LEVEL, IRQ_GPIO_CH3, &dataToSend );

	if ( sample[0] && sample[1] )
	{
		xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );
		portYIELD_FROM_ISR( xSwitchRequired );
	}
}

// Por el momento no voy a usar esta irq porque es muy sensible a los cambios por ruido en la AIN.
void ADC0_IRQHandler (void)
{
	portBASE_TYPE xSwitchRequired;

	ADC_IRQ0Support();
	portEND_SWITCHING_ISR( xSwitchRequired );
}
*/

