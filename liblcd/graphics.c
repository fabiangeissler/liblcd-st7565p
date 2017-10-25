/*
 * graphics.c
 *
 *  Created on: 18.10.2017
 *      Author: fabiangeissler
 */

#include "graphics.h"
#include "charset.h"

#ifdef LCD_LOGGING
	#include "log.h"
#endif

#include "stdbool.h"
#include "stdlib.h"
#include "string.h"

#ifndef MAX
	#define MAX(a, b) ((a < b) ? b : a)
#endif

#ifndef MIN
	#define MIN(a, b) ((a < b) ? a : b)
#endif

void _lcd_drawpixel(LCD_BITMAP* canvas, uint16_t addr, uint8_t bit, uint8_t mode)
{
	switch(mode & 0b11)
	{
	case LCD_CLEARPOINT:
		canvas->data[addr] &= ~(1 << bit);
		break;
	case LCD_SETPOINT:
		canvas->data[addr] |= (1 << bit);
		break;
	case LCD_INVERTPOINT:
		canvas->data[addr] ^= (1 << bit);
		break;
	}
}

void _lcd_refreshdirtyrect(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x0, int y0, int x1, int y1)
{
	if(x0 > x1)
	{
		int t = x0;
		x0 = x1;
		x1 = t;
	}

	if(y0 > y1)
	{
		int t = y0;
		y0 = y1;
		y1 = t;
	}

	// one pixel margin for thick points
	--x0; --y0;
	++x1; ++y1;

	if(x0 < 0)
		x0 = 0;
	if(y0 < 0)
		y0 = 0;
	if(x1 >= canvas->width)
		x1 = canvas->width - 1;
	if(y1 >= canvas->height)
		y1 = canvas->height - 1;

	if((x0 > x1) || (y0 > y1))
		return;

	if(mode->dirty)
	{
		mode->dirty_x0 = MIN(mode->dirty_x0, x0);
		mode->dirty_y0 = MIN(mode->dirty_y0, y0);
		mode->dirty_x1 = MAX(mode->dirty_x1, x1);
		mode->dirty_y1 = MAX(mode->dirty_y1, y1);
	}
	else
	{
		mode->dirty = true;
		mode->dirty_x0 = x0;
		mode->dirty_y0 = y0;
		mode->dirty_x1 = x1;
		mode->dirty_y1 = y1;
	}
}

void _lcd_drawpoint(LCD_BITMAP* canvas, int x, int y, uint8_t mode)
{
	if((x < 0) || (x >= canvas->width) || (y < 0) || (y >= canvas->height))
		return;

	uint16_t byteaddr = (y >> 3) * canvas->width + x;
	uint8_t bit = (y & 0b111);

	_lcd_drawpixel(canvas, byteaddr, bit, mode);

	if(mode & LCD_THICKPOINT)
	{
		if(x > 0)
			_lcd_drawpixel(canvas, byteaddr - 1, bit, mode);
		if(x < (canvas->width - 1))
			_lcd_drawpixel(canvas, byteaddr + 1, bit, mode);
		if(y > 0)
		{
			if(bit == 0)
				_lcd_drawpixel(canvas, byteaddr - canvas->width, 7, mode);
			else
				_lcd_drawpixel(canvas, byteaddr, bit - 1, mode);
		}
		if(y < (canvas->height - 1))
		{
			if(bit == 7)
				_lcd_drawpixel(canvas, byteaddr + canvas->width, 0, mode);
			else
				_lcd_drawpixel(canvas, byteaddr, bit + 1, mode);
		}
	}
}

void lcd_drawpoint(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x, int y)
{
	_lcd_drawpoint(canvas, mode->graphicsmode, x, y);
	_lcd_refreshdirtyrect(canvas, mode, x, y, x, y);
}

void lcd_drawline(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x0, int y0, int x1, int y1)
{
	// bresenham implementation taken from wikipedia (18.10.2017)
	// https://de.wikipedia.org/wiki/Bresenham-Algorithmus

	int dx =  abs(x1 - x0), sx = (x0 < x1) ? 1 : -1;
	int dy = -abs(y1 - y0), sy = (y0 < y1) ? 1 : -1;
	int err = dx + dy, e2; /* error value e_xy */

	while(1)
	{
		_lcd_drawpoint(canvas, mode->graphicsmode, x0, y0);

		if (x0 == x1 && y0 == y1)
			break;

		e2 = 2 * err;

		if (e2 > dy)
		{ /* e_xy+e_x > 0 */
			err += dy;
			x0 += sx;
		}

		if (e2 < dx)
		{ /* e_xy+e_y < 0 */
			err += dx;
			y0 += sy;
		}
	}

	_lcd_refreshdirtyrect(canvas, mode, x0, y0, x1, y1);
}

void lcd_drawcircle(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x0, int y0, int radius)
{
	// circle bresenham implementation taken from wikipedia (18.10.2017)
	// https://de.wikipedia.org/wiki/Bresenham-Algorithmus

    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    _lcd_drawpoint(canvas, x0, y0 + radius, mode->graphicsmode);
    _lcd_drawpoint(canvas, x0, y0 - radius, mode->graphicsmode);
    _lcd_drawpoint(canvas, x0 + radius, y0, mode->graphicsmode);
    _lcd_drawpoint(canvas, x0 - radius, y0, mode->graphicsmode);

    while(x < y)
    {
		if(f >= 0)
		{
			--y;
			ddF_y += 2;
			f += ddF_y;
		}

		++x;
		ddF_x += 2;
		f += ddF_x + 1;

		_lcd_drawpoint(canvas, x0 + x, y0 + y, mode->graphicsmode);
		_lcd_drawpoint(canvas, x0 - x, y0 + y, mode->graphicsmode);
		_lcd_drawpoint(canvas, x0 + x, y0 - y, mode->graphicsmode);
		_lcd_drawpoint(canvas, x0 - x, y0 - y, mode->graphicsmode);
		_lcd_drawpoint(canvas, x0 + y, y0 + x, mode->graphicsmode);
		_lcd_drawpoint(canvas, x0 - y, y0 + x, mode->graphicsmode);
		_lcd_drawpoint(canvas, x0 + y, y0 - x, mode->graphicsmode);
		_lcd_drawpoint(canvas, x0 - y, y0 - x, mode->graphicsmode);
    }

	_lcd_refreshdirtyrect(canvas, mode, x0 - radius, y0 - radius, x0 + radius, y0 + radius);
}

void _lcd_drawbyte(uint8_t* addr, uint8_t value, uint8_t mode)
{
	switch(mode & 0b11)
	{
	case LCD_CLEARPOINT:
		(*addr) &= ~value;
		break;
	case LCD_SETPOINT:
		(*addr) |= value;
		break;
	case LCD_INVERTPOINT:
		(*addr) ^= value;
		break;
	}
}

void _lcd_fillline(uint8_t* startaddr, uint8_t dx, uint8_t pattern, uint8_t mode)
{
	for(int x = 0; x <= dx; ++x)
		_lcd_drawbyte(startaddr++, pattern, mode);
}

void lcd_fillrect(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x0, int y0, int x1, int y1)
{
	if(x0 < 0)
		x0 = 0;
	if(y0 < 0)
		y0 = 0;
	if(x1 >= canvas->width)
		x1 = canvas->width - 1;
	if(y1 >= canvas->height)
		y1 = canvas->height - 1;
	if((x0 > x1) || (y0 > y1))
		return;

	int nextliney = (8 - (y0 & 0b111) + y0);

#ifdef LCD_LOGGING
	lcd_logint(nextliney, "lcd_fillrect nly");
#endif

	uint8_t mod0 = (y0 & 0b111);
	uint8_t mod1 = (y1 & 0b111);
	uint8_t* sline = (uint8_t*)(canvas->data + x0 + ((y0 >> 3) * canvas->width));
	uint8_t dx = x1 - x0;
	uint8_t pattern = 0;

	if(y1 < nextliney)
	{
		for(int i = mod0; i <= mod1; ++i)
			pattern |= (1 << i);

		_lcd_fillline(sline, dx, pattern, mode->graphicsmode);
	}
	else
	{
		uint8_t* eline = (uint8_t*)(canvas->data + x0 + ((y1 >> 3) * canvas->width));

		for(int i = mod0; i <= 7; ++i)
			pattern |= (1 << i);

		_lcd_fillline(sline, dx, pattern, mode->graphicsmode);

		for(uint8_t* l = (sline + canvas->width); l < eline; l += canvas->width)
		{
			_lcd_fillline(l, dx, 0xFF, mode->graphicsmode);
		}

		pattern = 0;

		for(int i = 0; i <= mod1; ++i)
			pattern |= (1 << i);

		_lcd_fillline(eline, dx, pattern, mode->graphicsmode);
	}

	_lcd_refreshdirtyrect(canvas, mode, x0, y0, x1, y1);
}

//void lcd_drawbitmap(LCD_BITMAP* canvas, int x, int y, LCD_BITMAP* source, uint8_t mode)
//{
//	uint8_t ymin = (y < 0) ? -y : 0;
//	uint8_t xmin = (x < 0) ? -x : 0;
//
//	uint8_t ymax = ((srcpxheight - ymin) > cvspxheight) ? (ymin + cvspxheight) : srcpxheight;
//	uint8_t xmax = ((source->width - xmin) > canvas->width) ? (xmin + canvas->width) : source->width;
//
//	for(uint8_t yp = ymin; yp < ymax; ++yp)
//	{
//
//	}
//}

void _lcd_drawhalfchar(LCD_BITMAP* canvas, int* x, uint8_t* lineaddr, int8_t shift_bits, char c, uint8_t mode, bool space)
{
	uint8_t dat, sh;

	if((c >= 0x21) && (c <= 0xEF))
	{
		const uint8_t *addr = &lcd_charset[(c - 0x21) * 5];

		for(uint8_t i = 0; i < 5; ++i)
		{
			if((*x) >= canvas->width)
				break;

			if((*x) >= 0)
			{
				dat = LCD_MEM_READ(addr + i);

				if(shift_bits < 0)
					sh = dat >> (-shift_bits);
				else
					sh = dat << shift_bits;

				_lcd_drawbyte(lineaddr + (*x), sh, mode);
			}

			++(*x);
		}
	}
	else if(c == ' ')
	{
		for(int i = 0; i < 3; ++i)
		{
			if((*x) >= canvas->width)
				break;

			if((*x) >= 0)
				_lcd_drawbyte(lineaddr + (*x)++, 0, mode);
		}
	}
	else
	{
		for(uint8_t i = 0; i < 5; ++i)
		{

			if((*x) >= canvas->width)
				break;

			if((*x) >= 0)
			{
				dat = LCD_MEM_READ(lcd_charunknown + i);

				if(shift_bits < 0)
					sh = dat >> (-shift_bits);
				else
					sh = dat << shift_bits;

				_lcd_drawbyte(lineaddr + (*x), sh, mode);
			}

			++(*x);
		}
	}

	if(space)
	{
		if(((*x) >= 0) && ((*x) < canvas->width))
			_lcd_drawbyte(lineaddr + (*x)++, 0, mode);
	}
}

char* _lcd_drawhalfline(LCD_BITMAP* canvas, int x, uint8_t* lineaddr, uint8_t shift_bits, char* txt, uint8_t mode, int* maxx)
{
#ifdef LCD_LOGGING
	lcd_logstr(txt, "_lcd_drawhalfline txt");
	lcd_logint((int)lineaddr, "_lcd_drawhalfline line");
	lcd_logint((int)canvas->data, "_lcd_drawhalfline data");
#endif

	int rx = x;

	while(1)
	{
#ifdef LCD_LOGGING
		lcd_logchar((*txt), "_lcd_drawhalfline");
#endif

		switch(*txt)
		{
		case '\n':
			++txt;
			/* no break */
		case 0:
			(*maxx) = MAX(rx, (*maxx));
			return txt;
		case '\r':
			(*maxx) = MAX(rx, (*maxx));
			rx = x;
			break;
		default:
			_lcd_drawhalfchar(canvas, &rx, lineaddr, shift_bits, (*txt), mode, (*(txt + 1)) != '\n');
		}

		++txt;
	}
}

void lcd_drawtext(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x, int y, char* txt)
{
#ifdef LCD_LOGGING
	lcd_logstr("enter", "lcd_drawtext");
#endif

	uint8_t* line = (uint8_t*)(canvas->data + ((y < 0) ? 0 : ((y >> 3) * canvas->width)));
	int8_t mod = y & 0b111;
	int8_t nmod = mod - 8;
	int maxx = x;
	int miny = y;

	while((*txt) != 0)
	{
		if(y <= -8)
		{
			if((txt = strchr(txt, '\n')) == 0)
				return;
		}
		else if(y < 0)
		{
			txt = _lcd_drawhalfline(canvas, x, canvas->data, y, txt, mode->graphicsmode, &maxx);
		}
		else if(y <= (canvas->height - 8))
		{
			char* tmp = txt;

			txt = _lcd_drawhalfline(canvas, x, line, mod, txt, mode->graphicsmode, &maxx);
			line += canvas->width;

			if(mod != 0)
				_lcd_drawhalfline(canvas, x, line, nmod, tmp, mode->graphicsmode, &maxx);
		}
		else if(y < canvas->height)
		{
			_lcd_drawhalfline(canvas, x, line, mod, txt, mode->graphicsmode, &maxx);
			y += 8;
			return;
		}

		y += 8;
	}

	_lcd_refreshdirtyrect(canvas, mode, x, miny, maxx, y);

#ifdef LCD_LOGGING
	lcd_logstr("exit", "lcd_drawtext");
#endif
}
