/*
 * region.h
 *
 *  Created on: 18.10.2017
 *      Author: fabiangeissler
 */

#ifndef BITMAP_H_
#define BITMAP_H_

#include "stdint.h"
#include "stdbool.h"
#include "settings.h"

/// \brief Bitmap data structure.
///
/// A bitmap contains graphical data. Each bit in the data field represents one pixel.
/// This structure contains information aboout the bitmap, such as width and height in pixels,
/// the byte-size of the data array and the data array itself.
typedef struct {
	uint8_t width; 	///< Width of the bitmap in pixels.
	uint8_t height; 	///< Height of the bitmap in pixels.
	uint16_t size;	///< Size of the data array in bytes.

	/// \brief Bytes per pixel column.
	///
	/// Number of bytes to store all the pixels of one bitmap column (y-direction).
	uint8_t bpc;

	/// \brief The bitmap pixel data storage array.
	///
	/// Each byte represents eight pixels in y-direction.
	/// LSB is lower y and MSB is higher y value.
	/// Successive bytes follow each other in x-direction.
	/// That way <width> bytes represent an area of <width> x 8 pixels.
	/// If (<height> mod 8) does not equal zero the upper bits of the
	/// last byte row are padded, but value is irrelevant.
	uint8_t* data;
} LCD_BITMAP;

/**
 * \brief Get the amount of memory used by all allocated bitmaps.
 * \return The amount of memory used by all allocated bitmaps in bytes.
 */
uint16_t lcd_getbitmapmemusage();

/**
 * \brief Set the maximum amount of memory used by all allocated bitmaps.
 * \return When more memory than \p limit is currently allocated
 * the return value is false and the new limit is not set.
 * \param limit The memory limit in bytes.
 */
bool lcd_settotalbitmapmemlimit(uint16_t limit);

/**
 * \brief Allocate memory for a bitmap.
 * \return If the memory used when trying to create this bitmap is
 * higher than the set limit it returns false and will not be allocated.
 * \param bmp Pointer to uninitialized \ref LCD_BITMAP structure to initialize.
 * \param width Bitmap width in pixels.
 * \param height Bitmap height in pixels.
 */
bool lcd_bitmapalloc(LCD_BITMAP* bmp, uint8_t width, uint8_t height);

/**
 * \brief Free memory of a bitmap.
 * \param bmp Pointer to initialized \ref LCD_BITMAP structure to free.
 */
void lcd_bitmapfree(LCD_BITMAP* bmp);

/**
 * \brief Clear the contents of a bitmap.
 * \param bmp Pointer to initialized \ref LCD_BITMAP structure.
 * \param inv If true, set all pixels on, else set all pixels off.
 */
void lcd_bitmapclear(LCD_BITMAP* bmp, bool inv);

#endif /* BITMAP_H_ */
