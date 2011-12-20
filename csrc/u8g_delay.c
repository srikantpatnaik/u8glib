/*

  u8g_delay.c

  Universal 8bit Graphics Library
  
  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
  
*/


#include "u8g.h"

#if defined(__AVR__)
/*==== Generic AVR delay ====*/


#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

/*
  Delay by the provided number of milliseconds.
  Thus, a 16 bit value will allow a delay of 0..65 seconds
  Makes use of the _delay_loop_2
  
  _delay_loop_2 will do a delay of n * 4 prozessor cycles.
  with f = F_CPU cycles per second,
  n = f / (1000 * 4 )
  with f = 16000000 the result is 4000
  with f = 1000000 the result is 250
  
  the millisec loop, gcc requires the following overhead:
  - movev 1
  - subwi 2x2
  - bne i 2
  ==> 7 cycles
*/

void u8g_Delay(uint16_t val)
{
  /* old version did a call to the arduino lib: delay(val); */
  while( val != 0 )
  {
    _delay_loop_2( (F_CPU / 4000 ) -7);
    val--;
  }
}

#elif defined(__18CXX)

/*==== Delay for PIC18 (not tested) ====*/

#include <delays.h>

#define GetSystemClock()		(64000000ul)      // Hz
#define GetInstructionClock()	(GetSystemClock()/4)

void u8g_Delay(uint16_t val)
{
	unsigned int _iTemp = (val);
	while(_iTemp--)		
		Delay1KTCYx((GetInstructionClock()+999999)/1000000);
}

#elif defined(__PIC32MX)
/*==== Delay for PIC32 (assumes chipkit arduino environment ) ====*/
void u8g_Delay(uint16_t val)
{
	delay(val);
}

#else

/*==== Any other systems ====*/
void u8g_Delay(uint16_t val)
{
	/* do not know how to delay... */
}
#endif