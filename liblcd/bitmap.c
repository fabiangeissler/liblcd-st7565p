/*
 * region.c
 *
 *  Created on: 18.10.2017
 *      Author: fabiangeissler
 */

#include "bitmap.h"

#include "stdlib.h"
#include "string.h"

uint16_t _lcd_totalmemlimit = 0xFFFF;
uint16_t _lcd_totalmemory = 0;

uint16_t lcd_getbitmapmemusage()
{
	return _lcd_totalmemory;
}

bool lcd_settotalbitmapmemlimit(uint16_t limit)
{
	if(limit < _lcd_totalmemory)
		return false;

	_lcd_totalmemlimit = limit;

	return true;
}

bool lcd_bitmapalloc(LCD_BITMAP* bmp, uint8_t width, uint8_t height)
{
	bmp->width = width;
	bmp->height = height;

	// bpc = (height / 8) + (height mod 8 == 0) ? 0 : 1;
	bmp->bpc = (height >> 3) + (((height & 0b111) == 0) ? 0 : 1);

	// calculate region size in bytes
	bmp->size = bmp->bpc * bmp->width;

	if(_lcd_totalmemory + bmp->size > _lcd_totalmemlimit)
	{
		bmp->data = 0;
		return false;
	}
	else
	{
		// allocate memory for region data
		if((bmp->data = malloc(bmp->size)) == 0)
			return false;

		_lcd_totalmemory += bmp->size;

		return true;
	}
}

void lcd_bitmapfree(LCD_BITMAP* bmp)
{
	if(bmp->data == 0)
		return;

	_lcd_totalmemory -= bmp->size;

	free(bmp->data);
	bmp->data = 0;
}

void lcd_bitmapclear(LCD_BITMAP* bmp, bool inv)
{
	if(inv)
		memset(bmp->data, 0xFFFF, bmp->size);
	else
		memset(bmp->data, 0, bmp->size);
}
