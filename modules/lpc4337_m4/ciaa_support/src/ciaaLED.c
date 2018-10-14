/*
 * ciaaLED.c
 *
 *  Created on: 12/9/2018
 *      Author: fran
 */

#include "ciaaGPIO_def.h"

#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_LED3           0x04
#define LEDS_LED4           0x08
#define LEDS_NO_LEDS        0x00

static const io_port_t gpioLEDBits[] =
{
	{PORT_LEDR, PIN_LEDR},
	{PORT_LEDG, PIN_LEDG},
	{PORT_LEDB, PIN_LEDB},
	{PORT_LED1, PIN_LED1},
	{PORT_LED2, PIN_LED2},
	{PORT_LED3, PIN_LED3}
};

void ciaaLED_Init (void)
{
   /* LEDs EDU-CIAA-NXP */
   Chip_SCU_PinMux(2, 0, MD_PUP|MD_EZI, FUNC4);  /* GPIO5[0], LED0R */
   Chip_SCU_PinMux(2, 1, MD_PUP|MD_EZI, FUNC4);  /* GPIO5[1], LED0G */
   Chip_SCU_PinMux(2, 2, MD_PUP|MD_EZI, FUNC4);  /* GPIO5[2], LED0B */
   Chip_SCU_PinMux(2, 10, MD_PUP|MD_EZI, FUNC0); /* GPIO0[14], LED1 */
   Chip_SCU_PinMux(2, 11, MD_PUP|MD_EZI, FUNC0); /* GPIO1[11], LED2 */
   Chip_SCU_PinMux(2, 12, MD_PUP|MD_EZI, FUNC0); /* GPIO1[12], LED3 */

   Chip_GPIO_SetDir(LPC_GPIO_PORT, PORT_LEDR, (1<<PIN_LEDR)|(1<<PIN_LEDG)|(1<<PIN_LEDB), 1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, PORT_LED1, (1<<PIN_LED1), 1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, PORT_LED2, (1<<PIN_LED2)|(1<<PIN_LED3), 1);

   Chip_GPIO_ClearValue(LPC_GPIO_PORT, PORT_LEDR, (1<<PIN_LEDR)|(1<<PIN_LEDG)|(1<<PIN_LEDB));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, PORT_LED1, (1<<PIN_LED1));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, PORT_LED2, (1<<PIN_LED2)|(1<<PIN_LED3));
}

void ciaaLED_Set(uint8_t LEDNumber, bool On)
{
	if (LEDNumber < (sizeof(gpioLEDBits) / sizeof(io_port_t)))
	{
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, gpioLEDBits[LEDNumber].port, gpioLEDBits[LEDNumber].pin, On);
	}
}

bool ciaaLED_Test(uint8_t LEDNumber)
{
	if (LEDNumber < (sizeof(gpioLEDBits) / sizeof(io_port_t)))
	{
		return (bool) Chip_GPIO_GetPinState(LPC_GPIO_PORT, gpioLEDBits[LEDNumber].port, gpioLEDBits[LEDNumber].pin);
	}
	return false;
}

void ciaaLED_Toggle(uint8_t LEDNumber)
{
	ciaaLED_Set(LEDNumber, !ciaaLED_Test(LEDNumber));
}
