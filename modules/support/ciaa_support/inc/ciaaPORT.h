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

// Vienen de la tabla de definicion de puertos.
#define POS_LED_FIRST		0
#define POS_LED_LAST		5
#define POS_TEC_FIRST		6
#define POS_TEC_LAST		9
#define POS_GPIO_FIRST		10
#define POS_GPIO_LAST		24

#define LEDS_TOTAL 			( 6 )
#define TECL_TOTAL 			( 4 )
#define ADC_TOTAL			( 3 )

#define TECL_VALID(X) 		( (TECL1<=X) && (TECL4>=X) )
#define LEDS_VALID(X) 		( (LED_R<=X) && (LED_3>=X) )
#define AINP_VALID(X) 		( (AIN_1<=X) && (AIN_3>=X) )
#define GPIO_VALID(X) 		( (GPIO_0<=X) && (LCD_4>=X) )

#define IRQ_VALID_CH(X)		( IRQ_GPIO_CH0<=X && IRQ_GPIO_CH7>=X )
#define MODE_VALID(X)		( GPIO_OUT_MODE==X || GPIO_INP_MODE==X )

//#define LEDS_INDEX(X)		( X-LED_R )
//#define TECL_INDEX(X)		( X-TECL1 )
//#define AINP_INDEX(X)		( X-AIN_1 )
//#define GPIO_INDEX(X)		( X-GPIO_0 ))

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
	GPIO_1,
	GPIO_2,
	GPIO_3,
	GPIO_4,
	GPIO_5,
	GPIO_6,
	GPIO_7,
	GPIO_8,

	LCD_EN,
	LCD_RS,
	LCD_1,
	LCD_2,
	LCD_3,
	LCD_4,

	SPI_MISO,
	SPI_SCK,
	SPI_MOSI,
	TECL1_4,
	AIN_1,
	AIN_2,
	AIN_3,

} perif_t; //size= 1 byte

typedef enum
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
