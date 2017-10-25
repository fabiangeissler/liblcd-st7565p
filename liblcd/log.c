/*
 * log.c
 *
 *  Created on: 20.10.2017
 *      Author: fabiangeissler
 */

#include "log.h"

#ifdef LCD_LOGGING

#include "stdlib.h"

lcd_log_callback _lcd_log = 0;

void lcd_logstr(char* str, char* marker)
{
	if(_lcd_log != 0)
	{
		char txt[7];

		itoa(lcd_ramcheck(), txt, 10);

		_lcd_log("Mem: ");
		_lcd_log(txt);
		_lcd_log(" Mark: ");
		_lcd_log(marker);
		_lcd_log(" Message: ");
		_lcd_log(str);
		_lcd_log("\n");
	}
}

void lcd_logchar(char c, char* marker)
{
	char cstr[4] = {'\'', c, '\'', 0};
	lcd_logstr(cstr, marker);
}

void lcd_logint(int i, char* marker)
{
	char cstr[7];
	itoa(i, cstr, 10);
	lcd_logstr(cstr, marker);
}

int lcd_ramcheck () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#endif
