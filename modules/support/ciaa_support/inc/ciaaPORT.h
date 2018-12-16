/*
 * ciaaPORT.h
 *
 *  Created on: Dec 6, 2018
 *      Author: fran
 */

#ifndef MODULES_SUPPORT_CIAA_SUPPORT_INC_CIAAPORT_H_
#define MODULES_SUPPORT_CIAA_SUPPORT_INC_CIAAPORT_H_

#include "lpc_types.h"

#define GPIO_OUT_MODE		( 1 )
#define GPIO_INP_MODE		( 0 )

#define LEDS_TOTAL 			( LED_3 - LED_R +1 )
#define TECL_TOTAL 			( TECL4 - TECL1 +1 )
#define ADC_TOTAL			( AIN_3 - AIN_1 +1 )
#define GPIO_TOTAL			( SPI_MOSI - GPIO_0 +1)

#define TECL_VALID(X) 		( (TECL1<=X) && (TECL4>=X) )
#define LEDS_VALID(X) 		( (LED_R<=X) && (LED_3>=X) )
#define AINP_VALID(X) 		( (AIN_1<=X) && (AIN_3>=X) )
#define GPIO_VALID(X) 		( (GPIO_0<=X) && (SPI_MOSI>=X) )

#define IRQ_VALID_CH(X)		( IRQ_GPIO_CH0<=X && IRQ_GPIO_CH7>=X )
#define MODE_VALID(X)		( GPIO_OUT_MODE==X || GPIO_INP_MODE==X )

typedef struct _pin_gpio_config
{
	uint8_t 	pinPort;
	uint8_t 	pinNumber;
	uint8_t 	function;
	uint8_t 	gpioPort;
	uint8_t 	gpioNumber;

} gpioCFG_t;

// NO MODIFICAR EL ORDEN!!
typedef enum _perifericos_edu_ciaa
{
	LED_R= 0,
	LED_G,
	LED_B,
	LED_1,
	LED_2,
	LED_3,

	TECL1,
	TECL2,
	TECL3,
	TECL4,

	GPIO_0,
	GPIO_1, // SW 2
	GPIO_2,
	GPIO_3,
	GPIO_4,
	GPIO_5, // SW 3
	GPIO_6,
	GPIO_7,
	GPIO_8,

	LCD_EN, // LED AUX 1
	LCD_RS, // LED AUX 4
	LCD_1,  // SW 1
	LCD_2,
	LCD_3,  // LED AUX 3
	LCD_4,  // LED AUX 5

	SPI_MISO,
	SPI_SCK,
	SPI_MOSI, // LED AUX 2

	AIN_1, // CH 1
	AIN_2, // CH 2
	AIN_3, // CH 3

	GPIO_ALL,
	TECL_ALL

} perif_t; //size= 1 byte

typedef enum _irq_gpio_ch
{
	IRQ_GPIO_CH0=0,
	IRQ_GPIO_CH1,
	IRQ_GPIO_CH2,
	IRQ_GPIO_CH3,
	IRQ_GPIO_CH4,
	IRQ_GPIO_CH5,
	IRQ_GPIO_CH6,
	IRQ_GPIO_CH7

} irqChId_t;

extern const gpioCFG_t pin_config[];

#endif /* MODULES_SUPPORT_CIAA_SUPPORT_INC_CIAAPORT_H_ */
