/*
 * ciaaPORT.c
 *
 *  Created on: Dec 6, 2018
 *      Author: fran
 */

#include "ciaaPORT.h"

// LEDS ***************************************************
// Definiciones de LEDS para el SCU:
#define PIN_PORT_LEDR		2	// F4
#define PIN_PORT_LEDG		2	// F4
#define PIN_PORT_LEDB		2	// F4
#define PIN_PORT_LED1		2	// F0
#define PIN_PORT_LED2		2	// F0
#define PIN_PORT_LED3		2	// F0

#define PIN_NUMB_LEDR		0	//gpio5 0
#define PIN_NUMB_LEDG		1	//gpio5 4
#define PIN_NUMB_LEDB		2	//gpio5 2
#define PIN_NUMB_LED1		10	//gpio0 14
#define PIN_NUMB_LED2		11	//gpio1 11
#define PIN_NUMB_LED3		12	//gpio1 12

// Definiciones de GPIO para api GPIO:
#define GPIO_PORT_LEDR		5
#define GPIO_PORT_LEDG		5
#define GPIO_PORT_LEDB		5
#define GPIO_PORT_LED1		0
#define GPIO_PORT_LED2		1
#define GPIO_PORT_LED3		1

#define GPIO_NUMB_LEDR		0
#define GPIO_NUMB_LEDG		1
#define GPIO_NUMB_LEDB		2
#define GPIO_NUMB_LED1		14
#define GPIO_NUMB_LED2		11
#define GPIO_NUMB_LED3		12

// TECLAS ***************************************************
#define PIN_PORT_TEC1		1	// F0
#define PIN_PORT_TEC2		1	// F0
#define PIN_PORT_TEC3		1	// F0
#define PIN_PORT_TEC4		1	// F0

#define PIN_NUMB_TEC1		0	//gpio0 4
#define PIN_NUMB_TEC2		1	//gpio0 8
#define PIN_NUMB_TEC3		2	//gpio0 9
#define PIN_NUMB_TEC4		6	//gpio1	9

// Definiciones de GPIO para api GPIO:
#define GPIO_PORT_TEC1		0
#define GPIO_PORT_TEC2		0
#define GPIO_PORT_TEC3		0
#define GPIO_PORT_TEC4		1

#define GPIO_NUMB_TEC1		4
#define GPIO_NUMB_TEC2		8
#define GPIO_NUMB_TEC3		9
#define GPIO_NUMB_TEC4		9

// GPIOS  ***************************************************
// Definiciones de GPIOs para el SCU:
#define PIN_PORT_GPIO0		6	// F0
#define PIN_PORT_GPIO1		6	// F0
#define PIN_PORT_GPIO2		6	// F0
#define PIN_PORT_GPIO3		6	// F0
#define PIN_PORT_GPIO4		6	// F0
#define PIN_PORT_GPIO5		6	// F0
#define PIN_PORT_GPIO6		6	// F0
#define PIN_PORT_GPIO7		6	// F0
#define PIN_PORT_GPIO8		6	// F0

#define PIN_NUMB_GPIO0		1	//gpio3 0
#define PIN_NUMB_GPIO1		4	//gpio3 1	//4 //2
#define PIN_NUMB_GPIO2		5	//gpio3 2	//5 //3
#define PIN_NUMB_GPIO3		7	//gpio3 3 	//7 //4
#define PIN_NUMB_GPIO4		8	//gpio3 4	//8 //5
#define PIN_NUMB_GPIO5		9	//gpio3 5
#define PIN_NUMB_GPIO6		10	//gpio3 6
#define PIN_NUMB_GPIO7		11	//gpio3 7
#define PIN_NUMB_GPIO8		12	//gpio2 8

// Definiciones de GPIO para api GPIO:
#define GPIO_PORT_GPIO0		3
#define GPIO_PORT_GPIO1		3
#define GPIO_PORT_GPIO2		3
#define GPIO_PORT_GPIO3		5  //3 //5
#define GPIO_PORT_GPIO4		5  //3 //5
#define GPIO_PORT_GPIO5		3
#define GPIO_PORT_GPIO6		3
#define GPIO_PORT_GPIO7		3
#define GPIO_PORT_GPIO8		2

#define GPIO_NUMB_GPIO0		0
#define GPIO_NUMB_GPIO1		3  //1 //3
#define GPIO_NUMB_GPIO2		4  //2 //4
#define GPIO_NUMB_GPIO3		15 //3 //15
#define GPIO_NUMB_GPIO4		16 //4 //16
#define GPIO_NUMB_GPIO5		5
#define GPIO_NUMB_GPIO6		6
#define GPIO_NUMB_GPIO7		7
#define GPIO_NUMB_GPIO8		8

//****************************************************
// Definiciones de OTROS GPIOs para el SCU:
#define PIN_PORT_SPI_MISO	1	// F0
#define PIN_PORT_SPI_MOSI	1	// F0
#define PIN_PORT_SPI_SCK	0xF	// NO DISPONIBLE
#define PIN_PORT_LCD_EN		4	// F4
#define PIN_PORT_LCD_RS		4	// F4
#define PIN_PORT_LCD1		4	// F0
#define PIN_PORT_LCD2		4	// F0
#define PIN_PORT_LCD3		4	// F0
#define PIN_PORT_LCD4		4	// F4

#define PIN_NUMB_SPI_MISO	3	//gpio0 10
#define PIN_NUMB_SPI_MOSI	4	//gpio0 11
#define PIN_NUMB_SPI_SCK	4
#define PIN_NUMB_LCD_EN		9	//gpio5 13
#define PIN_NUMB_LCD_RS		8	//gpio5 12
#define PIN_NUMB_LCD1		4	//gpio2 4
#define PIN_NUMB_LCD2		5	//gpio2 5
#define PIN_NUMB_LCD3		6	//gpio2 6
#define PIN_NUMB_LCD4		10	//gpio5 14

// Definiciones de GPIO para api GPIO:
#define GPIO_PORT_SPI_MISO	0
#define GPIO_PORT_SPI_MOSI	0
#define GPIO_PORT_LCD_EN	5
#define GPIO_PORT_LCD_RS	5
#define GPIO_PORT_LCD1		2
#define GPIO_PORT_LCD2		2
#define GPIO_PORT_LCD3		2
#define GPIO_PORT_LCD4		5

#define GPIO_NUMB_SPI_MISO	10
#define GPIO_NUMB_SPI_MOSI	11
#define GPIO_NUMB_LCD_EN	13
#define GPIO_NUMB_LCD_RS	12
#define GPIO_NUMB_LCD1		4
#define GPIO_NUMB_LCD2		5
#define GPIO_NUMB_LCD3		6
#define GPIO_NUMB_LCD4		14

// PARAMETROS CFG *****************************************
#define PIN_FUNCION_0		0
#define PIN_FUNCION_4		4


const gpioCFG_t pin_config[] =
{
	{PIN_PORT_LEDR, PIN_NUMB_LEDR, PIN_FUNCION_4, GPIO_PORT_LEDR, GPIO_NUMB_LEDR },
	{PIN_PORT_LEDG, PIN_NUMB_LEDG, PIN_FUNCION_4, GPIO_PORT_LEDG, GPIO_NUMB_LEDG },
	{PIN_PORT_LEDB, PIN_NUMB_LEDB, PIN_FUNCION_4, GPIO_PORT_LEDB, GPIO_NUMB_LEDB },
	{PIN_PORT_LED1, PIN_NUMB_LED1, PIN_FUNCION_0, GPIO_PORT_LED1, GPIO_NUMB_LED1 },
	{PIN_PORT_LED2, PIN_NUMB_LED2, PIN_FUNCION_0, GPIO_PORT_LED2, GPIO_NUMB_LED2 },
	{PIN_PORT_LED3, PIN_NUMB_LED3, PIN_FUNCION_0, GPIO_PORT_LED3, GPIO_NUMB_LED3 },

	{PIN_PORT_TEC1, PIN_NUMB_TEC1, PIN_FUNCION_0, GPIO_PORT_TEC1, GPIO_NUMB_TEC1 },
	{PIN_PORT_TEC2, PIN_NUMB_TEC2, PIN_FUNCION_0, GPIO_PORT_TEC2, GPIO_NUMB_TEC2 },
	{PIN_PORT_TEC3, PIN_NUMB_TEC3, PIN_FUNCION_0, GPIO_PORT_TEC3, GPIO_NUMB_TEC3 },
	{PIN_PORT_TEC4, PIN_NUMB_TEC4, PIN_FUNCION_0, GPIO_PORT_TEC4, GPIO_NUMB_TEC4 },

	{PIN_PORT_GPIO0, PIN_NUMB_GPIO0, PIN_FUNCION_0, GPIO_PORT_GPIO0, GPIO_NUMB_GPIO0 },
	{PIN_PORT_GPIO1, PIN_NUMB_GPIO1, PIN_FUNCION_0, GPIO_PORT_GPIO1, GPIO_NUMB_GPIO1 },
	{PIN_PORT_GPIO2, PIN_NUMB_GPIO2, PIN_FUNCION_0, GPIO_PORT_GPIO2, GPIO_NUMB_GPIO2 },
	{PIN_PORT_GPIO3, PIN_NUMB_GPIO3, PIN_FUNCION_0, GPIO_PORT_GPIO3, GPIO_NUMB_GPIO3 },
	{PIN_PORT_GPIO4, PIN_NUMB_GPIO4, PIN_FUNCION_0, GPIO_PORT_GPIO4, GPIO_NUMB_GPIO4 },
	{PIN_PORT_GPIO5, PIN_NUMB_GPIO5, PIN_FUNCION_0, GPIO_PORT_GPIO5, GPIO_NUMB_GPIO5 },
	{PIN_PORT_GPIO6, PIN_NUMB_GPIO6, PIN_FUNCION_0, GPIO_PORT_GPIO6, GPIO_NUMB_GPIO6 },
	{PIN_PORT_GPIO7, PIN_NUMB_GPIO7, PIN_FUNCION_0, GPIO_PORT_GPIO7, GPIO_NUMB_GPIO7 },
	{PIN_PORT_GPIO8, PIN_NUMB_GPIO8, PIN_FUNCION_0, GPIO_PORT_GPIO8, GPIO_NUMB_GPIO8 },
	
	{PIN_PORT_LCD_EN, PIN_NUMB_LCD_EN, PIN_FUNCION_4, GPIO_PORT_LCD_EN, GPIO_NUMB_LCD_EN },
	{PIN_PORT_LCD_RS, PIN_NUMB_LCD_RS, PIN_FUNCION_4, GPIO_PORT_LCD_RS, GPIO_NUMB_LCD_RS },
	{PIN_PORT_LCD1, PIN_NUMB_LCD1, PIN_FUNCION_0, GPIO_PORT_LCD1, GPIO_NUMB_LCD1 },
	{PIN_PORT_LCD2, PIN_NUMB_LCD2, PIN_FUNCION_0, GPIO_PORT_LCD2, GPIO_NUMB_LCD2 },
	{PIN_PORT_LCD3, PIN_NUMB_LCD3, PIN_FUNCION_0, GPIO_PORT_LCD3, GPIO_NUMB_LCD3 },
	{PIN_PORT_LCD4, PIN_NUMB_LCD4, PIN_FUNCION_4, GPIO_PORT_LCD4, GPIO_NUMB_LCD4 },

	{PIN_PORT_SPI_MISO, PIN_NUMB_SPI_MISO, PIN_FUNCION_0, GPIO_PORT_SPI_MISO, GPIO_NUMB_SPI_MISO },
	{PIN_PORT_SPI_SCK, PIN_NUMB_SPI_SCK, PIN_FUNCION_0, GPIO_PORT_LCD3, GPIO_NUMB_LCD3 }, // NO ESTA DISPONIBLE COMO GPIO!!! NO USAR!!
	{PIN_PORT_SPI_MOSI, PIN_NUMB_SPI_MOSI, PIN_FUNCION_0, GPIO_PORT_SPI_MOSI, GPIO_NUMB_SPI_MOSI },

};

