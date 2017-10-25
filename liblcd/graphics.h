/*
 * graphics.h
 *
 *  Created on: 18.10.2017
 *      Author: fabiangeissler
 */

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "stdint.h"
#include "stdbool.h"

#include "bitmap.h"
#include "settings.h"

/**
 * \brief Composition mode for drawings onto a bitmap graphics.
 */
enum lcd_graphicsmode {
	LCD_SETPOINT = 0b01, 	///< Set the pixels on
	LCD_CLEARPOINT = 0b10,	///< Set the pixels off
	LCD_INVERTPOINT = LCD_SETPOINT | LCD_CLEARPOINT, ///< invert the pixels
	LCD_THICKPOINT = 0b100	///< Draw thick pixels
};

/**
 * \brief Graphics info structure.
 *
 * This structure stores information about the graphics operations.
 */
typedef struct {
	uint8_t graphicsmode; 	///< Composition mode
	bool dirty;				///< Bitmap needs to be reprinted
	uint8_t dirty_x0;		///< Lower corner x of dirty rect
	uint8_t dirty_y0;		///< Lower corner y of dirty rect
	uint8_t dirty_x1;		///< Upper corner x of dirty rect
	uint8_t dirty_y1;		///< Upper corner y of dirty rect
} LCD_GRAPHICS;

/**
 * \brief Draw a point to a bitmap canvas.
 * \param canvas Pointer to initialized \ref LCD_BITMAP structure of the canvas to draw on.
 * \param x Position of the point in x-direction in pixels.
 * \param y Position of the point in y-direction in pixels.
 * \param mode Composition mode of the drawing operation.
 */
void lcd_drawpoint(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x, int y);

/**
 * \brief Draw a line to a bitmap canvas.
 *
 * Implementation taken from wikipedia (https://de.wikipedia.org/wiki/Bresenham-Algorithmus).
 * \param canvas Pointer to initialized \ref LCD_BITMAP structure of the canvas to draw on.
 * \param x0 Start point on x-axis.
 * \param y0 Start point on y-axis.
 * \param x1 End point on x-axis.
 * \param y2 End point on y-axis.
 * \param mode Composition mode of the drawing operation.
 */
void lcd_drawline(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x0, int y0, int x1, int y1);

/**
 * \brief Draw a circle to a bitmap canvas.
 *
 * Implementation taken from wikipedia (https://de.wikipedia.org/wiki/Bresenham-Algorithmus).
 * \param canvas Pointer to initialized \ref LCD_BITMAP structure of the canvas to draw on.
 * \param x0 Center point on x-axis.
 * \param y0 Center point on y-axis.
 * \param radius Circle radius in pixels.
 * \param mode Composition mode of the drawing operation.
 */
void lcd_drawcircle(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x0, int y0, int radius);

/**
 * \brief Draw a filled rectangle to a bitmap canvas.
 * \param canvas Pointer to initialized \ref LCD_BITMAP structure of the canvas to draw on.
 * \param x0 Lower value x-axis point.
 * \param y0 Lower value y-axis point.
 * \param x1 Higher value x-axis point.
 * \param y2 Higher value y-axis point.
 * \param mode Composition mode of the drawing operation.
 */
void lcd_fillrect(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x0, int y0, int x1, int y1);

//void lcd_drawbitmap(LCD_BITMAP* canvas, int x, int y, LCD_BITMAP* source, uint8_t mode = LCD_SETPOINT);

/**
 * \brief Draw text to a bitmap canvas.
 * \param canvas Pointer to initialized \ref LCD_BITMAP structure of the canvas to draw on.
 * \param x Starting point of the text on the x-axis.
 * \param y Starting point of the text on the y-axis.
 * \param txt The string to draw.
 * \param mode Composition mode of the drawing operation.
 */
void lcd_drawtext(LCD_BITMAP* canvas, LCD_GRAPHICS* mode, int x, int y, char* txt);

#endif /* GRAPHICS_H_ */
