#include "../../ej13_blinky_button/inc/Pulse_Count_Task.h"

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

void PULSE_COUNT_TASK_Init(void)
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

uint8_t PULSE_COUNT_TASK_UpdateFallings(void){
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

void PULSE_COUNT_TASK_UpdateFreq(void){

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
