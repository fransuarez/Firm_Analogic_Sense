/*
 * api_GPIO.c
 *
 *  Created on: Oct 11, 2018
 *      Author: fran
 */

#include "chip.h"

void GPIO_EnableIRQ_CH0 ( uint8_t port, uint8_t pin )
{
    Chip_PININT_Init( LPC_GPIO_PIN_INT );
    Chip_SCU_GPIOIntPinSel( IRQ_PIN_INT0, port, pin );

    //Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH0);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH0 );

    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH0 );
	/* Set lowest priority for RIT */
	NVIC_SetPriority( PIN_INT0_IRQn, ((1<<__NVIC_PRIO_BITS) - 1) );

	/* Enable IRQ for RIT */
	NVIC_EnableIRQ( PIN_INT0_IRQn );
}

uint8_t GPIO_IRQHandler_CH0 ( void )
{
	uint8_t retval;

	// Por ahora es muy rustico, el canal 0 esta reservado solo para la tecla 1:
	if( PININTCH0 == Chip_PININT_GetFallStates( LPC_GPIO_PIN_INT ) )
	{
		retval= 1;
		Chip_PININT_ClearFallStates( LPC_GPIO_PIN_INT, PININTCH0 );
	}

	else if ( PININTCH0 == Chip_PININT_GetRiseStates( LPC_GPIO_PIN_INT ))
	{
		retval= 0;
		Chip_PININT_ClearRiseStates( LPC_GPIO_PIN_INT, PININTCH0 );
	}

	NVIC_ClearPendingIRQ( PIN_INT0_IRQn );

	return retval;
}

