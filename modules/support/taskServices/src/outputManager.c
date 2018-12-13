/*
 * IO_Manager.c
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#include "services_config.h"
#include "terminalManager.h"
#include "api_RTC.h"

#include "ciaaPORT.h"
#include "ciaaGPIO_def.h"
#include "ciaaLED.h"
#include "ciaaTEC.h"  // Esta lib despues la voy a reemplazar por ciaaGPIO.h


#define SIZE_BUFF_TERM		( 32 )
#define SIZE_MON_TERM		( 128 )

#define SAMPLE_ANALOG_MSEC	( 15*1000 ) //Viene de
#define SAMPLE_DIGITAL_MSEC	( 200 )
#define SAMPLE_DIGITAL_IRQ	( 0 )

#define SAMPLE_MAX_SEC		( 255 ) // Viene de (1<<7)-1 =255 == 4'15"
#define SAMPLE_MAX_MINUTES	( 4 ) 	// Viene de 2^7-1= 255/60= 4,25-> 4'
#define SAMPLE_MAX_SECONDS	( 15 ) 	// Viene de 60"+15"

#define RET_VAL_NO_PROCESS  ( 0 )
#define RET_VAL_OK			( 1 )
#define RET_VAL_BUFF_FULL	( 2 )
#define RET_VAL_ERR_QUEUE	( 4 )

//**************************************************************************************************
extern QueueHandle_t 		MGR_INPUT_QUEUE;
extern QueueHandle_t 		MGR_OUTPUT_QUEUE;
extern QueueHandle_t 		MGR_TERMINAL_QUEUE;
extern QueueHandle_t 		MGR_DATALOG_QUEUE;

extern SemaphoreHandle_t 	MGR_INPUT_MUTEX;

//extern UBaseType_t*			STACKS_TAREAS;
//extern TaskHandle_t 		MGR_OUTPUT_HANDLER;


//**************************************************************************************************
typedef struct periferics_register
{
	externId_t 	name;
	perif_t		id_gpio;
	uint8_t		enable;
	uint8_t		satuts;		// CHANGE, MAXIMO, MINIMO, ERROR > b0000
	uint32_t	seg_time_samples;

	unit_t		unit;
	shTime_t	time_value;
	int32_t		value;

} perReg_t;

typedef struct periferics_ids
{
	externId_t 	id;
	const char * name;
	const char * unit;

} perInfo_t;

//**************************************************************************************************
//static int sendReg=0;

static char 			buffSendTerminal[SIZE_BUFF_TERM]= "";
static char 			buffMonTerminal[SIZE_MON_TERM]= "";
static GpioReg_t 		regToLog[REG_OPR_LOG];
static dlogPack_t 		dataToLog = { .cmd= writePage, .data= regToLog };
static RTC_t hdRTC;

static perInfo_t nombOutputs[EXT_OUTPUTS_TOTAL]=
{
	{ .id= RELAY_VALVE		, .name= "R_VALVE"	, .unit= "LEVEL" 	},
	{ .id= RELAY_HEATER		, .name= "R_HEATER" , .unit= "LEVEL" 	},

	{ .id= ALARM_TEMPER		, .name= "A_TEMP"	, .unit= "LEVEL"  	},
	{ .id= ALARM_LEVEL		, .name= "A_LEVEL"	, .unit= "LEVEL" 	},
	{ .id= ALARM_CONDUCT	, .name= "A_COND"	, .unit= "LEVEL" 	},
	{ .id= ALARM_INTERRUPT	, .name= "A_INTERR"	, .unit= "LEVEL" 	},
	{ .id= ALARM_OVER_TIME	, .name= "A_OVER_TIME" , .unit= "LEVEL" },

	{ .id= SIGNAL_PROCCESS	, .name= "S_PROCC"	, .unit= "LEVEL"  	},
	{ .id= SIGNAL_COMPLETE	, .name= "S_COMPL"	, .unit= "LEVEL" 	},
	{ .id= SIGNAL_MODE		, .name= "S_MODE"	, .unit= "LEVEL" 	}
};

static perReg_t stateOutputs[EXT_OUTPUTS_TOTAL]=
{
	{	.name= RELAY_VALVE		, .id_gpio= SPI_MOSI, .enable= TRUE, .unit= LEVEL  	},
	{ 	.name= RELAY_HEATER		, .id_gpio= LCD_RS, .enable= TRUE, .unit= LEVEL		},

 	{ 	.name= ALARM_TEMPER		, .id_gpio= LCD_4, .enable= TRUE, .unit= LEVEL	},
	{	.name= ALARM_LEVEL		, .id_gpio= LCD_3, .enable= TRUE, .unit= LEVEL	},
	{	.name= ALARM_CONDUCT	, .id_gpio= LCD_EN, .enable= TRUE, .unit= LEVEL},
	{	.name= ALARM_INTERRUPT	, .id_gpio= LED_R, .enable= TRUE, .unit= LEVEL  },
	{ 	.name= ALARM_OVER_TIME	, .id_gpio= LED_1, .enable= TRUE, .unit= LEVEL	},

 	{ 	.name= SIGNAL_PROCCESS	, .id_gpio= LED_2, .enable= TRUE, .unit= LEVEL	},
	{	.name= SIGNAL_COMPLETE	, .id_gpio= LED_3, .enable= TRUE, .unit= LEVEL	},
	{	.name= SIGNAL_MODE		, .id_gpio= LCD_2, .enable= TRUE, .unit= LEVEL},
};
#define CANT_AUX_LEDS 	6
#define OUTPUT_GET_INDEX(X)	( X-RELAY_VALVE )
static perif_t testOutput[CANT_AUX_LEDS]=
{
	LCD_EN, SPI_MOSI, LCD_3, LCD_RS, LCD_4, LCD_2
};

//**************************************************************************************************

static void taskOutputs_InitPin (void)
{
	int i;
	ciaaLED_Init();

	for (i = 0; i < CANT_AUX_LEDS; ++i)
	{
		ciaaGPIO_EnablePin( testOutput[i], GPIO_OUT_MODE );
		ciaaGPIO_SetLevel( testOutput[i], GPIO_LOW_LEVEL);

	}
	//xTimerStart( TIMER_TECLAS_INIT, 2 );
}
// TODO agregarla alguna libreria de manejo de tiempos. Lo unico que deberia de pasarle extra es 4' y 15".
static uint8_t calculateDiffTime (uint8_t initMin, uint8_t initSec, uint8_t endMin, uint8_t endSec)
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

static uint8_t historyRegister (dInOutQueue_t* paqInfoGPIO)
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
			histRegGpio.secTime= calculateDiffTime( minute_init, second_init, paqInfoGPIO->sTime.minutes, paqInfoGPIO->sTime.seconds );
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

static uint8_t updateDigOutput (dInOutQueue_t * regDigital)
{
	uint8_t retval= RET_VAL_OK;
	int i= OUTPUT_GET_INDEX(regDigital->data.name);

	if( RTC_GET_IN_TIMED() )
	{
		RTC_getTime( &hdRTC );
		regDigital->sTime.minutes= hdRTC.min;
		regDigital->sTime.seconds= hdRTC.sec;
	}

	stateOutputs[i].enable= TRUE; // Se supone que si esta aca _> esta activo.
	stateOutputs[i].time_value.minutes= regDigital->sTime.minutes;
	stateOutputs[i].time_value.seconds= regDigital->sTime.seconds;
	stateOutputs[i].value= regDigital->data.value;

	// Si coiniciden los registros entonces actualiza el valor leido.
	// FIXME esto debe ir en la funcion de configuracion. A lo sumo debe devolver error.
	if( (stateOutputs[i].name != regDigital->data.name) || (LEVEL != stateOutputs[i].unit) )
	{
		stateOutputs[i].name = regDigital->data.name;
		stateOutputs[i].unit = LEVEL;
		stateOutputs[i].id_gpio = regDigital->gpio;
	}
	else
	{
		// Es medio pedorro que este acá, lo hago para fines de alguna rutina posterior que lo necesite.
		regDigital->gpio= stateOutputs[i].id_gpio;
	}

	return retval;
}

//**************************************************************************************************
void timerAlarmCallback (TimerHandle_t pxTimer)
{
	static int i=0;
	//static terMsg_t msgToSend= { .mode= MP_PRINT_NORMAL, .msg=buffMonTerminal, .size=SIZE_MON_TERM };

	if( EXT_OUTPUTS_TOTAL <= i )
	{
		i= 0;
	}
/*
	sprintf( buffMonTerminal, "ID:%s UN:%s HAB:%i V:%i %i'%i\"", nombInputs[i].name, nombInputs[i].unit, stateOutputs[i].enable,
			 stateInputs[i].value, stateInputs[i].time_value.minutes, stateInputs[i].time_value.seconds );

	Terminal_Msg_Normal( &msgToSend, buffMonTerminal );
	xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
*/
	i++;
}

void taskControlOutputs (void * a)
{
	static terMsg_t msgToSend= { .mode= MP_PRINT_NORMAL, .msg=buffSendTerminal, .size=SIZE_BUFF_TERM };
	dOutputQueue_t dataRecLed;
	uint8_t i;

	taskOutputs_InitPin();

	// En el caso de alarmas puedo activarles un blinky con un timer.
	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_INPUT ))
		{
			switch (dataRecLed.mode)
			{
			case portOutputLed:
			case portOutputDigital:

				updateDigOutput( &dataRecLed );

				if( RET_VAL_BUFF_FULL & historyRegister( &dataRecLed ) )
				{
					xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
				}

				ciaaGPIO_SetLevel( dataRecLed.gpio, dataRecLed.data.value );

				sprintf( buffSendTerminal, "[Out:%s  State:%i]\r\n",
						 nombOutputs[OUTPUT_GET_INDEX(dataRecLed.data.name)].name, dataRecLed.data.value );
				Terminal_Msg_Normal( &msgToSend, buffSendTerminal );
				xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
				/*
				static uint8_t statLeds=GPIO_LOW_LEVEL;

				for (i = 0; i < CANT_AUX_LEDS; ++i)
				{
					ciaaGPIO_SetLevel( testOutput[i], statLeds );
				}
				statLeds= ~statLeds;
				*/
				break;
			default:
				break;
			}
			/*
			if( historyDigInput( &dataRecLed ) )
			{
				xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
			}
			 */
		}
		else
		{
			/*for(i=LED_R; i<= LED_3; i++)
			{
				ciaaLED_Set( i, FALSE );
			}*/
		}

		//ACTUALIZAR_STACK( MGR_OUTPUT_HANDLER, MGR_OUTPUT_ID_STACK );
		vTaskDelay( MGR_OUTPUT_DELAY );

	}
}

/*==================[irq handlers functions ]=========================*/
// Por el momento no voy a usar esta irq porque es muy sensible a los cambios por ruido en la AIN.


