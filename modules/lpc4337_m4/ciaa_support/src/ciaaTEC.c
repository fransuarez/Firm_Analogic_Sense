/*
 * ciaaKEY.c
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#include "ciaaGPIO_def.h"

/* EDU-CIAA-NXP button defines */
typedef enum _tec_stat
{
	TEC_LIBERADO = 0x00,
	TEC1_PRESSED = 0x01,
	TEC2_PRESSED = 0x02,
	TEC3_PRESSED = 0x04,
	TEC4_PRESSED = 0x08,
} tecStat_t;

static const io_port_t 	gpioBtnBits[] =
{
	{PORT_TEC1, PIN_TEC1},
	{PORT_TEC2, PIN_TEC2},
	{PORT_TEC3, PIN_TEC3},
	{PORT_TEC4, PIN_TEC4}
};

static const tecStat_t 	gpioBtnIDs[]  =
{
	TEC1_PRESSED, TEC2_PRESSED, TEC3_PRESSED, TEC4_PRESSED
};

void ciaaKEYS_Init(void)
{
	uint8_t i=0;
	/* EDU-CIAA-NXP buttons */
	PINMUX_GRP_T pin_config[] = {
			{1, 0, MD_PUP|MD_EZI|FUNC0},	/* TEC1 -> P1_0 */
			{1, 1, MD_PUP|MD_EZI|FUNC0},	/* TEC2 -> P1_1 */
			{1, 2, MD_PUP|MD_EZI|FUNC0},	/* TEC3 -> P1_2 */
			{1, 6, MD_PUP|MD_EZI|FUNC0} 	/* TEC4 -> P1_6 */
	};

	Chip_SCU_SetPinMuxing(pin_config, (sizeof(pin_config) / sizeof(PINMUX_GRP_T)));

	for ( i = 0; i < (sizeof(gpioBtnBits) / sizeof(io_port_t)); i++) {
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, gpioBtnBits[i].port, gpioBtnBits[i].pin);
	}
	/*
	Chip_GPIO_SetDir(LPC_GPIO_PORT, 0,(1<<4)|(1<<8)|(1<<9),0);
	Chip_GPIO_SetDir(LPC_GPIO_PORT, 1,(1<<9),0);
	*/
}

tecStat_t ciaaKEYS_Status(void)
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

void ciaaKEY_EnableIRQ ( void )
{
    Chip_PININT_Init( LPC_GPIO_PIN_INT );
    Chip_SCU_GPIOIntPinSel( IRQ_PIN_INT0, PORT_TEC1, PIN_TEC1 );
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
