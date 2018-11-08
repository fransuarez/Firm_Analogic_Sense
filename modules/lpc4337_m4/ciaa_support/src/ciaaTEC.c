/*
 * ciaaKEY.c
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#include "ciaaGPIO_def.h"
#include "api_GPIO.h"
#include "ciaaTEC.h"

// ------ Private constants -----------------------------------

#define TotalCalls 		10000
#define KEY_INP_PASS 	TEC3
#define TECL_TOTAL	 	4

// ------ Private variable ------------------------
#include "chip.h"

#define IRQ_PIN_INT0	0
#define IRQ_PIN_INT1	1
#define IRQ_PIN_INT2	2
#define IRQ_PIN_INT3	3

// ------ Public variable ------------------------
typedef struct config_tecl
{
	bool		enableIRQ;
	uint8_t		idIRQ;
	bool		enableDebounce;
	uint8_t		Test0;
	uint8_t		Test1;
	uint8_t		Test2;
	uint8_t		Test3;
} teclCfg_t;

static const io_port_t 	gpioBtnBits[TECL_TOTAL] =
{
	{GPIO_PORT_TEC1, GPIO_NUMB_TEC1},
	{GPIO_PORT_TEC2, GPIO_NUMB_TEC2},
	{GPIO_PORT_TEC3, GPIO_NUMB_TEC3},
	{GPIO_PORT_TEC4, GPIO_NUMB_TEC4}
};

static teclCfg_t teclStatus[TECL_TOTAL]=
{
	{
		.idIRQ= IRQ_PIN_INT0
	},
	{
		.idIRQ= IRQ_PIN_INT1
	},
	{
		.idIRQ= IRQ_PIN_INT2
	},
	{
		.idIRQ= IRQ_PIN_INT3
	}
};

// ------ Public functions  ----------------------------------------

void ciaaTEC_Init (void)
{
	uint8_t i;
	const dig_port_t pin_config[TECL_TOTAL] =
	{
		{PIN_PORT_TEC1, PIN_NUMB_TEC1, PIN_FUNCION_0},	// TEC1 -> P1_0
		{PIN_PORT_TEC2, PIN_NUMB_TEC2, PIN_FUNCION_0},	// TEC2 -> P1_1
		{PIN_PORT_TEC3, PIN_NUMB_TEC3, PIN_FUNCION_0},	// TEC3 -> P1_2
		{PIN_PORT_TEC4, PIN_NUMB_TEC4, PIN_FUNCION_0} 	// TEC4 -> P1_6
	};

	for ( i = 0; i < TECL_TOTAL; i++)
	{
		GPIO_EnablePin( pin_config[i].pinPort, pin_config[i].pinNumber, pin_config[i].function,
						gpioBtnBits[i].pinPort, gpioBtnBits[i].pinNumber, GPIO_INP_MODE );
	}
}

uint8_t ciaaTEC_Status (tecId_t key_id)
{
	uint8_t gpioBtnIDs= TECL_RELEASE;
	uint8_t i;


	if( TECL_TOTAL > key_id )
	{
		if ( !GPIO_GetLevel( gpioBtnBits[key_id].pinPort, gpioBtnBits[key_id].pinNumber ))
		{
			gpioBtnIDs = (1<<key_id);
		}
	}
	else
	{
		for ( i = 0; i < TECL_TOTAL; i++ )
		{
			if ( !GPIO_GetLevel( gpioBtnBits[i].pinPort, gpioBtnBits[i].pinNumber ))
			{
				gpioBtnIDs |= (1<<i);
			}
		}
	}
	return gpioBtnIDs;
}

void ciaaTEC_EnableIRQ (tecId_t key_id)
{
	GPIO_InputIRQEnable( gpioBtnBits[key_id].pinPort, gpioBtnBits[key_id].pinNumber, teclStatus[key_id].idIRQ );
}

void ciaaTEC_DisableIRQ (tecId_t key_id)
{
	GPIO_InputIRQDisable( teclStatus[key_id].idIRQ );
}

void ciaaTEC_DebounInit (tecId_t key_id)
{
	uint8_t i;

	if( TECL_TOTAL > key_id )
	{
		teclStatus[key_id].Test0= TECL_FREE;
		teclStatus[key_id].Test1= TECL_FREE;
		teclStatus[key_id].Test2= TECL_FREE;
		teclStatus[key_id].Test3= TECL_FREE;
	}
	else
	{
		for (i=0; i < TECL_TOTAL; ++i)
		{
			teclStatus[i].Test0= TECL_FREE;
			teclStatus[i].Test1= TECL_FREE;
			teclStatus[i].Test2= TECL_FREE;
			teclStatus[i].Test3= TECL_FREE;
		}
	}
}

uint8_t ciaaTEC_DebounStatus (tecId_t key_id)
{
	uint8_t i;
	uint8_t retval= TECL_RELEASE;

	if( TECL_TOTAL > key_id )
	{
		// Actualizar muestras
		teclStatus[key_id].Test3 = teclStatus[key_id].Test2;
		teclStatus[key_id].Test2 = teclStatus[key_id].Test1;
		teclStatus[key_id].Test1 = teclStatus[key_id].Test0;

		teclStatus[key_id].Test0 = GPIO_GetLevel(gpioBtnBits[key_id].pinPort, gpioBtnBits[key_id].pinNumber);

		if ((TECL_FREE == teclStatus[key_id].Test3) && (TECL_FREE == teclStatus[key_id].Test2) &&
			(TECL_PUSH == teclStatus[key_id].Test1) && (TECL_PUSH == teclStatus[key_id].Test0))
		{
			// Flanco de bajada detectado.
			retval= (1<<key_id);
		}
	}
	else
	{
		for (i = 0; i < TECL_TOTAL; ++i)
		{
			// Actualizar muestras
			teclStatus[i].Test3 = teclStatus[i].Test2;
			teclStatus[i].Test2 = teclStatus[i].Test1;
			teclStatus[i].Test1 = teclStatus[i].Test0;

			teclStatus[i].Test0 = GPIO_GetLevel(gpioBtnBits[i].pinPort, gpioBtnBits[i].pinNumber);

			if ((TECL_FREE == teclStatus[i].Test3) && (TECL_FREE == teclStatus[i].Test2) &&
				(TECL_PUSH == teclStatus[i].Test1) && (TECL_PUSH == teclStatus[i].Test0))
			{
				// Flanco de bajada detectado.
				retval |= 1<<i;
			}
		}
	}
	return retval;
}

uint8_t ciaaTEC_Level_ISR (tecId_t key_id)
{
	uint8_t i;
	uint8_t retval=TECL_RELEASE;

	if( TECL1_4 > key_id )
	{
		if( GPIO_InputIRQHandler( teclStatus[key_id].idIRQ ) )
		{
			retval = (1<<key_id);
		}
	}
	else
	{
		for (i = 0; i < TECL_TOTAL; ++i)
		{
			if( GPIO_InputIRQHandler( teclStatus[i].idIRQ ) )
			{
				retval |= (1<<i);
			}
		}
	}
	return retval;
}



