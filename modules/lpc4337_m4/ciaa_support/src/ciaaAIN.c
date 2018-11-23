/*
 * ciaaAIN.c
 *
 *  Created on: Nov 20, 2018
 *      Author: fran
 */

#include "ciaaGPIO_def.h"
#include "api_ADC.h"

#include "temperaturaMath.h"

// *************************************************************************************************
#define ADC_INPUTS		( 3 )
#define ADC_REGISTERS	( 10 )

#define AIN_LEVEL_NORM	( 0 )
#define AIN_LEVEL_INF	( 1 )
#define AIN_LEVEL_SUP	( 2 )
#define AIN_FUNC_WIND	( 0 )
#define AIN_FUNC_INTEGER ( 1 )

#define MASK_ENABLE		( 0x01 )

#define ADC_INDEX_RENEW(X) ((ADC_REGISTERS <= X)? 0: ++X)
#define SET_ENABLE(X,V)	( (TRUE==V)? (X=MASK_ENABLE): X=(~MASK_ENABLE) )
#define GET_ENABLE(X)	( (MASK_ENABLE & X)? TRUE: FALSE )


// *************************************************************************************************

typedef struct _conversor_params
{
	uint16_t max;		// Valor maximo posible > ADC_COTA_SUP
	uint16_t min;		// Valor minimo posible > ADC_COTA_INF
	uint16_t inp_1;
	uint16_t inp_2;
	int32_t out;

} convParm_t;

typedef uint8_t (* pProcFunc_t)(convParm_t *);
typedef uint8_t (* pMathFunc_t)(void *  );

typedef struct _sensor_params
{
	sensor_t 	name;
	unit_t   	unit_input;
	unit_t   	unit_output;
	pProcFunc_t converter;

} sensDef_t;

typedef struct _analog_state
{
	//uint8_t channel;
	uint8_t 	change;
	uint8_t 	last;
	uint16_t 	values[ADC_REGISTERS];

} analog_t;

typedef struct _analog_register
{
	uint8_t 	enable;
	convParm_t 	calc_values;
	sensDef_t* 	sens_type;
	analog_t* 	state;

} regAnInp_t;

// *************************************************************************************************
static uint8_t convVoltTermocuple (convParm_t* );
static uint8_t convVoltThermistor (convParm_t* );
static uint8_t convSigAmperimeter (convParm_t*);
static uint8_t convSigConductimeter (convParm_t*);
static uint8_t convSigWaterLevel (convParm_t* );

// *************************************************************************************************
static temp_t actualTemp=25;
static analog_t adcSatus[ADC_INPUTS];

static sensDef_t sensor_descript[]=
{
	{ .name= TERMOCUPLE		, .unit_input=uVOLTS , .unit_output=CELSIUS	, .converter= convVoltTermocuple 	},
	{ .name= THERMISTOR		, .unit_input=OHMS	 , .unit_output=CELSIUS	, .converter= convVoltThermistor 	},
	{ .name= AMPERIMETER	, .unit_input=mAMPERS, .unit_output=AMPERS	, .converter= convSigAmperimeter 	},
	{ .name= CONDUCTIMETER	, .unit_input=uFARADS, .unit_output=mgSQRMTR, .converter= convSigAmperimeter 	},
	{ .name= WATER_LEVEL	, .unit_input=mAMPERS, .unit_output=mMETERS	, .converter= convSigAmperimeter 	}
};

static regAnInp_t analogInputs[ADC_INPUTS]=
{
	{	.enable			= TRUE,
		.state			= &adcSatus[0],
		.sens_type		= &sensor_descript[THERMISTOR],
		.calc_values.min= 10,
		.calc_values.max= 50000	// Valor de 50Kohms limitacion del posible modelo fisico
	},
	{ 	.enable			=TRUE,
		.state			= &adcSatus[1],
		.sens_type		= &sensor_descript[TERMOCUPLE],
		.calc_values.min= 0,
		.calc_values.max= 5000	// Valor max acotado de la tabla en milivolts
	},
	{ 	.enable			=TRUE,
		.state			= &adcSatus[2],
		.sens_type		= &sensor_descript[CONDUCTIMETER],
		.calc_values.min= 0,
		.calc_values.max= 100
	}
};

// *************************************************************************************************
/*
static uint8_t proccWindow(void * data)
{
	regAnInp_t* reg= (regAnInp_t*) data;
	uint16_t  value= reg->state->values[reg->state->last];
	uint8_t retval=0;

	if( (reg->min)>=value )
	{
		retval= AIN_LEVEL_INF;
	}
	else if( (reg->max)<value )
	{
		retval= AIN_LEVEL_SUP;
	}

	return retval;
}

static uint8_t proccInteger(void * data)
{
	regAnInp_t* reg= (regAnInp_t*) data;
	uint16_t  value= reg->state->values[reg->state->last];
	uint8_t retval=0;

	reg->state->acum += value;

	if( (reg->max)<(reg->state->acum) )
	{
		retval= AIN_LEVEL_SUP;		// Devuelve el estado de alarma
	}

	return retval;
}
*/
static void insertValue(analog_t * reg, uint16_t value)
{
	uint8_t pos= reg->last;

	if( reg->values[pos] != value )
	{
		reg->change= TRUE;
	}
	else
	{
		reg->change= FALSE;
	}
	reg->values[pos]= value;

	ADC_INDEX_RENEW( reg->last );
	// ADC_insertValue( &adcSatus[chId], value );
}


static uint8_t convVoltTermocuple (convParm_t* argum)
{
	uint8_t retval= 0;
	temp_t temp_aux;
	volt_t volt_aux;

	argum->inp_2= actualTemp;
	volt_aux = (volt_t)(argum->inp_1);
	volt_aux *= (volt_t)(argum->max-argum->min)/(ADC_COTA_SUP*1000);

	temp_aux= tcuplaKVoltToTemp( volt_aux, argum->inp_2 );

	if( ERROR_TEMP != temp_aux )
	{
		argum->out= (int32_t) temp_aux;
		retval= 1;
	}
	return retval;
}

static uint8_t convVoltThermistor (convParm_t* argum)
{
	// Estos parametros deberan de quedar en eeprom
	/*	Puntos calibracion NTC termistor:
	 * 	temp ambiente		resistencia
		@20 				R 10000
		@10					R 15000
		@2					R 25000  < Para modelo Sheinhart
	 */
	static cntc_t coef_thermistor={.A=0, .B=0, .C=0};
	static temp_t temp_calib[PTOS_CALIB_TERM]={20,10};
	static resi_t rest_calib[PTOS_CALIB_TERM]={10000,15000};
	static uint8_t id_calib=2;

	uint8_t retval= 0;
	temp_t temp_aux;
	resi_t rest_aux;

	rest_aux= (resi_t)(argum->inp_1);
	rest_aux= (resi_t)(rest_aux*((float)(argum->max-argum->min)/(ADC_COTA_SUP)));

	temp_aux= tstorResToTemp( rest_aux, &coef_thermistor );

	if( ERROR_TEMP != temp_aux )
	{
		argum->out= (int32_t) temp_aux;
		actualTemp= argum->out;
		retval= 1;
	}
	else
	{
		if( 1<id_calib )
		{
			if( tstorCalcCoef( rest_calib, temp_calib, &coef_thermistor ) )
			{
				retval= 1;
			}
			id_calib=0;
		}
		else
		{
			rest_calib[id_calib]= (resi_t)(argum->inp_1);
			temp_calib[id_calib]= (temp_t)(argum->inp_2);
			id_calib++;
		}
	}
	return retval;
}

static uint8_t convSigAmperimeter (convParm_t* argum)
{
	uint8_t retval= 1;
	resi_t rest_aux;

	rest_aux= (resi_t)(argum->inp_1);
	rest_aux= (resi_t)(rest_aux*((float)(argum->max-argum->min)/(ADC_COTA_SUP)));

	argum->out= (int32_t) rest_aux;

	return retval;
}

static uint8_t convSigConductimeter (convParm_t* argum)
{
	uint8_t retval= 1;


	return retval;
}

static uint8_t convSigWaterLevel (convParm_t* argum)
{
	uint8_t retval= 1;

	return retval;
}



// **********************************************************************************
void ciaaAIN_Init(void)
{/*
	adcSatus[0].channel= ADC_CH1;
	adcSatus[1].channel= ADC_CH2;
	adcSatus[2].channel= ADC_CH3;

	SET_ENABLE(analogInputs[0].enable, TRUE);
	SET_ENABLE(analogInputs[1].enable, TRUE);
	SET_ENABLE(analogInputs[2].enable, TRUE);
*/

	ADC_init();
}

// FIXME: Estos parametros deberan de quedar en eeprom!!!

uint8_t ciaaAIN_Config (uint16_t val_1, uint16_t val_2, uint8_t mode, uint8_t id_gpio)
{
	uint8_t retval=0;
	uint8_t id;

	if( AINP_VALID(id_gpio) )
	{
		id=	AINP_INDEX(id_gpio);

		switch (mode) {
			case ain_cotas_conversion:
				analogInputs[id].calc_values.max= val_1;
				analogInputs[id].calc_values.min= val_2;
				retval=1;
				break;
			case ain_calibracion:
				analogInputs[id].calc_values.inp_1= val_1; //Aca va el valor de resistencia.
				analogInputs[id].calc_values.inp_2= val_2; //Aca va el valor de temperatura.

				if( THERMISTOR == analogInputs[id].sens_type->name )
				{
					if( convVoltThermistor( &analogInputs[id].calc_values ) )
					{
						retval=1; //El parametro de calibracion fue correcto.
					}
				}
				break;
			case ain_habilitacion:
				//SET_ENABLE(analogInputs[id].enable,(val_1)? TRUE: FALSE) ;
				analogInputs[id].enable= ((val_1)? TRUE: FALSE);
				retval=1;
				break;
			case ain_reasignacion:
				if( SENSOR_VALID(val_1) )
				{
					analogInputs[id].sens_type= &sensor_descript[val_1];
					analogInputs[id].calc_values.min= 0;
					analogInputs[id].calc_values.max= 100;
					retval=1;
				}
				break;
			default:
				break;
		}
	}
	return retval;
}

uint8_t ciaaAIN_Update (void)
{
	uint16_t* values;
	uint8_t i;
	uint8_t retval=0;
	//uint8_t retaux;

	values= ADC_refresh();

	for (i = 0; i < ADC_INPUTS; ++i)
	{
		if( GET_ENABLE( analogInputs[i].enable ))
		{
			insertValue( analogInputs[i].state, values[i]);
			//modifyFlags( retaux, &retval );

			analogInputs[i].calc_values.inp_1= values[i];

			if( analogInputs[i].sens_type->converter( &analogInputs[i].calc_values ))
			{
				retval++;
			}

		}
	}
	return retval;
}

void ciaaAIN_Reset (void)
{
	uint8_t i;

	for (i = 0; i < ADC_INPUTS; ++i)
	{
		if( GET_ENABLE( analogInputs[i].enable ))
		{
			analogInputs[i].state->last=0;
			//SET_SUPMAX(analogInputs[i].enable, FALSE);
			//SET_SUPMIN(analogInputs[i].enable, FALSE);
		}
	}
}

