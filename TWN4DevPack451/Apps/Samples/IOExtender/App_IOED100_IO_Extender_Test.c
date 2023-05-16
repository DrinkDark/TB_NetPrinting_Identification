// ******************************************************************
//
//    File: App_IOED100_IO_Extender_Test
//    Date: 2017-01-27
// Version: 1.00
//
// Purpose:
//
// This App is demonstrates the use of I/O Extender Ports as well as
// the LCD Display. IDs of cards can be read and displayed on LCD.
// These IDs can be sent off the board via one of the extender ports
// available. Un-comment the relevant defines in
// "Peripheral Port Switches" section below.
// 
// Feel free to modify this program for your specific purposes!
//
// V1.00:
// ------
// - Initial release
//
// ******************************************************************

//------------------------- Include Section -------------------------
#include "twn4.sys.h"
#include "apptools.h"
#include "lcd_spi_ea.c"
#include "prs.h"


//--------------------- Peripheral Port Switches --------------------
// Uncomment ports used

//#define WIEGAND
#define OMRON
//#define RS485_RS422
#define I2C_SE95D		// Temperature Sensor
#define LCD_SPI_EA		// LCD Display over SPI


//----------------------- Defines and Settings ----------------------

// Define behaviour of App: Startup App independent of
// enumeration by USB
const unsigned char AppManifest[] =
{
    EXECUTE_APP, 1, EXECUTE_APP_ALWAYS,
    TLV_END
};

#define LFTAGTYPES        		(ALL_LFTAGS & ~(TAGMASK(LFTAG_TIRIS) | TAGMASK(LFTAG_COTAG)))
#define HFTAGTYPES        		(ALL_HFTAGS & ~(TAGMASK(HFTAG_NFCP2P)))
#define CARDTIMEOUT				2000UL	// Timeout in milliseconds
#define MAXCARDIDLEN            32		// Length in bytes
#define MAXCARDSTRINGLEN		128   	// Length W/O null-termination

// ******************************************************************
// ****** Main Program Loop *****************************************
// ******************************************************************

int main(void)
{
	// Init variables for RFID Card reading and processing
	int TagType;
	int IDBitCnt;
	byte ID[MAXCARDIDLEN];
	char CardString[MAXCARDSTRINGLEN+1], TmpString[2];
	SetTagTypes(LFTAGTYPES,HFTAGTYPES);
	char OldCardString[MAXCARDSTRINGLEN+1]; 
    OldCardString[0] = 0;
	
	bool systemReady = true;
	bool LCD_background_color = 0;
	
	// On Start-up emit sounds and light LEDs
    SetVolume(30);
    BeepLow();
    BeepHigh();

    LEDInit(REDLED | GREENLED | YELLOWLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);

	// Use GPIO3 | GPIO4, GPIO5 and GPIO6 as output
	GPIOConfigureOutputs(GPIO3 | GPIO4 | GPIO5 | GPIO6, GPIO_PUPD_NOPULL, GPIO_OTYPE_PUSHPULL);
	GPIOClearBits(GPIO3 | GPIO4 | GPIO5 | GPIO6);
	GPIOSetBits(GPIO3 | GPIO4 | GPIO5 | GPIO6);
	
	// Use GPIO7 as input
	GPIOConfigureInputs(GPIO7, GPIO_PUPD_PULLUP);
	
	// Initialize COM2 parameters for RS485 and RS422
	TCOMParameters COMParameters;
	COMParameters.BaudRate=32768;
	COMParameters.WordLength=COM_WORDLENGTH_8;
    COMParameters.Parity=COM_PARITY_NONE;
    COMParameters.StopBits=COM_STOPBITS_1;
    COMParameters.FlowControl=COM_STOPBITS_1;
	SetCOMParameters(CHANNEL_COM2, &COMParameters);
	
#ifdef I2C_SE95D
	// Initialize slave SE95D temperature sensor
	const int I2CAddress = 0x48;
	byte I2C_Temp[3];
	I2CInit(I2CMODE_MASTER);	
	I2CMasterStart();
	I2CMasterBeginWrite(I2CAddress | 0x01);	// write
	I2CMasterTransmitByte(0x80);			// pointer byte
	I2CMasterTransmitByte(0x40);			// 1 conversion / sec
	I2CMasterStop();
#endif

#ifdef LCD_SPI_EA
	// Init SPI for graphic display EA DOGS102-6
	Sleep(1000, SNEP_STATE_SLEEP);			// Wait for display ready (power on)
	SPIDisplayInit();
	SPIDisplayClear(LCD_background_color);	// Clear Display
	if (systemReady) systemReady &= SPIDisplayString6x8XY(1, 0, "IO SHIELD V1.00", 15, LCD_background_color);
	if (systemReady) systemReady &= SPIDisplayDrawLine(5, 9, 96, 9, LCD_background_color);
	if (systemReady) systemReady &= SPIDisplayDrawLine(5, 11, 96, 11, LCD_background_color);
	if (systemReady) systemReady &= SPIDisplayFillInRectangle(0,22,SIZE_X-1,40,LCD_background_color);

	if (systemReady) systemReady &= SPIDisplayString6x8XY(0, 3, "ID:", 3, !LCD_background_color);
	if (systemReady) systemReady &= SPIDisplayString6x8XY(0, 4, "Bit:", 4, !LCD_background_color);
	if (systemReady) systemReady &= SPIDisplayString6x8XY(2, 7, "TEMP:", 5, LCD_background_color);
#endif

	// If Initialization comlete without errors, enter infinite loop to read cards
    while (systemReady)
    {
#ifdef I2C_SE95D
		// Update room temperature information from I2C-accessible sensor
		// Receive two bytes from the slave
		I2CMasterStart();
		I2CMasterBeginRead(I2CAddress | 0x00);	// read
		// All bytes except last byte require an ACK to be sent
		I2CMasterSetAck(ON);
		I2C_Temp[0] = I2CMasterReceiveByte();
		// Turn off ACK before reading last byte
		I2CMasterSetAck(OFF);
		I2C_Temp[1] = I2CMasterReceiveByte();
		I2CMasterStop();

	#ifdef LCD_SPI_EA
		// If using LCD Display, update temperature there
		int i = I2C_Temp[0];
		CardString[0] = ((i/100)>0)?((i/100)+'0'):' ';
		if(I2C_Temp[0]&0x80) {			// negative?
			i -= 255;					// 2 compliment
			CardString[0] = '-';
		}
		CardString[1] = ((i/10)%10)+'0';
		CardString[2] = (i%10)+'0';
		CardString[3] = '.';
		i = ((I2C_Temp[1]&0x80)>>7);
		if(I2C_Temp[0]&0x80) {			// negative?
			i *= 1;						// Bit inverted
		}
		CardString[4] = (i==0)?'0':'5';
		CardString[5] = ' ';
		CardString[6] = 'C';
		SPIDisplayString6x8XY(7, 7, CardString, 7, LCD_background_color);
	#endif
#endif
		
		// If Card detected, store its ID
	    if (SearchTag(&TagType,&IDBitCnt,ID,MAXCARDIDLEN))
	    {
		  	// Convert ID to ASCII
		    ConvertBinaryToString(ID,0,IDBitCnt,CardString,16,(IDBitCnt+7)/8*2,MAXCARDSTRINGLEN);
			
			// Check if new Card ID is the same as old Card ID
    		if (strcmp(CardString,OldCardString) != 0)
    		{
				// New Card ID found - send it to the Host
    			strcpy(OldCardString,CardString);
				// Send card string including prefix (actually no prefix) and suffix ("\r")
			    HostWriteString(CardString);
			    HostWriteString("\r");
			    
				// Acknowledge with sounds and LEDs that a new card was processed
			    LEDOff(GREENLED);
			    LEDOn(REDLED);
			    LEDBlink(REDLED,500,500);
			
			    SetVolume(100);
			    BeepHigh();
				
#ifdef LCD_SPI_EA
				// Update Card ID on LCD Display
				SPIDisplayString6x8XY(5, 3, "            ", 12, !LCD_background_color);
				SPIDisplayString6x8XY(5, 3, CardString, IDBitCnt/4, !LCD_background_color);
				TmpString[0] = ((IDBitCnt/10)%10)+'0';
				TmpString[1] = (IDBitCnt%10)+'0';
				SPIDisplayString6x8XY(5, 4, TmpString, 2, !LCD_background_color);
#endif

#ifdef WIEGAND
				// Send data over Wiegand:
				// Use GPIO5 and GPIO6 for Wiegand interface
				GPIOConfigureOutputs(GPIO5 | GPIO6, GPIO_PUPD_NOPULL, GPIO_OTYPE_PUSHPULL);
				// Enter idle level. In this case we have active low outputs
				GPIOSetBits(GPIO5 | GPIO6);
				// Now send the bits
				SendWiegand(GPIO5,GPIO6,100,1000,ID,IDBitCnt);					
#endif
				
#if defined(OMRON)
				// Send data over Omron:
				// Use GPIO6 and GPIO5 for the clock/data interface, GPIO4 for cir(card in reader)
				GPIOConfigureOutputs(GPIO6 | GPIO5 | GPIO4, GPIO_PUPD_NOPULL, GPIO_OTYPE_PUSHPULL);
				// Enter idle level. In this case we have active low outputs
				GPIOSetBits(GPIO4 | GPIO6 | GPIO5);
				// Now send the bits
				SendOmron(GPIO6,GPIO5,500,1000,500,ID,IDBitCnt);
				// card is removed
				GPIOClearBits(GPIO4);
#endif		
				
#if defined(RS485_RS422)
				// Send data over RS485 / RS422
				GPIOClearBits(GPIO4);
				// Send card ID to serial line
				for(i=0; i<(IDBitCnt/8); i++)
					WriteByte(CHANNEL_COM2, ID[i]);
				WriteByte(CHANNEL_COM2, '\r');
				// enter read level
				GPIOSetBits(GPIO4);
#endif
    		}
			
			// Start de-bounce timer to prevent multiple processing iterations on the same card
           	StartTimer(CARDTIMEOUT);
	    }
		
		// Check if de-bounce timer done
        if (TestTimer())
        {
			// Card ID de-bounce timer done. The old Card ID can be processed again.
		    OldCardString[0] = 0;
		    LEDOn(GREENLED);
		    LEDOff(REDLED);
        }
    }
}
