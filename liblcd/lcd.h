/*
 * lcd.h
 *
 *  Created on: 03.04.2017
 *      Author: fabian
 */

#ifndef LCD_H_
#define LCD_H_

#include "stdbool.h"
#include "stdint.h"
#include "settings.h"

#include "bitmap.h"
#include "graphics.h"
#include "callbacks.h"

/// \brief Logical state of a command.
enum { // LCD command state
	LCD_CMDON = 1,  	///< Command ON
	LCD_CMDOFF = 0	///< Command OFF
};

/// \brief Commands that can be send to the display.
enum lcd_commands { // LCD commands
	LCD_CMDDISPLAY 			= 0xAE,	///< Turn display on or off (OR with CMDON or CMDOFF).
	LCD_CMDSTARTLINESET 	= 0x40,		///< Set start line address (OR with line number).
	LCD_CMDPAGEADDRSET		= 0xB0, ///< Set page address to write to (OR with page number).
	LCD_CMDCOLUMNADDRSETH	= 0x10, ///< Set column address high nibble (OR with high nibble).
	LCD_CMDCOLUMNADDRSETL	= 0x00, ///< Set column address low nibble (OR with low nibble).
	LCD_CMDCOLUMNREVERSE	= 0xA0,		///< Reverse column scan direction (OR with CMDON or CMDOFF).
	LCD_CMDDISPLAYINVERSE	= 0xA6, ///< Inverse all display points (OR with CMDON or CMDOFF).
	LCD_CMDSETALLPOINTS		= 0xA4,	///< Set all points ignoring data and inversion (OR with CMDON or CMDOFF).
};

/**
 * \brief Initialize the lcd library.
 * \param spi_callback Callback function pointer to transceive data on the SPI peripheral.
 * \param delay_callback Callback function pointer for a delay function (it is only used by this initialization method).
 * \param reset_callback Callback function pointer to control the reset pin of the display.
 * \param a0_callback Callback function pointer to control the a0 pin of the display.
 * \param select_callback Callback function pointer to control the chip select pin of the display.
 * \param lcd_width Width of the display in pixels.
 * \param lcd_height Height of the display in pixels.
 */
void lcd_init(lcd_spi_callback spi_callback, lcd_delay_callback delay_callback,
		      lcd_pin_callback reset_callback, lcd_pin_callback a0_callback,
			  lcd_pin_callback select_callback, uint8_t lcd_width, uint8_t lcd_height);

#ifdef LCD_LOGGING
	/**
	 * \brief Set the callback function pointer for the logging function.
	 * \param log_callback Callback function pointer for the logging function (can be NULL).
	 */
	void lcd_setlogfunction(lcd_log_callback log_callback);
#endif

/**
 * \brief Send a command to the display.
 * \param cmd The command to send (see \ref lcd_commands).
 */
void lcd_command(uint8_t cmd);

/**
 * \brief Set a text to the display.
 *
 * The text can be placed independently of allocated bitmaps
 * but has to be aligned to the eight pixel line grid in y direction.
 * Therefore the \p line Parameter is already in number of lines and not in pixels.
 * Also the text will overwrite anything that it is printed over.
 * To merge text into an image and use composition modes, draw it onto a bitmap.
 * \param x The start position of the text on the x-axis in pixels.
 * \param line The start position of the text on the y-axis in lines.
 * \param str The string to print.
 * \param inv Inverts the pixel states of the text before printing.
 */
void lcd_printtext(uint8_t x, uint8_t line, char* str, bool inv);

/**
 * \brief Calculate the size of a text.
 * \param str The string to measure.
 * \param width Pointer to the variable to be set by the function with the value of the pixel-with of the text.
 * \param lines Pointer to the variable to be set by the function with the value of the line count of the text.
 */
void lcd_textsize(char* str, uint16_t* width, uint16_t* lines);

/**
 * \brief Print a bitmap to the display.
 *
 * Like text, bitmaps can also only be placed in the eight bit line grid
 * and overwrite the existing content in the affected parts of the line.
 * Ideally the height of a bitmap is a multiple of eight, so no pixels are wasted.
 * \param x Starting point of the bitmap on the x-axis in pixels.
 * \param line Starting point of the bitmap on the y-axis in lines.
 * \param bmp The bitmap to print.
 */
void lcd_printbitmap(uint8_t x, uint8_t line, LCD_BITMAP* bmp);

/**
 * \brief Print a bitmap to the display with the information about the dirty region.
 *
 * Like text, bitmaps can also only be placed in the eight bit line grid
 * and overwrite the existing content in the affected parts of the line.
 * Ideally the height of a bitmap is a multiple of eight, so no pixels are wasted.
 * \param x Starting point of the bitmap on the x-axis in pixels.
 * \param line Starting point of the bitmap on the y-axis in lines.
 * \param bmp The bitmap to print.
 * \param g Graphics info structure to reprint only the dirty parts.
 */
void lcd_printgraphicbitmap(uint8_t x, uint8_t line, LCD_BITMAP* bmp, LCD_GRAPHICS* g);

/**
 * \brief Clear the bitmap contents (set all pixels off).
 */
void lcd_clear();

#endif /* LCD_H_ */
