/*
 * lcd.c
 *
 *  Created on: 03.04.2017
 *      Author: fabian
 */

#include "lcd.h"
#include "charset.h"
#include "log.h"

#include "string.h"
#include "stdlib.h"

lcd_spi_callback _lcd_spi;
lcd_pin_callback _lcd_reset_set;
lcd_pin_callback _lcd_a0_set;
lcd_pin_callback _lcd_select_set;

uint8_t _lcd_width, _lcd_height;

void _lcd_data(uint8_t data)
{
	// initiate data transfer
	_lcd_a0_set(1);

	// send data
	_lcd_select_set(0);
	_lcd_spi(data);
	_lcd_select_set(1);
}

void lcd_init(lcd_spi_callback spi_callback, lcd_delay_callback delay_callback,
	      lcd_pin_callback reset_callback, lcd_pin_callback a0_callback,
		  lcd_pin_callback select_callback, uint8_t lcd_width, uint8_t lcd_height)
{
	_lcd_width = lcd_width;
	_lcd_height = lcd_height;
	_lcd_spi = spi_callback;
	_lcd_reset_set = reset_callback;
	_lcd_a0_set = a0_callback;
	_lcd_select_set = select_callback;

	_lcd_reset_set(0);
	delay_callback(100);
	_lcd_reset_set(1);

	lcd_command(0xA0); // ADC mode: normal
	lcd_command(0xC8); // COM scan direction: reverse
	lcd_command(0xA3); // LCD bias setting

	lcd_command(0x2C); // voltage booster: ON
	delay_callback(100);
	lcd_command(0x2E); // and voltage regulator: ON
	delay_callback(100);
	lcd_command(0x2F); // and voltage follower: ON

	lcd_command(0x24); // voltage regulator ratio
	lcd_command(0x81); // prepare write to Electronic Volume Register
	lcd_command(0x15); // write to Electronic Volume Register
	delay_callback(100);

	lcd_command(0xAF); // display: ON
	lcd_command(0x40); // start address: 0

	lcd_clear();
}

#ifdef LCD_LOGGING
	void lcd_setlogfunction(lcd_log_callback log_callback)
	{
		_lcd_log = log_callback;
		lcd_logstr("logging function set", "lcd_setlogfunction");
	}
#endif

void lcd_command(uint8_t cmd)
{
	// initiate command transfer
	_lcd_a0_set(0);

	// send command
	_lcd_select_set(0);
	_lcd_spi(cmd);
	_lcd_select_set(1);
}

void _lcd_chardat(uint8_t dat, bool inv, uint8_t* x)
{
	if(inv)
		dat = ~dat;

	_lcd_data(dat);

	++(*x);

	if((*x) >= _lcd_width)
		return;
}

void _lcd_char(char c, bool inv, bool space, uint8_t* x)
{
	uint8_t dat;

#ifdef LCD_LOGGING
	lcd_logchar(c, "_lcd_char");
#endif

	if((c >= 0x21) && (c <= 0xEF))
	{
		const uint8_t *addr = &lcd_charset[(c - 0x21) * 5];

		for(uint8_t i = 0; i < 5; ++i)
		{
			dat = LCD_MEM_READ(addr + i);

			if(dat != 0)
			{
				_lcd_chardat(dat, inv, x);
			}
		}
	}
	else if(c == ' ')
	{
		_lcd_chardat(0, inv, x);
	}
	else
	{
		for(uint8_t i = 0; i < 5; ++i)
		{
			uint8_t dat = LCD_MEM_READ(lcd_charunknown + i);

			_lcd_chardat(dat, inv, x);
		}
	}

	if(space)
	{
		_lcd_chardat(0, inv, x);
	}
}

void lcd_printtext(uint8_t x, uint8_t line, char* str, bool inv)
{
#ifdef LCD_LOGGING
	lcd_logstr(str, "lcd_printtext");
	lcd_logint(_lcd_height, "lcd_printtext height");
	lcd_logint(_lcd_width, "lcd_printtext width");
	lcd_logint(line, "lcd_printtext line");
	lcd_logint(x, "lcd_printtext x");
#endif

	if((line >= (_lcd_height >> 3)) || (x >= _lcd_width))
		return;

	lcd_command(LCD_CMDPAGEADDRSET | line);
	lcd_command(LCD_CMDCOLUMNADDRSETH | (x >> 4));
	lcd_command(LCD_CMDCOLUMNADDRSETL | (x & 0x0F));

#ifdef LCD_LOGGING
	lcd_logstr(str, "lcd_printtext");
#endif

	uint8_t px = x;

	while((*str) != 0)
	{
#ifdef LCD_LOGGING
	lcd_logchar(*str, "lcd_printtext");
#endif

		switch(*str)
		{
		case '\n':
			lcd_command(LCD_CMDPAGEADDRSET | (++line));

			if(line >= (_lcd_height >> 8))
				return;

			/* no break */
		case '\r':
			lcd_command(LCD_CMDCOLUMNADDRSETH | (x >> 4));
			lcd_command(LCD_CMDCOLUMNADDRSETL | (x & 0x0F));
			px = x;
			break;
		default:
			_lcd_char(*str, inv, *(str + 1) != 0, &px);
		}

		++str;
	}
}

uint8_t _lcd_charwidth(char c)
{
	uint8_t dat;

	if(c < ' ')
		return 0;
	else if((c > ' ') && (c <= 0xEF))
	{
		uint8_t cnt = 0;
		const uint8_t *addr = &lcd_charset[(c - 0x21) * 5];

		for(uint8_t i = 0; i < 5; ++i)
		{
			dat = LCD_MEM_READ(addr + i);

			if(dat != 0)
			{
				++cnt;
			}
		}

		return cnt;
	}
	else if(c == ' ')
	{
		return 3;
	}
	else
	{
		return 5;
	}
}

char* _lcd_linewidth(char* str, uint16_t* len)
{
	(*len) = 0;

	while(1)
	{
		switch(*str)
		{
		case '\n':
			--(*len);
			++str;
			/* no break */
		case 0:
			return str;
		default:
			(*len) += _lcd_charwidth(*str) + 1;
		}

		++str;
	}
}

void lcd_textsize(char* str, uint16_t* width, uint16_t* lines)
{
	uint16_t linelen;
	uint16_t maxlen = 0;

	(*lines) = 0;

	while((*str) != 0)
	{
		str = _lcd_linewidth(str, &linelen);

		++(*lines);

		if(linelen > maxlen)
			maxlen = linelen;
	}

	(*width) = maxlen;
}

void lcd_printbitmap(uint8_t x, uint8_t line, LCD_BITMAP* bmp)
{
	uint8_t lmax = (bmp->height >> 3);
	uint8_t xmax = bmp->width;

	uint8_t* addr = bmp->data;

	if((lmax + line) >= (_lcd_height >> 3))
		lmax = (_lcd_height >> 3) - 1 - line;
	if((xmax + x) >= _lcd_width)
		xmax = _lcd_width - 1 - x;

	for(uint8_t py = 0; py < lmax; ++py, addr += bmp->width, ++line)
	{
		lcd_command(LCD_CMDPAGEADDRSET | line);
		lcd_command(LCD_CMDCOLUMNADDRSETH | (x >> 4));
		lcd_command(LCD_CMDCOLUMNADDRSETL | (x & 0x0F));

		for(uint8_t px = 0; px < xmax; ++px)
		{
			_lcd_data(addr[px]);
		}
	}
}


void lcd_printgraphicbitmap(uint8_t x, uint8_t line, LCD_BITMAP* bmp, LCD_GRAPHICS* g)
{
	uint8_t lmax = (g->dirty_y1 >> 3);
	uint8_t lmin = (g->dirty_y0 >> 3);
	uint8_t xmax = g->dirty_x1;
	uint8_t xmin = g->dirty_x0;

	if((lmax + line) >= (_lcd_height >> 3))
		lmax = (_lcd_height >> 3) - 1 - line;
	if((xmax + x) >= _lcd_width)
		xmax = _lcd_width - 1 - x;
	if((lmin + line) >= (_lcd_height >> 3))
		return;
	if((xmin + x) >= _lcd_width)
		return;

	x += xmin;
	line += lmin;

	uint8_t* addr = bmp->data + (lmin * bmp->width);

	for(uint8_t py = lmin; py <= lmax; ++py, addr += bmp->width, ++line)
	{
		lcd_command(LCD_CMDPAGEADDRSET | line);
		lcd_command(LCD_CMDCOLUMNADDRSETH | (x >> 4));
		lcd_command(LCD_CMDCOLUMNADDRSETL | (x & 0x0F));

		for(uint8_t px = xmin; px <= xmax; ++px)
		{
			_lcd_data(addr[px]);
		}
	}

	g->dirty = false;
}

void lcd_clear()
{
	for(uint8_t y = 0; y < (_lcd_height >> 3); ++y)
	{
		lcd_command(LCD_CMDPAGEADDRSET | y);
		lcd_command(LCD_CMDCOLUMNADDRSETH | 0);
		lcd_command(LCD_CMDCOLUMNADDRSETL | 0);

		for(uint8_t x = 0; x < _lcd_width; ++x)
		{
			_lcd_data(0);
		}
	}
}
