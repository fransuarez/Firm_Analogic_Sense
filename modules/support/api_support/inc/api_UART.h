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
 
#ifndef CIAAUART_H_
#define CIAAUART_H_

#include "chip.h"
#include "string.h"


void 	UART_Init 		( void );
int 	UART_Send		( int nUART, void * data, int datalen );
int 	UART_Recv 		( int nUART, void * data, int datalen );
int 	UART_SendChar 	( uint8_t c );
int 	UART_SendStr  	( const char *);
uint8_t UART_RecvChar 	( void );


#define print_RS485(x)		UART_Send ( 0, (uint8_t *)(x), strlen(x) )
#define print_RS232(x)  	UART_Send ( 2, (uint8_t *)(x), strlen(x) )
#define sendChr_DEBUG(x)  	UART_SendChar ( x )
#define sendStr_DEBUG(x)  	UART_Send ( 1, (uint8_t *)(x), strlen(x) )
#define sendBuf_DEBUG(x,y) 	UART_Send ( 1, (uint8_t *)(x), y )
#define recvBuf_DEBUG(x,y)	UART_Recv ( 1, (uint8_t *)(x), y )

#endif /* CIAAUART_H_ */
