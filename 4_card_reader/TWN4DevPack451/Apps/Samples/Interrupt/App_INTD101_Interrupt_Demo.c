// ******************************************************************
//
//    File: App_INTD101_Interrupt_Demo.c
//    Date: 2020-04-07
// Version: 1.01
//
// Purpose:
//
// Demonstrate usage of the app interrupt system.
//
// Functions:
//
// - Search for the transponders in the foreground.
// - Echo character incoming on the test channel back to the sender.
// - Trigger a delayed beep in the foreground process, once character
//   was transmitted.
//
// Feel free to modify this program for your specific purposes!
//
// V1.00:
// ------
// - Initial release
//
// V1.01:
// ------
// - Replace signalling via LEDs by some character output
//
// ******************************************************************

#include "twn4.sys.h"
#include "apptools.h"

// Choose test channel CHANNEL_USB, CHANNEL_COM1, CHANNEL_COM2

#define TEST_CHANNEL				CHANNEL_USB

#if TEST_CHANNEL == CHANNEL_USB
  #define INTNO_BYTES_TRANSMITTED	INTNO_USB_BYTES_TRANSMITTED
  #define INTNO_BYTES_RECEIVED		INTNO_USB_BYTES_RECEIVED
#endif
#if TEST_CHANNEL == CHANNEL_COM1
  #define INTNO_BYTES_TRANSMITTED	INTNO_COM1_BYTE_TRANSMITTED
  #define INTNO_BYTES_RECEIVED		INTNO_COM1_BYTE_RECEIVED
#endif
#if TEST_CHANNEL == CHANNEL_COM2
  #define INTNO_BYTES_TRANSMITTED	INTNO_COM2_BYTE_TRANSMITTED
  #define INTNO_BYTES_RECEIVED		INTNO_COM2_BYTE_RECEIVED
#endif

int Counter = 0;
bool TriggerBeep = false;

void SystickHandler(void)
{
	if (Counter > 0)
		Counter--;
	if (Counter != 1)
		return;
	TriggerBeep = true;
}
void BytesTransmittedHandler(void)
{
	Counter = 2000;
}
void BytesReceivedHandler(void)
{
	// Send all received characters back to sender ("echo")
	while (TestChar(TEST_CHANNEL))
	{
		char Char = ReadChar(TEST_CHANNEL);
		WriteChar(TEST_CHANNEL,Char);
	}
}

int main(void)
{
	// Signal startup
    SetVolume(30);
    BeepLow();
    BeepHigh();
    SetVolume(100);

	// Initialize LEDs
    LEDInit(REDLED | GREENLED | YELLOWLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    // Install interrupt handlers
	SetInterruptHandler(SystickHandler,INTNO_SYSTICK);
	SetInterruptHandler(BytesTransmittedHandler,INTNO_BYTES_TRANSMITTED);
	SetInterruptHandler(BytesReceivedHandler,INTNO_BYTES_RECEIVED);
	while (true)
	{
		// Do some nice and blocking foreground action:
		// Search transponders continously
		int TagType,IDBitCount;
		byte ID[32];
		if (SearchTag(&TagType,&IDBitCount,ID,sizeof(ID)))
			BeepHigh();
		if (TriggerBeep)
		{
			BeepLow();
			TriggerBeep = false;
		}
	}
}
