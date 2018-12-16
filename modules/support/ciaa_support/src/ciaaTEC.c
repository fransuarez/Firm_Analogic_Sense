/*
 * ciaaKEY.c
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

//#include "ciaaGPIO_def.h"
//#include "chip.h"
#include "lpc_types.h"
#include "api_GPIO.h"

//#include "ciaaPORT.h"
#include "ciaaTEC.h"

// ------ Private constants -----------------------------------
#define TECL_INDEX(X)	( X-TECL1 )
#define GPIO_INDEX(X)	( X-GPIO_0 )

// ------ Public variable ------------------------
typedef struct config_tecl
{
	bool		enableIRQ;
	bool		enableDebounce;
	irqChId_t	idIRQ;
	uint8_t		Test0;
	uint8_t		Test1;
	uint8_t		Test2;
	uint8_t		Test3;

} debGPIO_t;

static debGPIO_t teclStatus[TECL_TOTAL];

static debGPIO_t gpioStatus[GPIO_TOTAL];

// ------ Public functions  ----------------------------------------

void ciaaTEC_Init (void)
{
	uint8_t i;

	for ( i=TECL1; TECL4 >= i; i++)
	{
		GPIO_EnablePin( pin_config[i].pinPort,  pin_config[i].pinNumber,  pin_config[i].function,
						pin_config[i].gpioPort, pin_config[i].gpioNumber, GPIO_INP_MODE );
	}
}

tecReg_t ciaaTEC_Status (perif_t key_id)
{
	tecReg_t gpioBtnIDs= TECL_RELEASE;
	uint8_t id;

	if( TECL_VALID(key_id) )
	{
		if ( !GPIO_GetLevel( pin_config[key_id].gpioPort, pin_config[key_id].gpioNumber ))
		{
			id= TECL_INDEX(key_id);
			gpioBtnIDs = (1<<id);
		}
	}
	else
	{
		for ( id = 0; id < TECL_TOTAL; id++ )
		{
			if ( !GPIO_GetLevel( pin_config[key_id].gpioPort, pin_config[key_id].gpioNumber ))
			{
				gpioBtnIDs |= (1<<id);
			}
		}
	}
	return gpioBtnIDs;
}

void ciaaTEC_EnableIRQ (perif_t key_id, irqChId_t irq_id)
{
	if( TECL_VALID(key_id) && IRQ_VALID_CH(irq_id) )
	{
		teclStatus[TECL_INDEX(key_id)].idIRQ= irq_id;
		teclStatus[TECL_INDEX(key_id)].enableIRQ= TRUE;

		GPIO_InputIRQEnable(  pin_config[key_id].gpioPort, pin_config[key_id].gpioNumber, irq_id, IRQ_MODE_PULSADOR );
	}
}

void ciaaTEC_DisableIRQ (perif_t key_id)
{
	if( TECL_VALID(key_id) )
	{
		teclStatus[TECL_INDEX(key_id)].enableIRQ= FALSE;

		GPIO_InputIRQDisable( teclStatus[TECL_INDEX(key_id)].idIRQ );
	}
}

void ciaaTEC_DebounInit (perif_t key_id)
{
	uint8_t id;

	if( TECL_VALID(key_id) )
	{
		id= TECL_INDEX(key_id);
		teclStatus[id].Test0= TECL_FREE;
		teclStatus[id].Test1= TECL_FREE;
		teclStatus[id].Test2= TECL_FREE;
		teclStatus[id].Test3= TECL_FREE;
		teclStatus[id].enableDebounce= TRUE;
	}
	else
	{
		for (id=0; id < TECL_TOTAL; ++id)
		{
			teclStatus[id].Test0= TECL_FREE;
			teclStatus[id].Test1= TECL_FREE;
			teclStatus[id].Test2= TECL_FREE;
			teclStatus[id].Test3= TECL_FREE;
			teclStatus[id].enableDebounce= TRUE;
		}
	}
}

tecReg_t ciaaTEC_DebounStatus (perif_t key_id)
{
	tecReg_t retval= TECL_RELEASE;
	uint8_t id;

	if( TECL_VALID(key_id) )
	{
		id= TECL_INDEX(key_id);
		// Actualizar muestras
		teclStatus[id].Test3 = teclStatus[id].Test2;
		teclStatus[id].Test2 = teclStatus[id].Test1;
		teclStatus[id].Test1 = teclStatus[id].Test0;

		teclStatus[id].Test0 = GPIO_GetLevel( pin_config[key_id].gpioPort, pin_config[key_id].gpioNumber );

		if ((TECL_FREE == teclStatus[id].Test3) && (TECL_FREE == teclStatus[id].Test2) &&
			(TECL_PUSH == teclStatus[id].Test1) && (TECL_PUSH == teclStatus[id].Test0))
		{
			// Flanco de bajada detectado.
			retval= TECL_PRESSED(id);
		}
		else
		{
			retval= TECL_RELEASE(id);
		}
	}
	else if( TECL_ALL == key_id  )
	{
		for (id = 0; id < TECL_TOTAL; ++id)
		{
			// Actualizar muestras
			teclStatus[id].Test3 = teclStatus[id].Test2;
			teclStatus[id].Test2 = teclStatus[id].Test1;
			teclStatus[id].Test1 = teclStatus[id].Test0;

			teclStatus[id].Test0 = GPIO_GetLevel( pin_config[id+TECL1].gpioPort, pin_config[id+TECL1].gpioNumber );

			if ((TECL_FREE == teclStatus[id].Test3) && (TECL_FREE == teclStatus[id].Test2) &&
				(TECL_PUSH == teclStatus[id].Test1) && (TECL_PUSH == teclStatus[id].Test0))
			{
				// Flanco de bajada detectado.
				retval |= TECL_PRESSED(id);
			}
			else
			{
				retval &= ~TECL_RELEASE(id);
			}
		}
	}
	return retval;
}

tecReg_t ciaaTEC_Level_ISR (perif_t key_id)
{
	tecReg_t retval=TECL_RELEASE;
	uint8_t i;
	uint8_t id= TECL_INDEX(key_id);


	if( TECL_VALID(key_id) )
	{
		if( GPIO_InputIRQHandler( teclStatus[id].idIRQ ) )
		{
			retval = (1<<id);
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

//*****************************************************************************+
#define GPIO_TYPE(X)	( GPIO_VALID(X) || LEDS_VALID(X) )
#define LEDS_RGB(X) 	( (LED_R<=X) && (LED_B>=X) )

int ciaaGPIO_EnablePin (perif_t gpio_id, int mode)
{
	int retval= 0;

	if( GPIO_TYPE(gpio_id)  && MODE_VALID(mode) )
	{
		GPIO_EnablePin( pin_config[gpio_id].pinPort, pin_config[gpio_id].pinNumber, pin_config[gpio_id].function,
						pin_config[gpio_id].gpioPort, pin_config[gpio_id].gpioNumber, mode );
		retval=1;
	}
	return retval;
}

int ciaaGPIO_EnableIRQ (perif_t gpio_id, irqChId_t irq_id)
{
	int retval= 0;

	if( GPIO_TYPE(gpio_id) && IRQ_VALID_CH(irq_id) )
	{
		GPIO_InputIRQEnable( pin_config[gpio_id].gpioPort, pin_config[gpio_id].gpioNumber, irq_id, IRQ_MODE_LEVEL);
		retval=1;
	}
	return retval;
}

int ciaaGPIO_GetLevelIRQ (perif_t inp_id, irqChId_t irq_id)
{
	int retval= GPIO_HIGH_LEVEL;

	if( GPIO_TYPE(inp_id)  && IRQ_VALID_CH(irq_id) )
	{
		if( GPIO_InputIRQHandler( irq_id ) )
		{
			//retval = GPIO_LOW_LEVEL;
		}
		//if( GPIO_GetLevel(  pin_config[inp_id].gpioPort, pin_config[inp_id].gpioNumber ))
		{
			retval= GPIO_GetLevel(  pin_config[inp_id].gpioPort, pin_config[inp_id].gpioNumber );
		}
	}
	return retval;
}

void ciaaGPIO_SetLevel (perif_t inp_id, uint8_t value)
{
	int i;
	if( LEDS_VALID(inp_id) )
	{
		if( LEDS_RGB(inp_id) )
		{
			for (i = LED_R; i < LED_B; ++i)
			{
				GPIO_SetLevel( pin_config[i].gpioPort, pin_config[i].gpioNumber, FALSE );
			}
		}
	}
	if ( GPIO_TYPE(inp_id)  )
	{
		GPIO_SetLevel( pin_config[inp_id].gpioPort, pin_config[inp_id].gpioNumber, value );
	}
}

int ciaaGPIO_GetLevel (perif_t inp_id)
{
	int retval= GPIO_HIGH_LEVEL;

	if ( GPIO_TYPE(inp_id) )
	{
		if( GPIO_GetLevel(  pin_config[inp_id].gpioPort, pin_config[inp_id].gpioNumber ))
		{
			retval= GPIO_LOW_LEVEL;
		}
	}
	return retval;
}

void ciaaGPIO_Toggle (perif_t inp_id)
{
	if ( GPIO_TYPE(inp_id) )
	{
		ciaaGPIO_SetLevel( inp_id, !ciaaGPIO_GetLevel( inp_id ) );
	}
}

debInput_t ciaaGPIO_DebounStatus (perif_t key_id)
{
	debInput_t retval= input_state_uninit;
	uint8_t id;

	if( ! GPIO_VALID(key_id) )
	{
		return retval;
	}

	id= GPIO_INDEX(key_id);
	if( TRUE == gpioStatus[id].enableDebounce )
	{
		// Actualizar muestras
		gpioStatus[id].Test3 = gpioStatus[id].Test2;
		gpioStatus[id].Test2 = gpioStatus[id].Test1;
		gpioStatus[id].Test1 = gpioStatus[id].Test0;

		gpioStatus[id].Test0 = GPIO_GetLevel( pin_config[key_id].gpioPort, pin_config[key_id].gpioNumber );

		if ((GPIO_HIGH_LEVEL == gpioStatus[id].Test3) && (GPIO_HIGH_LEVEL == gpioStatus[id].Test2) &&
			(GPIO_LOW_LEVEL == gpioStatus[id].Test1) && (GPIO_LOW_LEVEL == gpioStatus[id].Test0))
		{
			retval= input_state_low;
		}
		else if( (GPIO_LOW_LEVEL == gpioStatus[id].Test3) && (GPIO_LOW_LEVEL == gpioStatus[id].Test2) &&
			(GPIO_HIGH_LEVEL == gpioStatus[id].Test1) && (GPIO_HIGH_LEVEL == gpioStatus[id].Test0))
		{
			retval= input_state_high;
		}
		else
		{
			retval= input_state_equal;
		}
	}
	return retval;
}

void ciaaGPIO_DebounInit (perif_t key_id)
{
	uint8_t id;

	if( GPIO_VALID(key_id) )
	{
		id= GPIO_INDEX(key_id);
		gpioStatus[id].Test0= GPIO_LOW_LEVEL;
		gpioStatus[id].Test1= GPIO_LOW_LEVEL;
		gpioStatus[id].Test2= GPIO_LOW_LEVEL;
		gpioStatus[id].Test3= GPIO_LOW_LEVEL;
		gpioStatus[id].enableDebounce= TRUE;
	}
	else
	{
		for (id=0; id < GPIO_TOTAL; ++id)
		{
			gpioStatus[id].Test0= GPIO_LOW_LEVEL;
			gpioStatus[id].Test1= GPIO_LOW_LEVEL;
			gpioStatus[id].Test2= GPIO_LOW_LEVEL;
			gpioStatus[id].Test3= GPIO_LOW_LEVEL;
			gpioStatus[id].enableDebounce= TRUE;
		}
	}
}

