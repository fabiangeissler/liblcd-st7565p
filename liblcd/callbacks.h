/*
 * delegates.h
 *
 *  Created on: 18.10.2017
 *      Author: fabiangeissler
 */

#ifndef CALLBACKS_H_
#define CALLBACKS_H_

#include "stdint.h"
#include "settings.h"

/// \brief Callback funtion pointer type for a delay function with integer time parameter in milliseconds.
typedef void (*lcd_delay_callback)(int ms);

/// \brief Callback funtion pointer type for a pin set function with parameter for the new pin state (0 = off, 1 = on).
typedef void (*lcd_pin_callback)(uint8_t state);

/// \brief Callback funtion pointer type for a spi transceive function with parameter for the data to send. Returns the received data.
typedef uint8_t (*lcd_spi_callback)(uint8_t data);

#ifdef LCD_LOGGING
	/// \brief Callback funtion pointer type for a logging function with parameter for the string to log.
	typedef void (*lcd_log_callback)(char* str);
#endif

#endif /* CALLBACKS_H_ */
