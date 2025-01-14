
#include "u8g2.h"




static const uint8_t u8g2_d_uc1701_dogs102_init_seq[] = {
    
  U8G2_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  
  U8G2_C(0x0e2),            			/* soft reset */
  U8G2_C(0x0ae),		                /* display off */
  U8G2_C(0x040),		                /* set display start line to 0 */
  U8G2_C(0x0a1),		                /* ADC set to reverse */
  U8G2_C(0x0c0),		                /* common output mode */
  U8G2_C(0x0a6),		                /* display normal, bit val 0: LCD pixel off. */
  U8G2_C(0x0a2),		                /* LCD bias 1/9 */
  U8G2_C(0x02f),		                /* all power  control circuits on */
  U8G2_C(0x027),		                /* regulator, booster and follower */
  U8G2_C(0x081),		                /* set contrast */
  U8G2_C(0x00e),		                /* contrast value, EA default: 0x010, previous value for S102: 0x0e */
  U8G2_C(0x0fa),		                /* Set Temp compensation */ 
  U8G2_C(0x090),		                /* 0.11 deg/c WP Off WC Off*/
  U8G2_C(0x0a4),		                /* normal display  */
  //U8G2_C(0x0af),		                /* display on */

  
  //U8G2_DLY(50),       			/* delay 100 ms */
  //U8G2_C(0x0a5),		                /* display all points, ST7565, UC1610 */
  //U8G2_DLY(50),       			/* delay 100 ms */
  //U8G2_DLY(50),       			/* delay 100 ms */
  //U8G2_C(0x0a4),		                /* normal display */
  
  //U8G2_C(0x010),		                /* col hi adr 0 */
  //U8G2_C(0x000),		                /* col lo adr 0 */
  //U8G2_C(0x0b0),		                /* page 0 */
  
  //U8G2_D1(0x0ff),		                /* data */
  //U8G2_D1(0x0ff),		                /* data */
  //U8G2_D1(0x0ff),		                /* data */
  //U8G2_D1(0x0ff),		                /* data */
  //U8G2_D1(0x0ff),		                /* data */
  //U8G2_D1(0x0ff),		                /* data */
  //U8G2_D1(0x0ff),		                /* data */
  //U8G2_D1(0x0ff),		                /* data */
  
  
  U8G2_END_TRANSFER(),             	/* disable chip */
  U8G2_END()             			/* end of sequence */
};

static const uint8_t u8g2_d_uc1701_dogs102_power_up_seq[] = {
  U8G2_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8G2_C(0x0af),		                /* display on */
  U8G2_END_TRANSFER(),             	/* disable chip */
  U8G2_END()             			/* end of sequence */
};

static const uint8_t u8g2_d_uc1701_dogs102_power_down_seq[] = {
  U8G2_START_TRANSFER(),             	/* enable chip, delay is part of the transfer start */
  U8G2_C(0x0ae),		                /* display off */
  U8G2_END_TRANSFER(),             	/* disable chip */
  U8G2_END()             			/* end of sequence */
};

static u8g2_display_info_t u8g2_uc1601_display_info =
{
  /* chip_enable_level = */ 0,
  /* chip_disable_level = */ 1,
  
  /* post_chip_enable_wait_ns = */ 5,
  /* pre_chip_disable_wait_ns = */ 5,
  /* reset_pulse_width_ms = */ 1, 
  /* post_reset_wait_ms = */ 6, 
  /* sda_setup_time_ns = */ 12,
  /* sck_pulse_width_ns = */ 15,
  /* sck_takeover_edge = */ 1,		/* rising edge */
  /* i2c_bus_clock_100kHz = */ 37,
  /* data_setup_time_ns = */ 30,
  /* write_pulse_width_ns = */ 40
};

uint8_t u8g2_d_uc1701_dogs102(u8g2_t *u8g2, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  uint8_t x;
  switch(msg)
  {
    case U8G2_MSG_DISPLAY_INIT:
      /* 1) set display info struct */
      u8g2->display_info = &u8g2_uc1601_display_info;
    
      /* 2) apply port directions to the GPIO lines and apply default values for the IO lines*/
      u8g2_gpio_Init(u8g2);
      u8g2_cad_Init(u8g2);

      /* 3) apply default value for chip select */
      u8g2_gpio_SetCS(u8g2, u8g2->display_info->chip_disable_level);
      /* no wait required here */
    
      /* 4) do reset */
      u8g2_gpio_SetReset(u8g2, 1);
      u8g2_gpio_Delay(u8g2, U8G2_MSG_DELAY_MILLI, u8g2->display_info->reset_pulse_width_ms);
      u8g2_gpio_SetReset(u8g2, 0);
      u8g2_gpio_Delay(u8g2, U8G2_MSG_DELAY_MILLI, u8g2->display_info->reset_pulse_width_ms);
      u8g2_gpio_SetReset(u8g2, 1);
      u8g2_gpio_Delay(u8g2, U8G2_MSG_DELAY_MILLI, u8g2->display_info->post_reset_wait_ms);
    
      /* 5) send startup code */
      u8g2_cad_SendSequence(u8g2, u8g2_d_uc1701_dogs102_init_seq);
    
      break;
    case U8G2_MSG_DISPLAY_POWER_DOWN:
      u8g2_cad_SendSequence(u8g2, u8g2_d_uc1701_dogs102_power_down_seq);
      break;
    case U8G2_MSG_DISPLAY_POWER_UP:
      u8g2_cad_SendSequence(u8g2, u8g2_d_uc1701_dogs102_power_up_seq);
      break;
    case U8G2_MSG_DISPLAY_SET_CONTRAST:
      break;
    case U8G2_MSG_DISPLAY_DRAW_TILE:
      u8g2_cad_StartTransfer(u8g2);
      x = ((u8g2_tile_t *)arg_ptr)->x_pos;
    
      u8g2_cad_SendCmd(u8g2, 0x010 | (x>>1) );
      u8g2_cad_SendCmd(u8g2, 0x000 | ((x&1) << 3));
      u8g2_cad_SendCmd(u8g2, 0x0b0 | (((u8g2_tile_t *)arg_ptr)->y_pos));
      u8g2_cad_SendData(u8g2, 8, ((u8g2_tile_t *)arg_ptr)->tile_ptr);
      u8g2_cad_EndTransfer(u8g2);
      break;
    case U8G2_MSG_DISPLAY_GET_LAYOUT:
      break;
    default:
      return 0;
  }
  return 1;
}


