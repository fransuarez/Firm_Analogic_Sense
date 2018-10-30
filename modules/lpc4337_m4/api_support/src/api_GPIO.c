/*
 * api_GPIO.c
 *
 *  Created on: Oct 11, 2018
 *      Author: fran
 */

#include "chip.h"

#define INT_IRQ_ID(X)		( PIN_INT0_IRQn+(X) )
#define PINT_IRQ_ID(X)		( (X)-PIN_INT0_IRQn )

#define PULLUP_BUFFER		( MD_PUP|MD_EZI )
#define CH_ID_TO_IRQ_ID(X)	( INT_IRQ_ID(sqrt(X)) )
#define IRQ_ID_TO_CH_ID(X)	( 1<<PINT_IRQ_ID(X) )
#define CH_ID_TO_PIN_SEL(X)	( sqrt(X) )


void GPIO_EnablePin (uint8_t pinPort, uint8_t pinNumber, uint8_t func, uint8_t gpioPort, uint8_t gpioNumber, uint8_t mode)
{
	Chip_SCU_PinMux  ( pinPort, pinNumber, PULLUP_BUFFER, func );
	Chip_GPIO_SetDir ( LPC_GPIO_PORT, gpioPort,(1<<gpioNumber), mode);

	if( mode )
	{
		Chip_GPIO_ClearValue(LPC_GPIO_PORT, gpioPort, (1<<gpioNumber));
	}
}

void GPIO_SetLevel (uint8_t gpioPort, uint8_t gpioPin, bool level)
{
	Chip_GPIO_SetPinState( LPC_GPIO_PORT, gpioPort, gpioPin, level );
}

bool GPIO_GetLevel (uint8_t gpioPort, uint8_t gpioPin)
{
	return Chip_GPIO_GetPinState( LPC_GPIO_PORT, gpioPort, gpioPin );
}

void GPIO_InputIRQEnable (uint8_t gpioPort, uint8_t gpioPin, uint8_t chId)
{
    Chip_PININT_Init( LPC_GPIO_PIN_INT );
    Chip_SCU_GPIOIntPinSel( chId , gpioPort, gpioPin );
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH(chId) );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH(chId) );

    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH(chId) );
    //Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH(chId) );
	/* Set lowest priority for RIT */
	NVIC_SetPriority( INT_IRQ_ID(chId), ((1<<__NVIC_PRIO_BITS) - 1) );

	/* Enable IRQ for RIT */
	NVIC_EnableIRQ( INT_IRQ_ID(chId) );
}

void GPIO_InputIRQDisable (uint8_t chId)
{
    Chip_PININT_DisableIntLow  ( LPC_GPIO_PIN_INT, PININTCH(chId) );
    Chip_PININT_DisableIntHigh ( LPC_GPIO_PIN_INT, PININTCH(chId) );
	NVIC_DisableIRQ( INT_IRQ_ID(chId) );
}

uint8_t GPIO_InputIRQHandler ( uint8_t chId )
{
	uint8_t retval=0;

	if( PININTCH(chId) == Chip_PININT_GetFallStates( LPC_GPIO_PIN_INT ) )
	{
		Chip_PININT_ClearFallStates( LPC_GPIO_PIN_INT, PININTCH(chId) );
		retval= 1;
	}

	else if ( PININTCH(chId) == Chip_PININT_GetRiseStates( LPC_GPIO_PIN_INT ))
	{
		Chip_PININT_ClearRiseStates( LPC_GPIO_PIN_INT, PININTCH(chId) );
		//retval= 0;
	}

	NVIC_ClearPendingIRQ( INT_IRQ_ID(chId) );

	return retval;
}

