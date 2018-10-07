#ifndef PULSE_COUNT_TASK_H_
#define PULSE_COUNT_TASK_H_

#include "auxiliar_gpios_def.h"

void PULSE_COUNT_TASK_Init(void);
uint8_t PULSE_COUNT_TASK_UpdateFallings(void);
void PULSE_COUNT_TASK_UpdateFreq(void);

void LEVEL_KEY_Detec_From_ISR ( queue_t* );
void VALUE_ANALOG_Read_Inputs ( signal_t * );

#endif /* PULSE_COUNT_TASK_H_ */
