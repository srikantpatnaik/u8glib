/*
  
  u8g_arduino_sw_spi.c

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

#if defined(ARDUINO)

#if ARDUINO < 100 
#include <WProgram.h>    
#include "wiring_private.h"
#include "pins_arduino.h"

#else 
#include <Arduino.h> 
#include "wiring_private.h"
#endif

uint8_t u8g_bitData, u8g_bitNotData;
uint8_t u8g_bitClock, u8g_bitNotClock;
volatile uint8_t *u8g_outData;
volatile uint8_t *u8g_outClock;

static void u8g_com_arduino_init_shift_out(uint8_t dataPin, uint8_t clockPin)
{
  u8g_outData = portOutputRegister(digitalPinToPort(dataPin));
  u8g_outClock = portOutputRegister(digitalPinToPort(clockPin));
  u8g_bitData = digitalPinToBitMask(dataPin);
  u8g_bitClock = digitalPinToBitMask(clockPin);

  u8g_bitNotClock = u8g_bitClock;
  u8g_bitNotClock ^= 0x0ff;

  u8g_bitNotData = u8g_bitData;
  u8g_bitNotData ^= 0x0ff;
}

static void u8g_com_arduino_do_shift_out_msb_first(uint8_t val)
{
  uint8_t cnt = 8;
  uint8_t bitData = u8g_bitData;
  uint8_t bitNotData = u8g_bitNotData;
  uint8_t bitClock = u8g_bitClock;
  uint8_t bitNotClock = u8g_bitNotClock;
  volatile uint8_t *outData = u8g_outData;
  volatile uint8_t *outClock = u8g_outClock;
  do
  {
    if ( val & 128 )
      *outData |= bitData;
    else
      *outData &= bitNotData;
   
    *outClock |= bitClock;
    *outClock &= bitNotClock;
    val <<= 1;
    cnt--;
  } while( cnt != 0 );
}


uint8_t u8g_com_arduino_sw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  switch(msg)
  {
    case U8G_COM_MSG_INIT:
      u8g_com_arduino_assign_pin_output_high(u8g);
      u8g_com_arduino_digital_write(u8g, U8G_PI_SCK, LOW);
      u8g_com_arduino_digital_write(u8g, U8G_PI_MOSI, LOW);
      u8g_com_arduino_init_shift_out(u8g->pin_list[U8G_PI_MOSI], u8g->pin_list[U8G_PI_SCK]);
      break;
    
    case U8G_COM_MSG_STOP:
      break;

    case U8G_COM_MSG_RESET:
      if ( u8g->pin_list[U8G_PI_RESET] != U8G_PIN_NONE )
        u8g_com_arduino_digital_write(u8g, U8G_PI_RESET, arg_val);
      break;
      
    case U8G_COM_MSG_CHIP_SELECT:
      if ( arg_val == 0 )
      {
        /* disable */
        u8g_com_arduino_digital_write(u8g, U8G_PI_CS, HIGH);
      }
      else
      {
        /* enable */
        u8g_com_arduino_digital_write(u8g, U8G_PI_SCK, LOW);
        u8g_com_arduino_digital_write(u8g, U8G_PI_CS, LOW);
      }
      break;

    case U8G_COM_MSG_WRITE_BYTE:
      u8g_com_arduino_do_shift_out_msb_first( arg_val );
      //u8g_arduino_sw_spi_shift_out(u8g->pin_list[U8G_PI_MOSI], u8g->pin_list[U8G_PI_SCK], arg_val);
      break;
    
    case U8G_COM_MSG_WRITE_SEQ:
      {
        register uint8_t *ptr = arg_ptr;
        while( arg_val > 0 )
        {
          u8g_com_arduino_do_shift_out_msb_first(*ptr++);
          // u8g_arduino_sw_spi_shift_out(u8g->pin_list[U8G_PI_MOSI], u8g->pin_list[U8G_PI_SCK], *ptr++);
          arg_val--;
        }
      }
      break;

      case U8G_COM_MSG_WRITE_SEQ_P:
      {
        register uint8_t *ptr = arg_ptr;
        while( arg_val > 0 )
        {
          u8g_com_arduino_do_shift_out_msb_first( u8g_pgm_read(ptr) );
          //u8g_arduino_sw_spi_shift_out(u8g->pin_list[U8G_PI_MOSI], u8g->pin_list[U8G_PI_SCK], u8g_pgm_read(ptr));
          ptr++;
          arg_val--;
        }
      }
      break;
      
    case U8G_COM_MSG_ADDRESS:                     /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
      u8g_com_arduino_digital_write(u8g, U8G_PI_A0, arg_val);
      break;
  }
  return 1;
}

#else /* ARDUINO */

uint8_t u8g_com_arduino_sw_spi_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr)
{
  return 1;
}

#endif /* ARDUINO */
