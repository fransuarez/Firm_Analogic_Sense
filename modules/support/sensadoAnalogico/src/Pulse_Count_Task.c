#include "board.h"
#include "ciaaAIN.h"

#include "auxiliar_gpios_def.h"

// ------ Private constants -----------------------------------
#define TotalCalls 10000 //
#define HI_LEVEL 1
#define LO_LEVEL 0
#define KEY_INP_PASS TEC3

// ------ Private variable ------------------------
static uint8_t Test3, Test2, Test1, Test0;
static uint32_t Falling_edge;

// ------ Public variable ------------------------
uint32_t FrecuenciaFlancos;
uint32_t TotalFlancos_Tec0; // Si hay un flanco de bajada es 1
LedOut led;

void DIGITAL_KEY_Init ( void )
{
    Chip_PININT_Init( LPC_GPIO_PIN_INT );
    Chip_SCU_GPIOIntPinSel( ID_IRQ_PIN_INT0, ID_PORT_TEC1, ID_PIN_TEC1 );
    //Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH0);
    Chip_PININT_ClearIntStatus( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_SetPinModeEdge( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_EnableIntLow  ( LPC_GPIO_PIN_INT, PININTCH0 );
    Chip_PININT_EnableIntHigh ( LPC_GPIO_PIN_INT, PININTCH0 );
	/* Set lowest priority for RIT */
	NVIC_SetPriority(PIN_INT0_IRQn, (1<<__NVIC_PRIO_BITS) - 1);

	/* Enable IRQ for RIT */
	NVIC_EnableIRQ(PIN_INT0_IRQn);
}

void PULSE_COUNT_TASK_Init (void)
{
	//digitalConfig(TEC3, INPUT );

	led = MENOR11;
	TotalFlancos_Tec0 = 0;
	Falling_edge = 0;

	Test3 = LO_LEVEL;
	Test2 = LO_LEVEL;
	Test1 = LO_LEVEL;
	Test0 = LO_LEVEL;

}

uint8_t PULSE_COUNT_TASK_UpdateFallings (void)
{
	// Actualizar muestras
	Test3 = Test2;
	Test2 = Test1;
	Test1 = Test0;
	Test0 = Buttons_GetStatus();
	//Test0 = digitalRead(TEC3);
	uint8_t retval;

	if ((Test3 == LO_LEVEL) &&(Test2 == LO_LEVEL) && (Test1 == HI_LEVEL) &&(Test0 == HI_LEVEL)){
	Falling_edge = 1;
	retval=1;
	// Flanco de bajada detectado
	}
	else {
	Falling_edge = 0;
	retval=0;
	}
	//Se suma uno a los flancos
	TotalFlancos_Tec0+=Falling_edge;
	if ((Test3 == HI_LEVEL) &&(Test2 == HI_LEVEL) && (Test1 == LO_LEVEL) &&(Test0 == LO_LEVEL)){
		retval=2;
	}
	return retval;
}

void PULSE_COUNT_TASK_UpdateFreq (void)
{

		// se muestran los resultados
		if(TotalFlancos_Tec0<11)
		{
			led = MENOR11;
		}
		if((TotalFlancos_Tec0>10)&&(TotalFlancos_Tec0<21))
		{
			led = MENOR21;
		}
		if(TotalFlancos_Tec0>20)
		{
			led = MAYOR20;
		}
		if(TotalFlancos_Tec0/(TotalCalls/1000) == 0)
		{
			FrecuenciaFlancos = 0;
		}
		else
		{
			FrecuenciaFlancos = 1/(TotalFlancos_Tec0/(TotalCalls/1000));
		}
		TotalFlancos_Tec0 = 0;

}

void LEVEL_KEY_Detec_From_ISR ( queue_t* dataToSend )
{
	uint32_t irqCH_Fall_Stat, irqCH_Rise_Stat;

	if( dataToSend )
	{
		irqCH_Fall_Stat= Chip_PININT_GetFallStates( LPC_GPIO_PIN_INT );
		irqCH_Rise_Stat= Chip_PININT_GetRiseStates( LPC_GPIO_PIN_INT );

		// Por ahora es muy rustico, el canal 0 esta reservado solo para la tecla 1:
		if( PININTCH0 == irqCH_Fall_Stat )
		{
			dataToSend->key= TECL1;
			dataToSend->state= HIGH;
			//dataToSend->ticktimes= 0;

			Chip_PININT_ClearFallStates( LPC_GPIO_PIN_INT, PININTCH0 );
		}

		if( PININTCH0 == irqCH_Rise_Stat)
		{
			dataToSend->key= TECL1;
			dataToSend->state= LOW;
			//dataToSend->ticktimes= 0;

			Chip_PININT_ClearRiseStates( LPC_GPIO_PIN_INT, PININTCH0 );
		}

		NVIC_ClearPendingIRQ( PIN_INT0_IRQn );
	}
}

void VALUE_ANALOG_Read_Inputs ( signal_t * dataToSend )
{
	static uint16_t valAnalogicInp [4];

	dataToSend->led= (LED_1 | LED_2 | LED_3);
	dataToSend->readVal= valAnalogicInp;

	// leo el termistor:
	valAnalogicInp[0]= ciaaAINRead(0);

	// leo la termocupla:
	valAnalogicInp[1]= ciaaAINRead(1);

	// leo el auxiliar 1:
	valAnalogicInp[2]= ciaaAINRead(2);
}
