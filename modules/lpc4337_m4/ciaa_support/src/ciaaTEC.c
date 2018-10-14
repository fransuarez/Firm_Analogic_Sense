/*
 * ciaaKEY.c
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#include "ciaaGPIO_def.h"
#include "api_GPIO.h"

// ------ Private constants -----------------------------------

#define IRQ_PIN_INT0	0
#define IRQ_PIN_INT1	1
#define IRQ_PIN_INT2	2
#define IRQ_PIN_INT3	3

#define TotalCalls 10000
#define KEY_INP_PASS TEC3
#define TECL_TOTAL	 	4


static const io_port_t 	gpioBtnBits[TECL_TOTAL] =
{
	{PORT_TEC1, PIN_TEC1},
	{PORT_TEC2, PIN_TEC2},
	{PORT_TEC3, PIN_TEC3},
	{PORT_TEC4, PIN_TEC4}
};

typedef enum _tec_stat
{
	TEC_LIBERADO = 0x00,
	TEC1_PRESSED = 0x01,
	TEC2_PRESSED = 0x02,
	TEC3_PRESSED = 0x04,
	TEC4_PRESSED = 0x08,

} tecStat_t;

static const tecStat_t 	gpioBtnIDs[TECL_TOTAL]  =
{
	TEC1_PRESSED, TEC2_PRESSED, TEC3_PRESSED, TEC4_PRESSED
};

// ------ Public functions  ----------------------------------------

void ciaaTEC_Init (void)
{
	uint8_t i=0;
	/* EDU-CIAA-NXP buttons */
	PINMUX_GRP_T pin_config[TECL_TOTAL] = {
			{1, 0, MD_PUP|MD_EZI|FUNC0},	/* TEC1 -> P1_0 */
			{1, 1, MD_PUP|MD_EZI|FUNC0},	/* TEC2 -> P1_1 */
			{1, 2, MD_PUP|MD_EZI|FUNC0},	/* TEC3 -> P1_2 */
			{1, 6, MD_PUP|MD_EZI|FUNC0} 	/* TEC4 -> P1_6 */
	};

	Chip_SCU_SetPinMuxing(pin_config, (sizeof(pin_config) / sizeof(PINMUX_GRP_T)));

	for ( i = 0; i < (sizeof(gpioBtnBits) / sizeof(io_port_t)); i++)
	{
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpioBtnBits[i].port, gpioBtnBits[i].pin);
	}
	/*
	// Otra manera de hacer lo mismo mas resumida?:
	Chip_GPIO_SetDir(LPC_GPIO_PORT, PORT_TEC1,(1<<PIN_TEC1)|(1<<PIN_TEC2)|(1<<PIN_TEC3),0);
	Chip_GPIO_SetDir(LPC_GPIO_PORT, PORT_TEC4,(1<<PIN_TEC4),0);
	*/
	GPIO_EnableIRQ_CH0( PORT_TEC1, PIN_TEC1 );
}


tecStat_t ciaaTEC_Status (void)
{
	tecStat_t ret = TEC_LIBERADO;
	uint8_t i;

	for ( i = 0; i < (sizeof(gpioBtnBits) / sizeof(io_port_t)); i++ )
	{
		if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpioBtnBits[i].port, gpioBtnBits[i].pin) == 0)
		{
			ret |= gpioBtnIDs[i];
		}
	}

	return ret;
}
/*
uint32_t ciaaReadInput(uint32_t inputNumber)
{
	return Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, inputs[inputNumber].port, inputs[inputNumber].bit);
}
*/
// ------ Private variable ------------------------
static uint8_t Test3, Test2, Test1, Test0;
static uint32_t Falling_edge;

// ------ Public variable ------------------------
uint32_t FrecuenciaFlancos;
uint32_t TotalFlancos_Tec0; // Si hay un flanco de bajada es 1


void PULSE_COUNT_TASK_Init (void)
{
	//digitalConfig(TEC3, INPUT );
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
	Test0 = ciaaTEC_Status();
	//Test0 = digitalRead(TEC3);
	uint8_t retval;

	if ((Test3 == LO_LEVEL) &&(Test2 == LO_LEVEL) && (Test1 == HI_LEVEL) &&(Test0 == HI_LEVEL))
	{
		Falling_edge = 1;
		retval=1;
		// Flanco de bajada detectado
	}
	else
	{
		Falling_edge = 0;
		retval=0;
	}
	//Se suma uno a los flancos
	TotalFlancos_Tec0+= Falling_edge;
	if ((Test3 == HI_LEVEL) &&(Test2 == HI_LEVEL) && (Test1 == LO_LEVEL) && (Test0 == LO_LEVEL))
	{
		retval=2;
	}
	return retval;
}

uint8_t LEVEL_KEY_Detec_From_ISR ( uint8_t key_id )
{
	uint8_t retval=ERROR_ID_TECLA;
	// Por ahora es muy rustico, el canal 0 esta reservado solo para la tecla 1:
	if( TECL1 == key_id )
	{
		if( GPIO_IRQHandler_CH0() )
		{
			retval= HI_LEVEL;
		}
		else
		{
			retval= LO_LEVEL;

		}
	}
	return retval;
}
/*

typedef enum
{
	MENOR11,
	MENOR21,
	MAYOR20
} LedOut;


void PULSE_COUNT_TASK_UpdateFreq (void)
{
	static LedOut led= MENOR11;

	// se muestran los resultados
	if( TotalFlancos_Tec0<11 )
	{
		led = MENOR11;
	}
	if( (TotalFlancos_Tec0>10)&&(TotalFlancos_Tec0<21) )
	{
		led = MENOR21;
	}
	if( TotalFlancos_Tec0>20 )
	{
		led = MAYOR20;
	}
	if( TotalFlancos_Tec0/(TotalCalls/1000) == 0)
	{
		FrecuenciaFlancos = 0;
	}
	else
	{
		FrecuenciaFlancos = 1/(TotalFlancos_Tec0/(TotalCalls/1000));
	}
	TotalFlancos_Tec0 = 0;
}
*/


