/*
 * IO_Manager.c
 *
 *  Created on: 2/7/2018
 *      Author: fran
 */

#include "services_config.h"
#include "terminalManager.h"

#include "ciaaGPIO_def.h"
#include "ciaaTEC.h"
#include "ciaaLED.h"
#include "ciaaAIN.h"
#include "api_RTC.h"


#define SIZE_BUFF_TERM		( 32 )
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
extern xTaskHandle 			MGR_INPUT_HANDLER;
extern xTaskHandle 			MGR_OUTPUT_HANDLER;
extern UBaseType_t*			STACKS_TAREAS;

extern xQueueHandle 		MGR_INPUT_QUEUE;
extern xQueueHandle 		MGR_OUTPUT_QUEUE;
extern xQueueHandle 		MGR_TERMINAL_QUEUE;
extern xQueueHandle 		MGR_DATALOG_QUEUE;
extern xSemaphoreHandle 	MGR_INPUT_MUTEX;

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
	int32_t		smt_max;
	int32_t		smt_min;

} perReg_t;

//**************************************************************************************************
static int sendReg=0;

static RTC_t hdRTC=
{
		.mday = 1,
		.month= 1,
		.year = 2018,
		.wday = 1,
		.hour = 0,
		.min  = 0,
		.sec  = 0
};
static char 			buffSendTerminal[SIZE_BUFF_TERM]= "vacio";
static GpioReg_t 		regToLog[REG_OPR_LOG];
static dlogPack_t 		dataToLog = { .cmd= writePage, .data= regToLog };

static perReg_t stateInputs[EXT_INPUTS_TOTAL]=
{
	{
		.name= TERMOCUPLE, 		 .id_gpio= AIN_1, .enable= TRUE, .seg_time_samples= SAMPLE_ANALOG_MSEC,
		.unit= CELSIUS, .smt_max= 70, .smt_min= 50,
	},
	{
		.name= THERMISTOR, 		 .id_gpio= AIN_2, .enable= TRUE, .seg_time_samples= SAMPLE_ANALOG_MSEC,
		.unit= CELSIUS, .smt_max= 50, .smt_min= 10,
	},
	{
		.name= AMPERIMETER, 	 .id_gpio= AIN_3, .enable= TRUE, .seg_time_samples= SAMPLE_ANALOG_MSEC,
		.unit= AMPERS, 	.smt_max= 70, .smt_min= 20,
	},
	{
		.name= CONDUCTIMETER, 	 .id_gpio= AIN_2, .enable= FALSE, .seg_time_samples= SAMPLE_ANALOG_MSEC,
		.unit= CELSIUS, .smt_max= 100, .smt_min= 0
	},
	{
		.name= WATER_LEVEL, 	 .id_gpio= AIN_3, .enable= FALSE, .seg_time_samples= SAMPLE_ANALOG_MSEC,
		.unit= CELSIUS, .smt_max= 1000, .smt_min= 800,
	},
	{
		.name= SW_START_STOP, 	 .id_gpio= TECL1, .enable= TRUE, .seg_time_samples= SAMPLE_DIGITAL_MSEC,
		.unit= LEVEL,
	},
	{
		.name= SW_INTERRUPT, 	 .id_gpio= TECL2, .enable= TRUE, .seg_time_samples= SAMPLE_DIGITAL_MSEC,
		.unit= LEVEL,
	},
	{
		.name= SL_OBJECT_DETECT, .id_gpio= TECL3, .enable= TRUE, .seg_time_samples= SAMPLE_DIGITAL_IRQ,
		.unit= LEVEL,
	},
	{
		.name= SL_MODE_FUNCTION, .id_gpio= DIN_0, .enable= TRUE, .seg_time_samples= SAMPLE_DIGITAL_IRQ,
		.unit= LEVEL,
	}
};
//**************************************************************************************************
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
	//if( !sendReg ){}
	else
	{
		if( !idReg )
		{
			if( RTC_GET_IN_TIMED() )
			{
				RTC_getTime( &hdRTC );
			}
			dataToLog.hourSamples= hdRTC.hour;
			dataToLog.minuSamples= hdRTC.min;
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

static uint8_t historyDigInput (dInOutQueue_t * regDigital)
{
	uint8_t retval=RET_VAL_NO_PROCESS;
	int i;

	if( RTC_GET_IN_TIMED() )
	{
		RTC_getTime( &hdRTC );
	}

	i= regDigital->data.name;
	stateInputs[i].enable= TRUE; // Se supone que si esta aca _> esta activo.
	// Si coiniciden los registros entonces actualiza el valor leido.
	if( (stateInputs[i].name == regDigital->data.name) && (LEVEL == stateInputs[i].unit) )
	{
		stateInputs[i].value= regDigital->data.value;
	}
	else
	{	// TODO ver que hacer cuando no coincide lo leido con lo configurado.
		stateInputs[i].name = regDigital->data.name;
		stateInputs[i].unit = LEVEL;
		stateInputs[i].id_gpio = regDigital->gpio;
		stateInputs[i].value= TECL_FREE;
	}
	stateInputs[i].time_value.minutes= hdRTC.min;
	stateInputs[i].time_value.seconds= hdRTC.sec;

	regDigital->sTime.minutes= hdRTC.min;
	regDigital->sTime.seconds= hdRTC.sec;
	//regDigital->data.secTime=0;
	// Se supone que si esta aca es porque vario su valor:
	// FIXME puedo agregarle una condiion extra, verificando si quiero historizar esta variable
	if( stateInputs[i].value )
	{
		retval= historyRegister( regDigital );
	}
	else
	{
	// FIXME Esto es una truchada por ahora, hasta que la determinacion de acciones lo haga.
	// 		 o bien la misma rutina de interrupciones ¿?
		stateInputs[i].value= TECL_FREE;
	}
	return retval;
}

static uint8_t historyAnInput (repAnStat_t * regAnalogics, uint16_t ainStatus)
{
	uint8_t retval= RET_VAL_NO_PROCESS;
	dInOutQueue_t data_record;
	uint8_t i, j;
	uint8_t reg_status;

	if( RTC_GET_IN_TIMED() )
	{
		RTC_getTime( &hdRTC );
	}

	for (j = 0; j < ADC_INPUTS; ++j)
	{
		i= regAnalogics[j].name;

		stateInputs[i].enable= regAnalogics[j].enable;
		if( (stateInputs[i].name == regAnalogics[j].name) && (stateInputs[i].unit == regAnalogics[j].unit) )
		{
			stateInputs[i].value= regAnalogics[j].value;
		}
		else
		{	// TODO ver que hacer cuando no coincide lo leido con lo configurado.
			stateInputs[i].name = regAnalogics[j].name;
			stateInputs[i].unit = regAnalogics[j].unit;
		}
		stateInputs[i].time_value.minutes= hdRTC.min;
		stateInputs[i].time_value.seconds= hdRTC.sec;

		reg_status= (uint8_t) AINP_GET_ALL(ainStatus, i);
		stateInputs[i].satuts= reg_status;

		// Solo grabo en el registro cuando la entrada varia su valor:
		// FIXME puedo agregarle una condiion extra, verificando si quiero historizar esta variable
		//if( AINP_GET_VAR(reg_status, i) )
		{
			//data_record.gpio= stateInputs[i].id_gpio; // No importa este valor

			data_record.sTime.minutes= hdRTC.min;
			data_record.sTime.seconds= hdRTC.sec;
			//data_record.data.secTime=0; // Este valor se encuentra en historyRegister.

			data_record.data.name= regAnalogics[j].name;
			data_record.data.value= regAnalogics[j].value;

			retval= historyRegister( &data_record );
		}
	}
	return retval;
}

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

//**************************************************************************************************
void taskControlOutputs (void * a)
{
	static terMsg_t msgToSend= { .mode= MP_DEF, .msg=buffSendTerminal, .size=SIZE_BUFF_TERM };
	dOutputQueue_t dataRecLed;
	uint8_t i;

	ciaaLED_Init();

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_OUTPUT_QUEUE, &dataRecLed, TIMEOUT_QUEUE_INPUT ))
		{
			if( outputLed == GET_TYPE(dataRecLed.mode) )
			{
				ciaaLED_Set( dataRecLed.gpio, dataRecLed.data.value );

				sprintf( buffSendTerminal, "[LED_%d= %i uni]\r\n", GET_ID(dataRecLed.gpio), dataRecLed.data.value );
				Terminal_Msg_Def( &msgToSend, buffSendTerminal );
				xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
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
			for(i=LED_B; i<= LED_3; i++)
			{
				ciaaLED_Set( i, FALSE );
			}
		}

		ACTUALIZAR_STACK( MGR_OUTPUT_HANDLER, MGR_OUTPUT_ID_STACK );
		vTaskDelay( MGR_OUTPUT_DELAY );

	}
}

void taskControlInputs (void * a)
{
	static dOutputQueue_t dataToSend;
	//static terMsg_t msgToSend;
	dInOutQueue_t dataRecKey;
	repAnStat_t reportAnalogInputs[ADC_INPUTS];
	uint16_t retAnalogUpdate;

	_calcSizeTypes( sizeTypes );

	ciaaTEC_Init();
	ciaaAIN_Init();
	ciaaTEC_EnableIRQ( TECL1 );
	ciaaTEC_EnableIRQ( TECL2 );

	while (1)
	{
		if( pdTRUE == xQueueReceive( MGR_INPUT_QUEUE, &dataRecKey, TIMEOUT_QUEUE_INPUT ))
		{
			if( inputTecl == GET_TYPE(dataRecKey.mode) )
			{
				dataToSend.mode= outputLed;
				dataToSend.data.value= TRUE;
			// Si presionan la tecla 1 entonces enciendo el led 1.
				if( TECL1 == dataRecKey.gpio && TECL_PUSH == dataRecKey.data.value )
				{
					dataToSend.gpio = LED_B;
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );
				}
				// Si presionan la tecla 2 entonces se envia un mensaje con el valor de la entrada analogica.
				if( TECL2 == dataRecKey.gpio && TECL_PUSH == dataRecKey.data.value )
				{
					dataToSend.gpio = LED_1;
					xQueueSend( MGR_OUTPUT_QUEUE, &dataToSend, TIMEOUT_QUEUE_OUTPUT );
				}
				// Si se completa el registro de RAM se envia a la eeprom:
				if( RET_VAL_BUFF_FULL & historyDigInput( &dataRecKey ) )
				{
					xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
				}
			}

			if( inputConfig == GET_TYPE(dataRecKey.mode) )
			{
				uint8_t id= dataRecKey.gpio;

				if( AINP_VALID(id) )
				{
					//if(  func_conf_termistor == dataRecKey.mode ){}
					ciaaAIN_Config( dataRecKey.conf.auxvalue1, dataRecKey.conf.auxvalue2, dataRecKey.mode, dataRecKey.gpio);
				}
				else if( TECL_VALID(id)  )
				{

				}

			}
		}
		else // FIXME ESTO TENGO QUE USAR TIMERS DE FREERTOS O DEL SISTEMA. no es constante para nada!!!!
		{
			/*
			sprintf( buffSendTerminal, "[AI_%d= %i uni]\r\n", AIN_1, ADC_read(AIN_1) );
			Terminal_Msg_Def( &msgToSend, buffSendTerminal );
			xQueueSend( MGR_TERMINAL_QUEUE, &msgToSend, TIMEOUT_QUEUE_MSG_OUT );
			 */
			retAnalogUpdate= ciaaAIN_Update( &reportAnalogInputs[0] );
			// Si se completa el registro de RAM se envia a la eeprom:
			if( RET_VAL_BUFF_FULL & historyAnInput( reportAnalogInputs, retAnalogUpdate ))
			{
				xQueueSend( MGR_DATALOG_QUEUE, &dataToLog, TIMEOUT_QUEUE_OUTPUT );
			}
		}
		// FIXME EN VEZ DE ESTE SISTEMA TENGO QUE USAR DEBOUNCE CON TIMERS PARA LOS DEMAS ENTRADAS BINARIAS. O USO TODAS LAS IRQ.
		// TODO UNA VEZ RECOGIDOS LOS DATOS, ESTOS SE GRABARAN EN UNA TABLA GENERAL. AHI ESTARAN LAS ACCIONES CORRESPONDIENTES
		// 		A REALIZAR CON LOS CAMBIOS EN ELLAS. USARE UNA MATRIZ DE RELACION ENTRE DISPAROS DE LAS VARIABLES PARA ACCIONAR
		// 		LAS SALIDAS. LOS PARAMETROS DE TIEMPO POR ETAPA LOS TRABAJARA LAS SALIDAS.
		// 		LAS ACCIONES SE ENVIARAN VARIAS JUNTAS EN LA COLA DEL MODULO DE SALIDA. IDEM CON LA COLA DE GRABADO Y DEL TERMINAL.

		ACTUALIZAR_STACK( MGR_INPUT_HANDLER, MGR_INPUT_ID_STACK );
		vTaskDelay( MGR_INPUT_DELAY );
	}
}

/*==================[irq handlers functions ]=========================*/
void GPIO0_IRQHandler (void)
{
	static dInOutQueue_t dataToSend= {.mode=inputTecl, .gpio=TECL1};
	portBASE_TYPE xSwitchRequired;

	dataToSend.data.name= SW_START_STOP; // FIXME esta correspondencia no puede ser forzada, debe salir de la tabla.
	dataToSend.data.value= TECL_FREE;

	if( TEC1_PRESSED & ciaaTEC_Level_ISR( TECL1 ) )
	{
		dataToSend.data.value= TECL_PUSH;
	}
	xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );

	portYIELD_FROM_ISR( xSwitchRequired );
}

void GPIO1_IRQHandler (void)
{
	static dInOutQueue_t dataToSend= {.mode=inputTecl, .gpio=TECL2};
	portBASE_TYPE xSwitchRequired;

	dataToSend.data.name= SW_INTERRUPT; // FIXME esta correspondencia no puede ser forzada, debe salir de la tabla.
	dataToSend.data.value= TECL_FREE;

	if( TEC2_PRESSED & ciaaTEC_Level_ISR( TECL2 ) )
	{
		dataToSend.data.value= TECL_PUSH;
	}
	xQueueSendFromISR( MGR_INPUT_QUEUE, &dataToSend, &xSwitchRequired );

	portYIELD_FROM_ISR( xSwitchRequired );
}

// Por el momento no voy a usar esta irq porque es muy sensible a los cambios por ruido en la AIN.
void ADC0_IRQHandler (void)
{
	/*
	portBASE_TYPE xSwitchRequired;

	ADC_IRQ0Support();
	portEND_SWITCHING_ISR( xSwitchRequired );
	*/
}

/*

#define SIZE_ARRAY_BUFF		MGR_DATALOG_QUEUE_LENGT
#define SIZE_ITEMS_BUFF		25
#define MSG_LOG_DINPUT		"[DI_%d=%i state]"

typedef struct _linkList
{
	uint8_t** ptrBuff;
	uint8_t** ptrLast;
	uint16_t posDisp;
	uint8_t posNext;
	uint8_t sizeRow;

} linkList_t;

static linkList_t 	bufferState;
static uint8_t 		byteConteiner[SIZE_ARRAY_BUFF][SIZE_ITEMS_BUFF];

static inline void initLinkList (linkList_t * pMsgList, uint8_t** pConteiner, uint16_t sizeConteiner)
{
	pMsgList->ptrBuff= pConteiner;
	pMsgList->ptrLast= pConteiner;
	pMsgList->posDisp= sizeConteiner;
	pMsgList->posNext= 0;
	pMsgList->sizeRow= SIZE_ITEMS_BUFF;
}

static int insertMsgToList (uint8_t gpioID, uint16_t value, genStr_t* retMsg, linkList_t* pMsgList)
{
	int retval=0;

	if( retMsg && pMsgList )
	{
		if( pMsgList->posDisp <= pMsgList->posNext )
		{
			pMsgList->posNext= 0;
		}
		pMsgList->ptrLast= (pMsgList->ptrBuff)+pMsgList->posNext*pMsgList->sizeRow;
		pMsgList->posNext++;

		retMsg->msg = (char *) pMsgList->ptrLast;
		retMsg->size= (uint16_t) sprintf( retMsg->msg, MSG_LOG_DINPUT, gpioID, value );
		retMsg->size++;

		retval= 1;
	}
	return retval;
}

static void quitMsgToList (linkList_t * pMsgList)
{
	if( NULL != pMsgList->ptrBuff )
	{
		pMsgList= (linkList_t*)(pMsgList->ptrBuff);
		pMsgList->ptrLast= NULL;
	}
}
*/

