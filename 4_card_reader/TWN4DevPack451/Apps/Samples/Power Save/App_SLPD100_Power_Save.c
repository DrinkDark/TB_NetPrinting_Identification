// ******************************************************************
//
//    File: App_SLPD100_Power_Save.c
//    Date: 2017-07-07
// Version: 1.00
//
// Purpose:
//
// This App demonstrates operation of system function Sleep and
// how minimum power consumption can be achieved.
//
// The App is using COM1 for host communication. The stop mode
// of sleep requires USB to be turned off.
//
// Feel free to modify this program for your specific purposes!
//
// V1.00:
// ------
// - Initial release
//
// ******************************************************************

#include "twn4.sys.h"
#include "apptools.h"

// Threshold specifies reader sensitivity in Sleep Mode, wrt. card detection.
// Modify this value between 0x5 and 0xFF depending on the application.
// Note: the higher the threshold, the less sensitive the reader is to disturbances
//   (chance of false positives therefore lower). However, if the threshold is too high,
//   may experience problems detecting cards.
//    0: sleep mode essentially non-existent (reader extremely sensitive)
//   16: medium frequency of false positives (once every few minutes)
//       given no significant metal objects in the vicinity (typical reader sensitivity)
#define PN5180_LPCD_THRESHOLD_VALUE 		16

// Time period between 2 LPCD attempts in milliseconds.
// Increasing this time period will reduce power consumption in sleep mode, but may result
// in slower response to card detection.
#define PN5180_LPCD_SENSING_PERIOD_VALUE 	200

const unsigned char AppManifest[] =
{
	OPEN_PORTS, 1, OPEN_PORT_COM1_MSK,
	PN5180_LPCD_THRESHOLD, 1, PN5180_LPCD_THRESHOLD_VALUE,
    PN5180_LPCD_SENSING_PERIOD, 2, PN5180_LPCD_SENSING_PERIOD_VALUE & 0xFF, PN5180_LPCD_SENSING_PERIOD_VALUE >> 8,
	TLV_END
};

// ******************************************************************
// ****** Main Program Loop *****************************************
// ******************************************************************

// Set this value to 0 in order to achieve even lower current consumption at the cost
// of no signalling of LEDs anymore.
#define USELEDS		1

int main(void)
{
#if USELEDS	
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
#endif    
    
    SetVolume(30);
    BeepLow();
    BeepHigh();

    while (true)
    {
    	// Got to sleep for an infinite time period. Device
    	// can be woken up by presence of a transponder (LPCD) or metallic object only.
	    Sleep(0, WAKEUP_BY_LPCD_MSK | SLEEPMODE_STOP);
	    
	    // An object was detected, now try to find a transponder
		int TagType;
		int IDBitCnt;
		byte ID[32];
	    if (SearchTag(&TagType,&IDBitCnt,ID,sizeof(ID)))
	    {
		  	// Convert ID to ASCII
			char CardString[128+1]; 
		    ConvertBinaryToString(ID,0,IDBitCnt,CardString,16,(IDBitCnt+7)/8*2,sizeof(CardString)-1);
			
			// Send card string to host
		    HostWriteString(CardString);
		    HostWriteString("\r");
		
			// Signal presence of a card to the user. Current consumption
			// could be lowered even more by turning off LEDs

		    SetVolume(100);
		    BeepHigh();
#if USELEDS	
		    LEDOff(GREENLED);
		    LEDOn(REDLED);
		    LEDBlink(REDLED,500,500);
            
            // Use sleep mode in order to keep LEDs blinking. 
            // Sleep for 2 seconds, no LPCD to be activated here
		    Sleep(2000, SLEEPMODE_SLEEP);

		    LEDOn(GREENLED);
		    LEDOff(REDLED);
#endif		    
        }
    }
}
