// ******************************************************************
//
//    File: lcd_spi_ea.c
//    Date: 2017-01-27
// Version: 2.00
//
// Purpose:	Source data to control the EA DOGS102-6 lcd module	
// 			including init, write and drawing functions.
// 
// Feel free to modify this program for your specific purposes!
//
// V1:
// ------
// - Initial release
//
// V2: 
// ---
// - Modified method descriptions.
// - Fixed bug in "SPIDisplayString6x8XY" where text cannot be written past column 13.
// - Introduced boundary error checking in "SPIDisplayString6x8XY" and changed return type to BOOL.
// - "SPIDisplayDrawLine" now has error checking, returns BOOL (Success) and supports 1-pixel line.
// - Name "SPIDisplayFillInRectangle" makes more sense than "SPIDisplayRectangle".
// - "SPIDisplayFillInRectangle" returns BOOL (Success); more flexibility supplying coordinates.
// - Refactored and fixed bug in corner case where Rectangle Y-size is contained within 1 letter.
// - Harmonized use of Background color parameter.
//
// ******************************************************************

#include "lcd_spi_ea.h"

#define SPI_CLOCKRATE		SPI_CLOCKRATE_15_MHZ
#define SPI_BITORDER		SPI_FIRSTBIT_MSB

byte lcd_buffer[SIZE_X][SIZE_Y/8];	// Buffer to store display data. The buffer organization is equal to
											// the one of the display

// This is ASCII character set for font with 6 x 8 dots.
// The ASCII Table is from character 0x20 to 0x7E defined.
const byte FONT_6x8_Table[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,	// 0x20
	0x00,0x00,0x06,0x5F,0x06,0x00,	// 0x21
	0x00,0x07,0x03,0x00,0x07,0x03,	// 0x22
	0x00,0x24,0x7E,0x24,0x7E,0x24,	// 0x23
	0x00,0x24,0x2B,0x6A,0x12,0x00,	// 0x24
	0x00,0x63,0x13,0x08,0x64,0x63,	// 0x25
	0x00,0x36,0x49,0x56,0x20,0x50,	// 0x26
	0x00,0x00,0x07,0x03,0x00,0x00,	// 0x27
	0x00,0x00,0x3E,0x41,0x00,0x00,	// 0x28
	0x00,0x00,0x41,0x3E,0x00,0x00,	// 0x29
	0x00,0x08,0x3E,0x1C,0x3E,0x08,	// 0x2A
	0x00,0x08,0x08,0x3E,0x08,0x08,	// 0x2B
	0x00,0x00,0xE0,0x60,0x00,0x00,	// 0x2C
	0x00,0x08,0x08,0x08,0x08,0x08,	// 0x2D
	0x00,0x00,0x60,0x60,0x00,0x00,	// 0x2E
	0x00,0x20,0x10,0x08,0x04,0x02,	// 0x2F
	0x00,0x3E,0x51,0x49,0x45,0x3E,	// 0x30
	0x00,0x00,0x42,0x7F,0x40,0x00,	// 0x31
	0x00,0x62,0x51,0x49,0x49,0x46,	// 0x32
	0x00,0x22,0x49,0x49,0x49,0x36,	// 0x33
	0x00,0x18,0x14,0x12,0x7F,0x10,	// 0x34
	0x00,0x2F,0x49,0x49,0x49,0x31,	// 0x35
	0x00,0x3C,0x4A,0x49,0x49,0x30,	// 0x36
	0x00,0x01,0x71,0x09,0x05,0x03,	// 0x37
	0x00,0x36,0x49,0x49,0x49,0x36,	// 0x38
	0x00,0x06,0x49,0x49,0x29,0x1E,	// 0x39
	0x00,0x00,0x6C,0x6C,0x00,0x00,	// 0x3A
	0x00,0x00,0xEC,0x6C,0x00,0x00,	// 0x3B
	0x00,0x08,0x14,0x22,0x41,0x00,	// 0x3C
	0x00,0x24,0x24,0x24,0x24,0x24,	// 0x3D
	0x00,0x00,0x41,0x22,0x14,0x08,	// 0x3E
	0x00,0x02,0x01,0x59,0x09,0x06,	// 0x3F
	0x00,0x3E,0x41,0x5D,0x55,0x1E,	// 0x40
	0x00,0x7E,0x11,0x11,0x11,0x7E,	// 0x41
	0x00,0x7F,0x49,0x49,0x49,0x36,	// 0x42
	0x00,0x3E,0x41,0x41,0x41,0x22,	// 0x43
	0x00,0x7F,0x41,0x41,0x41,0x3E,	// 0x44
	0x00,0x7F,0x49,0x49,0x49,0x41,	// 0x45
	0x00,0x7F,0x09,0x09,0x09,0x01,	// 0x46
	0x00,0x3E,0x41,0x49,0x49,0x7A,	// 0x47
	0x00,0x7F,0x08,0x08,0x08,0x7F,	// 0x48
	0x00,0x00,0x41,0x7F,0x41,0x00,	// 0x49
	0x00,0x30,0x40,0x40,0x40,0x3F,	// 0x4A
	0x00,0x7F,0x08,0x14,0x22,0x41,	// 0x4B
	0x00,0x7F,0x40,0x40,0x40,0x40,	// 0x4C
	0x00,0x7F,0x02,0x04,0x02,0x7F,	// 0x4D
	0x00,0x7F,0x02,0x04,0x08,0x7F,	// 0x4E
	0x00,0x3E,0x41,0x41,0x41,0x3E,	// 0x4F
	0x00,0x7F,0x09,0x09,0x09,0x06,	// 0x50
	0x00,0x3E,0x41,0x51,0x21,0x5E,	// 0x51
	0x00,0x7F,0x09,0x09,0x19,0x66,	// 0x52
	0x00,0x26,0x49,0x49,0x49,0x32,	// 0x53
	0x00,0x01,0x01,0x7F,0x01,0x01,	// 0x54
	0x00,0x3F,0x40,0x40,0x40,0x3F,	// 0x55
	0x00,0x1F,0x20,0x40,0x20,0x1F,	// 0x56
	0x00,0x3F,0x40,0x3C,0x40,0x3F,	// 0x57
	0x00,0x63,0x14,0x08,0x14,0x63,	// 0x58
	0x00,0x07,0x08,0x70,0x08,0x07,	// 0x59
	0x00,0x71,0x49,0x45,0x43,0x00,	// 0x5A
	0x00,0x00,0x7F,0x41,0x41,0x00,	// 0x5B
	0x00,0x02,0x04,0x08,0x10,0x20,	// 0x5C
	0x00,0x00,0x41,0x41,0x7F,0x00,	// 0x5D
	0x00,0x04,0x02,0x01,0x02,0x04,	// 0x5E
	0x80,0x80,0x80,0x80,0x80,0x80,	// 0x5F
	0x00,0x00,0x03,0x07,0x00,0x00,	// 0x60
	0x00,0x20,0x54,0x54,0x54,0x78,	// 0x61
	0x00,0x7F,0x44,0x44,0x44,0x38,	// 0x62
	0x00,0x38,0x44,0x44,0x44,0x28,	// 0x63
	0x00,0x38,0x44,0x44,0x44,0x7F,	// 0x64
	0x00,0x38,0x54,0x54,0x54,0x08,	// 0x65
	0x00,0x08,0x7E,0x09,0x09,0x00,	// 0x66
	0x00,0x18,0xA4,0xA4,0xA4,0x7C,	// 0x67
	0x00,0x7F,0x04,0x04,0x78,0x00,	// 0x68
	0x00,0x00,0x00,0x7D,0x40,0x00,	// 0x69
	0x00,0x40,0x80,0x84,0x7D,0x00,	// 0x6A
	0x00,0x7F,0x10,0x28,0x44,0x00,	// 0x6B
	0x00,0x00,0x00,0x7F,0x40,0x00,	// 0x6C
	0x00,0x7C,0x04,0x18,0x04,0x78,	// 0x6D
	0x00,0x7C,0x04,0x04,0x78,0x00,	// 0x6E
	0x00,0x38,0x44,0x44,0x44,0x38,	// 0x6F
	0x00,0xFC,0x44,0x44,0x44,0x38,	// 0x70
	0x00,0x38,0x44,0x44,0x44,0xFC,	// 0x71
	0x00,0x44,0x78,0x44,0x04,0x08,	// 0x72
	0x00,0x08,0x54,0x54,0x54,0x20,	// 0x73
	0x00,0x04,0x3E,0x44,0x24,0x00,	// 0x74
	0x00,0x3C,0x40,0x20,0x7C,0x00,	// 0x75
	0x00,0x1C,0x20,0x40,0x20,0x1C,	// 0x76
	0x00,0x3C,0x60,0x30,0x60,0x3C,	// 0x77
	0x00,0x6C,0x10,0x10,0x6C,0x00,	// 0x78
	0x00,0x9C,0xA0,0x60,0x3C,0x00,	// 0x79
	0x00,0x64,0x54,0x54,0x4C,0x00,	// 0x7A
	0x00,0x08,0x3E,0x41,0x41,0x00,	// 0x7B
	0x00,0x00,0x00,0x77,0x00,0x00,	// 0x7C
	0x00,0x00,0x41,0x41,0x3E,0x08,	// 0x7D
	0x00,0x02,0x01,0x02,0x01,0x00,	// 0x7E
	0x00,0x3C,0x26,0x23,0x26,0x3C,	// 0x7F
};											

// Parameters to Initialize the SPI Port to LCD Display
const byte SPIDisplay_TXDataInit[13] =
{
	// Initialize bottom view 3.3V (booster on) 8Bit SPI
	0x40,	// Display start at line 0
	0xA1,	// SEG reverse
	0xC0,	// Normal COM0 - COM63
	0xA4,	// Disable => Set all pixel to ON
	0xA6,	// Display invers off
	0xA2,	// Set Bias 1/9 (Duty 1/65)
	0x2F,	// Booster, Regulator and Follower ON
	0x27,	// Set contrast
	0x81,
	0x07,
	0xFA,	// Set temperature compensatiobn curve to -0,11%/°C
	0x90,
	0xAF	// Display on
};
										
/// -----------------------------------------------------------------------------
/// @brief  Public: Initializes the SPI display EA DOG
/// @param  None
/// @retval None
/// -----------------------------------------------------------------------------
void SPIDisplayInit(void)
{
	// Init SPI for graphic display EA DOGS102-6
	const TSPIParameters Mode = { SPI_MODE_MASTER, SPI_CPOL_HIGH, SPI_CPHA_EDGE2, SPI_CLOCKRATE, SPI_BITORDER };
	SPIInit(&Mode); 
	
	GPIOClearBits(GPIO3);					// CD = Low = Command
	SPIMasterBeginTransfer();				// CS0 = Low = communication with display
    SPITransmit(SPIDisplay_TXDataInit, 13);	// write data to SPI
    SPIMasterEndTransfer();					// CS0 = High = disable communication with display
	GPIOSetBits(GPIO3);						// CD = High = waiting
}

/// -----------------------------------------------------------------------------
/// @brief  Public: Clear (fill) the entire SPI display EA DOGS102-6
/// @param  bg_color: 0=full clear, 1=full black
/// @retval None
/// -----------------------------------------------------------------------------
void SPIDisplayClear(bool bg_color)
{
	byte TXData[3], x, y, z;
	
	if(bg_color)
		z=0xFF;								// all pixel set
	else
		z=0x00;								// all pixel clear
	
	SPIMasterBeginTransfer();				// CS0 = Low = communication with display
	for(x=0; x<SIZE_X; x++) 
	{
		for(y=0; y<SIZE_Y/8; y++)
		{
			lcd_buffer[x][y]=z; 			// clear the buffer
			TXData[0] = x&0x0F;				// set SRAM LSB column address
			TXData[1] = 0x10+(x>>4);		// set SRAM MSB column address
			TXData[2] = 0xB0+y;				// set SRAM page address
			GPIOClearBits(GPIO3);			// CD = Low = Command
			SPITransmit(TXData, 3);			// write data to SPI
			GPIOSetBits(GPIO3);				// CD = High

			TXData[0] = z;					// set display buffer
			SPITransmit(TXData, 1);			// write data to SPI
		}
	}
	SPIMasterEndTransfer();					// CS0 = High = disable communication with display
}

/// -----------------------------------------------------------------------------
/// @brief  Public: Write text (character 6x8pt) to the display
/// @param  x coordinate (0 <= x <= 17-len)
/// @param  y coordinate (0 <= y <= 7)
/// @param  text to write
/// @param  len of the text (1-17)
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayString6x8XY(byte x, byte y, char text[], byte len, bool bg_color)
{
	bool success = true;
	byte i, j, p;
	const byte *font_char;
	byte ch;
	byte TXData[3];

	if((y>7) || (y<0))						// correctness of row
		success = false;
	if((len>17) || (len<=0))				// correctness of text length
		success = false;
	if((x+len>17) || (x<0))					// correctness of column
		success = false;
	
	// Do not proceed with printing if input parameters are invalid
	if (!success)
		return success;

	p=0;									// text index
	SPIMasterBeginTransfer();				// CS0 = Low = communication with display
	for(i=x*6; i<(x+len)*6; i+=6) {			// i = column value
		if(i>=SIZE_X) {
			success = false;
			return success;
		}
		ch = (byte)text[p++];  				// next character
		ch-=0x20;							// character table from space (0x20)
		if(ch>0x5F)
			ch=0x20;						// default: space
		font_char = &FONT_6x8_Table[ch*6];
		for(j=i; j<(i+6); j++) {
			TXData[0] = j&0x0F;				// set SRAM LSB column address
			TXData[1] = 0x10+(j>>4);		// set SRAM MSB column address
			TXData[2] = 0xB0+y;				// set SRAM page address
			GPIOClearBits(GPIO3);			// CD = Low = Command
			SPITransmit(TXData, 3);			// write data to SPI
			GPIOSetBits(GPIO3);				// CD = High = waiting
	
			if(bg_color)
				TXData[0] = 0xFF-*font_char;// font inverse
			else
				TXData[0] = *font_char;		// font
			SPITransmit(TXData, 1);			// write data to SPI
			font_char++;
		}
	}
	SPIMasterEndTransfer();					// CS0 = High = disable communication with display
	
	return success;
}

/// -----------------------------------------------------------------------------
/// @brief  Public: Draw a Horizontal or Vertical line
/// @param  xs X-Start Coordinate (0 <= x < 102)
/// @param  ys Y-Start Coordinate (0 <= x < 64)
/// @param  xe X-End Coordinate (0 <= x < 102)
/// @param  ye Y-End Coordinate (0 <= x < 64)
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayDrawLine(byte xs, byte ys, byte xe, byte ye, bool bg_color)
{
	bool success = true;
	byte TXData[3], a, b, i;
	
	// Error checking: boundaries
	if ((xs>=SIZE_X) || (ys>=SIZE_Y) || (xe>=SIZE_X) || (ye>=SIZE_Y))
		success = false;

	// Do not proceed with printing if input parameters are invalid
	if (!success)
		return success;
		
	if(xs==xe) {							// Vertical  line
		a=ys;
		b=ye;
		if(ys>ye) {							// swap
			a=ye;
			b=ys;
		}
		SPIMasterBeginTransfer();			// CS0 = Low = communication with display
		for(i=a; i<=b; i++) {
			TXData[0] = xs&0x0F;			// set SRAM LSB column address
			TXData[1] = 0x10+(xs>>4);		// set SRAM MSB column address
			TXData[2] = 0xB0+i/8;			// set SRAM page address
			GPIOClearBits(GPIO3);			// CD = Low = Command
			SPITransmit(TXData, 3);			// write data to SPI
			GPIOSetBits(GPIO3);				// CD = High = waiting

			if(bg_color)
				lcd_buffer[xs][i/8]&=(0xFF-(1<<(i%8)));	// clear the buffer
			else
				lcd_buffer[xs][i/8]|=(1<<(i%8));     	// set the buffer
			TXData[0] = lcd_buffer[xs][i/8];
			SPITransmit(TXData, 1);			// write data to SPI
		}
		SPIMasterEndTransfer();				// CS0 = High = disable communication with display
		
		return success;
	}
	if(ys==ye) {							// Horizontal line
		a=xs;
		b=xe;
		if(xs>xe) {
			a=xe;
			b=xs;
		}
		SPIMasterBeginTransfer();			// CS0 = Low = communication with display
		for(i=a; i<=b; i++) {
			TXData[0] = i&0x0F;				// set SRAM LSB column address
			TXData[1] = 0x10+(i>>4);		// set SRAM MSB column address
			TXData[2] = 0xB0+ys/8;			// set SRAM page address
			GPIOClearBits(GPIO3);			// CD = Low = Command
			SPITransmit(TXData, 3);
			GPIOSetBits(GPIO3);				// CD = High = waiting
			
			if(bg_color)
				lcd_buffer[i][ys/8]&=(0xFF-(1<<(ys%8)));	// clear the buffer
			else
				lcd_buffer[i][ys/8]|=(1<<(ys%8));     		// set the buffer
			TXData[0] = lcd_buffer[i][ys/8];
			SPITransmit(TXData, 1);
		}
		SPIMasterEndTransfer();				// CS0 = High = disable communication with display
		
		return success;
	}
	
	success = false;
	return success;
}

/// -----------------------------------------------------------------------------
/// @brief  Public: Fill in a rectangle
/// @param  xs X-Coordinate of Start Point (0 <= x < 102)
/// @param  ys Y-Coordinate of Start Point (0 <= x < 64)
/// @param  xe X-Coordinate of End Point (0 <= x < 102)
/// @param  ye Y-Coordinate of End Point (0 <= x < 64)
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayFillInRectangle(byte xs, byte ys, byte xe, byte ye, bool bg_color)
{
	bool success = true;
	byte tempCoordinate = 0, j=0, ps=0, pe=0, mask=0;
	
	// Sanity Checks: boundaries
	if ((xs<0) || (ys<0) || (xe<0) || (ye<0) ||
		(xs>=SIZE_X) || (ys>=SIZE_Y) || (xe>=SIZE_X) || (ye>=SIZE_Y))
		success = false;
	
	// Make sure Start Point has the smallest X and Y coordinates of the 2
	if (xe < xs) {
		tempCoordinate = xs;
		xs = xe;
		xe = tempCoordinate;
	}
	if (ye < ys) {
		tempCoordinate = ys;
		ys = ye;
		ye = tempCoordinate;
	}
	
	// Do not proceed with printing if input parameters are invalid
	if (!success)
		return success;
	
	ps=ys/8; 								// startpage
	pe=ye/8; 								// endpage
	
	SPIMasterBeginTransfer();				// CS0 = Low = communication with display

	// Code optimized for blocks contained within 1 letter box (Y-direction only)
	if (pe == ps) {

		mask = 0xFF>>(8-ys%8) | 0xFF<<(ye%8+1);	// Mask protecting header/footer
		if (success) success &= SPIDisplayClearPageLCD(xs, xe, ps, mask, bg_color);
	}
	
	// The following code is optimized for blocks spanning more than 1 letter box
	else {
				
		//-------------Clear part of startpage--------------
		mask = 0xFF>>(8-ys%8);		// Mask protecting header
		if (success) success &= SPIDisplayClearPageLCD(xs, xe, ps, mask, bg_color);

		//-------------Clear part of endpage----------------
		mask = 0xFF<<(ye%8+1);		// Mask protecting footer
		if (success) success &= SPIDisplayClearPageLCD(xs, xe, pe, mask, bg_color);

		//-------------Clear middle pages-------------------
		mask = 0x00;				// set / clear full page
		for(j=ps+1; j<pe; j++) 		// loop starting first middle page to last middle page
		{
			if (success) success &= SPIDisplayClearPageLCD(xs, xe, j, mask, bg_color);
		}		
	}

	SPIMasterEndTransfer();					// CS0 = High = disable communication with display
	
	return success;
}

/// -----------------------------------------------------------------------------
/// @brief  Private Helper: Clear Y-Page (using mask) between 2 X-coordinates
/// @param  xs X-Coordinate of Start Point (0 <= x < 102)
/// @param  xe X-Coordinate of End Point (0 <= x < 102)
/// @param  page Index of Y-Page (0<= Page <= 7)
/// @param  mask Active-high bit-mask protecting part of the page
/// @param  bg_color background color (0:clear, 1:black)
/// @retval success of operation
/// -----------------------------------------------------------------------------
bool SPIDisplayClearPageLCD(byte xs, byte xe, byte page, byte mask, bool bg_color)
{
	byte TXData[3], i;
	
	TXData[0] = xs&0x0F;					// set SRAM LSB column address
	TXData[1] = 0x10+(xs>>4);				// set SRAM MSB column address
	TXData[2] = 0xB0+page;					// set SRAM page address
	GPIOClearBits(GPIO3);					// CD = Low = Command
	SPITransmit(TXData, 3);
	GPIOSetBits(GPIO3);						// CD = High = waiting

	for(i=xs; i<=xe; i++) 					// loop starting first colomn to last coloumn
	{
		if(bg_color)
			lcd_buffer[i][page]&=mask; 		//clear the buffer
		else
			lcd_buffer[i][page]|=~mask; 	//clear the buffer
		TXData[0] = lcd_buffer[i][page];	// send the changed pages of the buffer to the display
		SPITransmit(TXData, 1);
	}

	return true;
}


//----------------- EOF lcd_spi_ea.c ------------
