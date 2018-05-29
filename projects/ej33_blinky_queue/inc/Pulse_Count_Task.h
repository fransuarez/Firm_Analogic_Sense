#ifndef PULSE_COUNT_TASK_H_
#define PULSE_COUNT_TASK_H_

//#include "sAPI.h"
#include "board.h"
void PULSE_COUNT_TASK_Init(void);
uint8_t PULSE_COUNT_TASK_UpdateFallings(void);
void PULSE_COUNT_TASK_UpdateFreq(void);
typedef enum {
	MENOR11,
	MENOR21,
	MAYOR20
} LedOut;

LedOut led;

#endif /* PULSE_COUNT_TASK_H_ */
