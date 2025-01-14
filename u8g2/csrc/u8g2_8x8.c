/*

  u8g2_8x8.c
  
  font procedures, directly interfaces display procedures

*/

#include "u8g2.h"

void u8g2_Set8x8Font(u8g2_t *u8g2, const uint8_t *font_8x8)
{
  u8g2->font = font_8x8;
}

void u8g2_Draw8x8Glyph(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t encoding)
{
  uint8_t first, last, i;
  uint8_t buf[8];
  uint16_t offset;
  first = u8g2_pgm_read(u8g2->font+0);
  last = u8g2_pgm_read(u8g2->font+1);
  
  if ( first <= encoding && encoding <= last )
  {
    offset = encoding;
    offset -= first;
    offset *= 8;
    offset +=2;
    for( i = 0; i < 8; i++ )
    {
      buf[i] = u8g2_pgm_read(u8g2->font+offset);
      offset++;
    }
  }
  else
  {
    for( i = 0; i < 8; i++ )
    {
      buf[i] = 0;
    }
  }
  u8g2_display_DrawTile(u8g2, x, y, 1, buf);
}

void u8g2_Draw8x8String(u8g2_t *u8g2, uint8_t x, uint8_t y, const char *s)
{
  while( *s != '\0'  )
  {
    u8g2_Draw8x8Glyph(u8g2, x, y, (uint8_t)*s);
    s++;
    x++;
  }
}

