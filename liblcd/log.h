/*
 * log.h
 *
 *  Created on: 20.10.2017
 *      Author: fabiangeissler
 */

#ifndef LOG_H_
#define LOG_H_

#include "settings.h"

#ifdef LCD_LOGGING

#include "callbacks.h"

extern lcd_log_callback _lcd_log;

int lcd_ramcheck ();

void lcd_logstr(char* str, char* marker);

void lcd_logchar(char c, char* marker);

void lcd_logint(int i, char* marker);
#endif

#endif /* LOG_H_ */
