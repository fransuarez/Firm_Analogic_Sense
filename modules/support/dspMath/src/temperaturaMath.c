/*
 *  Created on: Nov 26, 201
 *  	Author: fran
 *
 * @file tempCalculos.c
 * @brief NTC thermistor library (Implementation)
 * @version 1.0
 * @copyright GNU Lesser General Public License version 3
 *            <http://www.gnu.org/licenses/lgpl.html>
 * Copyright (c) 2007, 2013 - SoftQuadrat GmbH, Germany
 * Contact: thermistor (at) softquadrat.de
 * Web site: thermistor.sourceforge.net
 */

/** \addtogroup soporte_libs FreeRTOS blink example
 ** @{ */

/*==================[inclusions]=============================================*/
#include "temperaturaMath.h"
#include <math.h>
//#include <stdlib.h>


/* constants ================================================================ */
#define OFFSET_KELVIN 			273.15
#define SIZE_TABLA_CONV_POS 	13
#define SIZE_TABLA_CONV_NEG 	2


/*==================[internal data declaration]==============================*/
static coefConvTer voltConvTipoKNeg [SIZE_TABLA_CONV_NEG]= {
// 		(1)°C  	0	  -1	 -2		-3	   -4	  -5	 -6		-7	   -8	  -9
		{  0, { 0.000,-0.039,-0.079,-0.118,-0.157,-0.197,-0.236,-0.275,-0.314,-0.353 }},
		{-10, {-0.392,-0.431,-0.470,-0.508,-0.547,-0.586,-0.624,-0.663,-0.701,-0.739 }}
};
static coefConvTer voltConvTipoKPos [SIZE_TABLA_CONV_POS]= {
//	 			0	  +1	 +2		+3		+4	  +5	 +6		+7	   +8	  +9
		{  0, { 0.000, 0.039, 0.079, 0.119, 0.158, 0.198, 0.238, 0.277, 0.317, 0.357 }},
		{ 10, { 0.397, 0.437, 0.477, 0.517, 0.557, 0.597, 0.637, 0.677, 0.718, 0.758 }},
		{ 20, { 0.798, 0.838, 0.879, 0.919, 0.960, 1.000, 1.041, 1.081, 1.122, 1.163 }},
		{ 30, { 1.203, 1.244, 1.285, 1.326, 1.366, 1.407, 1.448, 1.489, 1.530, 1.571 }},
		{ 40, { 1.612, 1.653, 1.694, 1.735, 1.776, 1.817, 1.858, 1.899, 1.941, 1.982 }},
		{ 50, { 2.023, 2.064, 2.106, 2.147, 2.188, 2.230, 2.271, 2.312, 2.354, 2.395 }},
		{ 60, { 2.436, 2.478, 2.519, 2.561, 2.602, 2.644, 2.685, 2.727, 2.768, 2.810 }},
		{ 70, { 2.851, 2.893, 2.934, 2.976, 3.017, 3.059, 3.100, 3.142, 3.184, 3.225 }},
		{ 80, { 3.267, 3.308, 3.350, 3.391, 3.433, 3.474, 3.516, 3.557, 3.599, 3.640 }},
		{ 90, { 3.682, 3.723, 3.765, 3.806, 3.848, 3.889, 3.931, 3.972, 4.013, 4.055 }},
		{100, { 4.096, 4.138, 4.179, 4.220, 4.262, 4.303, 4.344, 4.385, 4.427, 4.468 }},
		{110, { 4.509, 4.550, 4.591, 4.633, 4.674, 4.715, 4.756, 4.797, 4.838, 4.879 }},
		{120, { 4.920, 4.961, 5.002, 5.043, 5.084, 5.124, 5.165, 5.206, 5.247, 5.288 }},
};
// 	difTemp= dTempRef- voltConvTipoK[0].decenaCelsius+10;
/*==================[macros and definitions]=================================*/
#define VOLTAJE_VALIDO_TK(X) (( voltConvTipoKNeg[SIZE_TABLA_CONV_NEG-1].coefPorUnidadCelsius[9] <= X )\
							 && ( voltConvTipoKPos[SIZE_TABLA_CONV_POS-1].coefPorUnidadCelsius[9] >= X ))

#define TEMP_VALIDA_TK(X) 	 (( (voltConvTipoKNeg[SIZE_TABLA_CONV_NEG-1].decenaCelsius)-9 <= X )\
		 	 	 	 	 	 && ((voltConvTipoKPos[SIZE_TABLA_CONV_POS-1].decenaCelsius)+9 >= X ))

/*==================[internal functions declaration]=========================*/
static temp_t busquedaBinariaK (volt_t volt_ref);
static temp_t tempFromVoltK (volt_t voltRef);
static volt_t voltFromTempK (temp_t dTempRef);

/* internal public functions ================================================ */
/*
 * @brief Calcula la resistividad del termistor NTC a partir de los coeficientes de la
 * 		ecuacion de Steinhart {A,B,C} y la temperatura medida en °C.
 * @param dT temperatura medida en °C.
 * @param dCoeff:
 * - coeficientes de la ecuacion de Steinhart si se usa MODELO_SHEINART.
 * - coeficientes del modelo Beta donde {R0, T0, Beta} si se usa el MODELO_BETA.
 * @return valor de la resistencia equivalente en ohms.
 */
double tstorTempToRes (double dT, cntc_t* dCoeff)
{
	double r= ERROR_CALC;
	double u, v, c, x, y;

	dT+= OFFSET_KELVIN;
	if( dCoeff && (0<dT) && ( dCoeff->A && dCoeff->B && dCoeff->C) )
	{
#if MODELO_TERMISTOR == MODELO_SHEINART

		x = ( dCoeff->A - 1.0/dT )/( dCoeff->C );
		c = ( dCoeff->B )/( 3*dCoeff->C );
		y = sqrt( (c*c*c) + (x*x/4.0) );

		u = pow( (y - x/2), 1/3 );
		v = pow( (y + x/2), 1/3 );

		r  = exp(u - v);

#else
		x = dCoeff[0];
		c = 1/dCoeff[1]-1/dT;
		y = -dCoeff[2]*c;

		r = x*exp(y);
#endif
	}

	return r;
}

// -----------------------------------------------------------------------------
/*
 * @brief Calcula la temperatura del termistor NTC a partir de los coficientes de la
 * 		ecuacion de Steinhart {A,B,C} y la resistencia en ohms.
 * @param dR resitencia medida en ohms.
 * @param dCoeff coeficientes de la ecuacion de Steinhart.
 * @return valor de temperatura en °C.
 */

double tstorResToTemp (double dR, cntc_t* dCoeff)
{
	double ti= ERROR_TEMP;
	double x, y;

	if( dCoeff && (1<dR) && (dCoeff->A && dCoeff->B && dCoeff->C) )
	{
#if MODELO_TERMISTOR == MODELO_SHEINART
		x= log(dR);
		y= x*x*x;

		ti = dCoeff->A + dCoeff->B*x + dCoeff->C*y;
		ti = 1.0/ti;

#else
		x = dCoeff->C;
		y = dCoeff->A*exp(-(x/dCoeff->B));

		ti = x/log(dR/y);
#endif

		ti -= OFFSET_KELVIN;
	}

	return ti;
}

int tstorCalcCoef ( double dR[], double dT[], cntc_t* dCoeff )
{
	double L1, L2, L3;
	double G1, G2;

	if( dCoeff )
	{
#if MODELO_TERMISTOR == MODELO_SHEINART
		// La resistencia debe ser distinta a 0 y no pueden ser valores similares ya que las muestras deben ser distintas:
		if( 0<dR[0] && 0<dR[1] && 0<dR[2] && (dR[0] != dR[1]) && (dR[0] != dR[2]) && (dR[1] != dR[2]) )
		{
			L1= log( dR[0] );
			L2= log( dR[1] );
			L3= log( dR[2] );

			dT[0] +=OFFSET_KELVIN;
			dT[1] +=OFFSET_KELVIN;
			dT[2] +=OFFSET_KELVIN;

			// La temperatura esta en grados Kelvin no puede ser 0:
			if( dT[0] && dT[1] && dT[2] )
			{
				G1= (1/dT[1]-1/dT[0])/(L2-L1);
				G2= (1/dT[2]-1/dT[0])/(L3-L1);

				dCoeff->C= (G2-G1)/(L3-L2)/(L1+L2+L3);
				dCoeff->B= G1 - dCoeff->C*( L1*L1 + L1*L2 + L2*L2);
				dCoeff->A= 1/dT[0] - dCoeff->B*L1 - dCoeff->C*L1*L1*L1;

				return 1;
			}
		}
#else
		if( 0<dR[0] && 0<dR[1] && (dR[0] != dR[1]) )
		{
			L1= log( dR[0] );
			L2= log( dR[1] );

			dT[0] +=OFFSET_KELVIN;
			dT[1] +=OFFSET_KELVIN;

			if( dT[0] && dT[1] )
			{
				G1= 1/dT[0];
				G2= 1/dT[1];

				dCoeff->C= (L2-L1)/(G2-G1);
				dCoeff->B= dT[0];
				dCoeff->A= dR[0];

				return 1;
			}
		}

#endif
	}
	return 0;
}

// -----------------------------------------------------------------------------
/*
 * @brief Calcula la temperatura de la termocupla tipo K a partir de la tension medida
 * 		en sus terminales en mV y la temperatura ambiente o del recinto de medicion en °C.
 * @param dVolt tension medida en la termocupla en mV.
 * 				Debe estar en el rango de la tabla.
 * @param dTempRef temperatura ambiente de referencia medida en °C.
 * 		  		Debe estar dentro del rango de la tabla de conversion.
 * @return valor de temperatura en °C.
 */
temp_t tcuplaKVoltToTemp (volt_t dVolt, temp_t dTempRef)
{
	volt_t volt_ref;
	temp_t ret_val= ERROR_TEMP;

	if( TEMP_VALIDA_TK (dTempRef) )
	{
		// 1- Busca el valor de tension para la temperatura referencia de la termocupla.
		volt_ref = voltFromTempK( dTempRef );
		// 2- Suma este valor a la tension medida.
		volt_ref += dVolt;

		if( VOLTAJE_VALIDO_TK (volt_ref) )
		{
			// 3- Busca en la tabla la temperatura para tension total calculada.
			ret_val = tempFromVoltK( volt_ref );
		}
	}
	return ret_val;
}

// -----------------------------------------------------------------------------
/*
 * @brief Calcula la tension que equivalente en los terminales en mV de la termocupla a partir 
 *		de la temperatura medida en la termocupla y la ambiente en °C.
 * @param dTemp temperatura en el punto de medicion en °C.
 * @param dTempRef temperatura de referencia en el ambiente en °C.
 * @return valor de tension en mV.
 */
volt_t tcuplaKTempToVolt (temp_t dTemp, temp_t dTempRef)
{
	volt_t volt_ref= ERROR_VOLT;

	if( TEMP_VALIDA_TK (dTemp) && TEMP_VALIDA_TK (dTempRef) )
	{
		volt_ref  = voltFromTempK( dTemp ) - voltFromTempK( dTempRef );
	}
	return volt_ref;
}


/* private functions ======================================================== */
/*
 * @brief Calcula la tension de referencia segun tabla conversion.
 * @param dTempRef temperatura en grados celsius.
 * @return el valor de tension en mV segun tabla K.
 */
static volt_t voltFromTempK (temp_t dTempRef)
{
	temp_t difTemp;
	volt_t retval;
	uint32_t decenaT, unidadT;

	if( 0 > dTempRef )
	{
		difTemp= -dTempRef;
	 	decenaT= difTemp/10;
	 	unidadT= difTemp%10;
	 	retval= voltConvTipoKNeg[decenaT].coefPorUnidadCelsius[unidadT];

	}
	else if( 0 < dTempRef )
	{
		difTemp= dTempRef;
	 	decenaT= difTemp/10;
	 	unidadT= difTemp%10;
	 	retval= voltConvTipoKPos[decenaT].coefPorUnidadCelsius[unidadT];
	}
	else
	{
		retval= 0;
	}

 	return retval;
}

static temp_t tempFromVoltK (volt_t voltRef)
{
	temp_t ret_val;

	if( 0 != voltRef )
	{
		ret_val= busquedaBinariaK( voltRef );
	}
	else
	{
		ret_val=0;
	}
	return ret_val;
}

static temp_t busquedaBinariaK (volt_t volt_ref)
{
	int32_t pos_i, pos_j ,inf_i, sup_i, inf_j, sup_j, pos_ret_i, pos_ret_j;
	coefConvTer *pTablaConversion;
	volt_t abs_volt_ref;
	temp_t ret_val= ERROR_TEMP;

	if( 0 > volt_ref )
	{
		pTablaConversion= voltConvTipoKNeg;
		inf_i= 0;
		sup_i= SIZE_TABLA_CONV_NEG-1;

	}
	else
	{
		pTablaConversion= voltConvTipoKPos;
		inf_i= 0;
		sup_i= SIZE_TABLA_CONV_POS-1;
	}

	pos_ret_i= -1;
	abs_volt_ref= fabs( volt_ref );

	/* Realiza la busqueda primero buscando entre filas y despues entre columnas.
	 * El valor volt_ref debe ser distinto de 0. De serlo no hace falta calcular
	 * la temperatura referencia para la el caso de la termocupla tipo K.
	 */
	while( inf_i <= sup_i)
	{
		// 1- Comienza la busqueda por la fila central de la tabla de valores.
		pos_i =( (sup_i-inf_i)/2 )+inf_i;
		inf_j= 0;
		sup_j= 9;

		pos_ret_j= -1;
		while( inf_j <= sup_j )
		{
			// Comienza desde el centro de las columnas.
			pos_j =( (sup_j-inf_j)/2 )+inf_j;

			if( fabs( pTablaConversion[pos_i].coefPorUnidadCelsius[pos_j] ) >= abs_volt_ref )
			{
				if( 0 < pos_j )
				{
					if( fabs( pTablaConversion[pos_i].coefPorUnidadCelsius[pos_j-1] ) <= abs_volt_ref)
					{
						pos_ret_j= pos_j;
						break;
					}
				}
				else
				{	// Si ya no quedan mas posiciones por recorrer, entonces llegue al valor mas cercano.
					if( inf_i == sup_i )
					{
						pos_ret_j= pos_j;
						break;
					}
				}
			}
			if( fabs( pTablaConversion[pos_i].coefPorUnidadCelsius[pos_j]) < abs_volt_ref  )
			{
				// Esto se debe a como estan ordenados los datos en la tabla.
				//( abs_volt_ref<= 0 )? inf_j= pos_j+1: sup_j= (pos_j-1);
				inf_j= pos_j+1;
			}
			else
			{
				// Esto se debe a como estan ordenados los datos en la tabla.
				//( abs_volt_ref<= 0 )? sup_j= pos_j-1: inf_j= (pos_j+1);
				sup_j= pos_j-1;
			}
		}

		if( -1 != pos_ret_j )
		{
			pos_ret_i= pos_i;
			break;
		}
		else
		{
			if( fabs( pTablaConversion[pos_i].coefPorUnidadCelsius[0]) > abs_volt_ref )
			{
				sup_i =pos_i-1;
			}
			else
			{
				inf_i =pos_i+1;
			}
		}
	}

	if(-1 != pos_ret_i)
	{
	// Retorna la temperatura como la suma del campo decena mas la unidad (como el numero de columna hallado).
		ret_val= (temp_t)( fabs( pTablaConversion[pos_ret_i].decenaCelsius) + (pos_ret_j) );
		if( 0 > volt_ref )
		{
			ret_val=-(ret_val);
		}
	}

	return ret_val;
}
/*==================[irq handlers functions ]=========================*/








/** @} doxygen end group definition */

/*==================[end of file]============================================*/

