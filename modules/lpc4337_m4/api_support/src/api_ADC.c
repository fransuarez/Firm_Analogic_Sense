/* Copyright 2016, Pablo Ridolfi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
#include "chip.h"
//#include "api_ADC.h"

#define ADC_INPUTS	3
#define ADC_USE_IRQ 0
#define ADC_COTA_SUP ( 1<<10 )


static uint16_t current_in[ADC_INPUTS];

void ADC_init (void)
{
	ADC_CLOCK_SETUP_T clk = {200000, 10, true};

	Chip_ADC_Init( LPC_ADC0, &clk );

	Chip_ADC_EnableChannel( LPC_ADC0, ADC_CH1, ENABLE );
	Chip_ADC_EnableChannel( LPC_ADC0, ADC_CH2, ENABLE );
	Chip_ADC_EnableChannel( LPC_ADC0, ADC_CH3, ENABLE );

	Chip_ADC_Int_SetChannelCmd( LPC_ADC0, ADC_CH1, ENABLE );
	Chip_ADC_Int_SetChannelCmd( LPC_ADC0, ADC_CH2, ENABLE );
	Chip_ADC_Int_SetChannelCmd( LPC_ADC0, ADC_CH3, ENABLE );

	Chip_ADC_SetBurstCmd( LPC_ADC0, ENABLE );

#if ( ADC_USE_IRQ == 1 )
	NVIC_EnableIRQ( ADC0_IRQn );
#endif
}

uint16_t ADC_read (uint8_t chId)
{
	uint16_t retval= ADC_COTA_SUP;

	if( ADC_CH0<chId && ADC_CH4>chId )
	{
		if( SET == Chip_ADC_ReadStatus(LPC_ADC0, chId, ADC_DR_DONE_STAT) )
		{
			Chip_ADC_ReadValue(LPC_ADC0, chId, current_in+chId-1);
			retval= current_in[chId-1];
		}
	}
	return retval;
}

void ADC_IRQ0Support (void)
{
#if ( ADC_USE_IRQ == 1 )

	if( SET == Chip_ADC_ReadStatus(LPC_ADC0, ADC_CH1, ADC_DR_DONE_STAT) )
	{
		Chip_ADC_ReadValue(LPC_ADC0, ADC_CH1, current_in);
	}

	if( SET == Chip_ADC_ReadStatus(LPC_ADC0, ADC_CH2, ADC_DR_DONE_STAT) )
	{
		Chip_ADC_ReadValue(LPC_ADC0, ADC_CH2, current_in+1);
	}

	if( SET == Chip_ADC_ReadStatus(LPC_ADC0, ADC_CH3, ADC_DR_DONE_STAT) )
	{
		Chip_ADC_ReadValue(LPC_ADC0, ADC_CH3, current_in+2);
	}
#endif
	NVIC_ClearPendingIRQ( ADC0_IRQn );
}

void DAC_init (void)
{
	Chip_DAC_Init(LPC_DAC);

	Chip_DAC_ConfigDAConverterControl(LPC_DAC, DAC_DMA_ENA);

	Chip_DAC_UpdateValue(LPC_DAC, 0);
}

/* [in] level: Output level (0 - 100%) */
void DAC_set (float level)
{
	if(level > 100) level = 100;

	Chip_DAC_UpdateValue(LPC_DAC, (uint32_t)((level * (float)0x3FF) / 100.0));
}
