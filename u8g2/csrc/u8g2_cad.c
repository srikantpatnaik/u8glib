/*
  
  u8g2_cad.c
  
  "command arg data" interface to the graphics controller

  The following sequence must be used for any data, which is set to the display:
  
  
  uint8_t u8g2_cad_StartTransfer(u8g2_t *u8g2)

  any of the following calls
    uint8_t u8g2_cad_SendCmd(u8g2_t *u8g2, uint8_t cmd)
    uint8_t u8g2_cad_SendArg(u8g2_t *u8g2, uint8_t arg)
    uint8_t u8g2_cad_SendData(u8g2_t *u8g2, uint8_t cnt, uint8_t *data)
  
  uint8_t u8g2_cad_EndTransfer(u8g2_t *u8g2)



*/
/*
uint8_t u8g2_cad_template(u8g2_t *u8g2, uint8_t msg, uint16_t arg_int, void *arg_ptr)
{
  uint8_t i;
  
  switch(msg)
  {
    case U8G2_MSG_CAD_SEND_CMD:
      u8g2_mcd_byte_SetDC(mcd->next, 1);
      u8g2_mcd_byte_Send(mcd->next, arg_int);
      break;
    case U8G2_MSG_CAD_SEND_ARG:
      u8g2_mcd_byte_SetDC(mcd->next, 1);
      u8g2_mcd_byte_Send(mcd->next, arg_int);
      break;
    case U8G2_MSG_CAD_SEND_DATA:
      u8g2_mcd_byte_SetDC(mcd->next, 0);
      for( i = 0; i < 8; i++ )
	u8g2_mcd_byte_Send(mcd->next, ((uint8_t *)arg_ptr)[i]);
      break;
    case U8G2_MSG_CAD_RESET:
      return mcd->next->cb(mcd->next, msg, arg_int, arg_ptr);
    case U8G2_MSG_CAD_START_TRANSFER:
      return mcd->next->cb(mcd->next, msg, arg_int, arg_ptr);
    case U8G2_MSG_CAD_END_TRANSFER:
      return mcd->next->cb(mcd->next, msg, arg_int, arg_ptr);
    case U8G2_MSG_CAD_SET_I2C_ADR:
      return mcd->next->cb(mcd->next, msg, arg_int, arg_ptr);
    case U8G2_MSG_CAD_SET_DEVICE:
      return mcd->next->cb(mcd->next, msg, arg_int, arg_ptr);
    default:
      break;
  }
  return 1;
}

*/

#include "u8g2.h"

uint8_t u8g2_cad_SendCmd(u8g2_t *u8g2, uint8_t cmd)
{
  return u8g2->cad_cb(u8g2, U8G2_MSG_CAD_SEND_CMD, cmd, NULL);
}

uint8_t u8g2_cad_SendArg(u8g2_t *u8g2, uint8_t arg)
{
  return u8g2->cad_cb(u8g2, U8G2_MSG_CAD_SEND_ARG, arg, NULL);
}

uint8_t u8g2_cad_SendData(u8g2_t *u8g2, uint8_t cnt, uint8_t *data)
{
  return u8g2->cad_cb(u8g2, U8G2_MSG_CAD_SEND_DATA, cnt, data);
}

uint8_t u8g2_cad_StartTransfer(u8g2_t *u8g2)
{
  return u8g2->cad_cb(u8g2, U8G2_MSG_CAD_START_TRANSFER, 0, NULL);
}

uint8_t u8g2_cad_EndTransfer(u8g2_t *u8g2)
{
  return u8g2->cad_cb(u8g2, U8G2_MSG_CAD_END_TRANSFER, 0, NULL);
}

/*
  0000ccaa	command arg combination, aa = no of args, cc = no of commands
  0001dddd	data sequence
  11110000	CS Off
  11110001	CS On
  11111110	xxxxxxxx		delay in millis
  11111111	End of sequence

*/

void u8g2_cad_SendSequence(u8g2_t *u8g2, uint8_t const *data)
{
  uint8_t cmd;
  uint8_t v;

  for(;;)
  {
    cmd = *data;
    data++;
    switch( cmd )
    {
      case U8G2_MSG_CAD_SEND_CMD:
      case U8G2_MSG_CAD_SEND_ARG:
	  v = *data;
	  u8g2->cad_cb(u8g2, cmd, v, NULL);
	  data++;
	  break;
      case U8G2_MSG_CAD_SEND_DATA:
	  v = *data;
	  u8g2_cad_SendData(u8g2, 1, &v);
	  data++;
	  break;
      case U8G2_MSG_CAD_START_TRANSFER:
      case U8G2_MSG_CAD_END_TRANSFER:
	  u8g2->cad_cb(u8g2, cmd, 0, NULL);
	  break;
      case 0x0fe:
	  v = *data;
	  u8g2_gpio_Delay(u8g2, U8G2_MSG_DELAY_MILLI, v);	    
	  data++;
	  break;
      default:
	return;
    }
  }
}


/*
  convert to bytes by using 
    dc = 1 for commands and args and
    dc = 0 for data
*/
uint8_t u8g2_cad_110(u8g2_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8G2_MSG_CAD_SEND_CMD:
      u8g2_byte_SetDC(u8g2, 1);
      u8g2_byte_SendByte(u8g2, arg_int);
      break;
    case U8G2_MSG_CAD_SEND_ARG:
      u8g2_byte_SetDC(u8g2, 1);
      u8g2_byte_SendByte(u8g2, arg_int);
      break;
    case U8G2_MSG_CAD_SEND_DATA:
      u8g2_byte_SetDC(u8g2, 0);
      //u8g2_byte_SendBytes(u8g2, arg_int, arg_ptr);
      //break;
      /* fall through */
    case U8G2_MSG_CAD_INIT:
    case U8G2_MSG_CAD_START_TRANSFER:
    case U8G2_MSG_CAD_END_TRANSFER:
    case U8G2_MSG_CAD_SET_I2C_ADR:
    case U8G2_MSG_CAD_SET_DEVICE:
      return u8g2->byte_cb(u8g2, msg, arg_int, arg_ptr);
    default:
      return 0;
  }
  return 1;
}

/*
  convert to bytes by using 
    dc = 0 for commands and args and
    dc = 1 for data
*/
uint8_t u8g2_cad_001(u8g2_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8G2_MSG_CAD_SEND_CMD:
      u8g2_byte_SetDC(u8g2, 0);
      u8g2_byte_SendByte(u8g2, arg_int);
      break;
    case U8G2_MSG_CAD_SEND_ARG:
      u8g2_byte_SetDC(u8g2, 0);
      u8g2_byte_SendByte(u8g2, arg_int);
      break;
    case U8G2_MSG_CAD_SEND_DATA:
      u8g2_byte_SetDC(u8g2, 1);
      //u8g2_byte_SendBytes(u8g2, arg_int, arg_ptr);
      //break;
      /* fall through */
    case U8G2_MSG_CAD_INIT:
    case U8G2_MSG_CAD_START_TRANSFER:
    case U8G2_MSG_CAD_END_TRANSFER:
    case U8G2_MSG_CAD_SET_I2C_ADR:
    case U8G2_MSG_CAD_SET_DEVICE:
      return u8g2->byte_cb(u8g2, msg, arg_int, arg_ptr);
    default:
      return 0;
  }
  return 1;
}

