/*
 * settings.h
 *
 *  Created on: 18.10.2017
 *      Author: fabiangeissler
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#define LCD_ARCH_AVR
//#define LCD_LOGGING

#ifdef LCD_ARCH_AVR // store charset in device flash memory
	#include "avr/pgmspace.h"
	#include "util/delay.h"
	#define LCD_MEMORY PROGMEM
	#define LCD_MEM_READ(addr) pgm_read_byte(addr)
#elif LCD_ARCH_UNKNOWN // store charset in RAM
	#define LCD_MEMORY
	#define LCD_MEM_READ(addr) (*((char*)addr))
#endif

#endif /* SETTINGS_H_ */
