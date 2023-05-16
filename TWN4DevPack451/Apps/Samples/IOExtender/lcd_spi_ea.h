// ******************************************************************
//
//    File: lcd_spi_ea.h
//    Date: 2017-01-27
// Version: 2.00
//
// ******************************************************************
#ifndef _LCD_SPI_EA_H
#define _LCD_SPI_EA_H

//----------------- Definitions -----------
#define SIZE_X 102
#define SIZE_Y 64

//-------------- Public Functions ---------

/// -----------------------------------------------------------------------------
/// @brief  Public: Initializes the SPI display EA DOG
/// @param  None
/// @retval None
/// -----------------------------------------------------------------------------
void SPIDisplayInit (void);

/// -----------------------------------------------------------------------------
/// @brief  Public: Clear (fill) the entire SPI display EA DOGS102-6
/// @param  bg_color: 0=full clear, 1=full black
/// @retval None
/// -----------------------------------------------------------------------------
void SPIDisplayClear (bool bg_color);

/// -----------------------------------------------------------------------------
/// @brief  Public: Write text (character 6x8pt) to the display
/// @param  x coordinate (0 <= x <= 17-len)
/// @param  y coordinate (0 <= y <= 7)
/// @param  text to write
/// @param  len of the text (1-17)
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayString6x8XY (byte x, byte y, char text[], byte len, bool bg_color);

/// -----------------------------------------------------------------------------
/// @brief  Public: Draw a Horizontal or Vertical line
/// @param  xs X-Start Coordinate (0 <= x < 102)
/// @param  ys Y-Start Coordinate (0 <= x < 64)
/// @param  xe X-End Coordinate (0 <= x < 102)
/// @param  ye Y-End Coordinate (0 <= x < 64)
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayDrawLine (byte xs, byte ys, byte xe, byte ye, bool bg_color);

/// -----------------------------------------------------------------------------
/// @brief  Public: Fill in a rectangle
/// @param  xs X-Coordinate of Start Point (0 <= x < 102)
/// @param  ys Y-Coordinate of Start Point (0 <= x < 64)
/// @param  xe X-Coordinate of End Point (0 <= x < 102)
/// @param  ye Y-Coordinate of End Point (0 <= x < 64)
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayFillInRectangle (byte xs, byte ys, byte xe, byte ye, bool bg_color);

//------------- Private Functions ---------

/// -----------------------------------------------------------------------------
/// @brief  Private Helper: Clear Y-Page (using mask) between 2 X-coordinates
/// @param  xs X-Coordinate of Start Point (0 <= x < 102)
/// @param  xe X-Coordinate of End Point (0 <= x < 102)
/// @param  page Index of Y-Page (0<= Page <= 7)
/// @param  mask Active-high bit-mask protecting part of the page
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayClearPageLCD (byte xs, byte xe, byte page, byte mask, bool bg_color);

#endif
