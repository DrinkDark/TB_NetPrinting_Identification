// ******************************************************************
//
//    File: App_GIOD100_Interrupt_Demo.c
//    Date: 2020-08-07
// Version: 1.00
//
// Purpose:
//
// Demonstrate usage of the app interrupt system.
//
// Functions:
//
// - Demonstrate usage of GPIO interrupts for a Wiegand receiver
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

int Timeout = 0;

void SystickHandler(void)
{
	if (Timeout == 0)
		return;
	Timeout--;
	if (Timeout > 0)
		return;
	WriteChar(CHANNEL_USB,'\r');
	WriteChar(CHANNEL_USB,'\n');
}
void Data0Handler(void)
{
	WriteChar(CHANNEL_USB,'0');
	// Restart timeout
	Timeout = 10;
}
void Data1Handler(void)
{
	WriteChar(CHANNEL_USB,'1');
	// Restart timeout
	Timeout = 10;
}

int main(void)
{
    SetVolume(5);
    BeepLow();
    BeepHigh();
    SetVolume(100);

	// Red and green LED at the same time are Wiegand output
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);

	// For testing purposes, create a Wiegand transmitter @ GPIO0/GPIO1
	GPIOConfigureOutputs(GPIO0 | GPIO1,GPIO_PUPD_NOPULL,GPIO_OTYPE_PUSHPULL);
	// Enter idle level
	GPIOClearBits(GPIO0 | GPIO1);

	// Initialize the Wiegand receiver @ GPIO2/GPIO3
	// Prepare inputs
	GPIOConfigureInputs(GPIO2 | GPIO3,GPIO_PUPD_PULLDOWN);
	// Interrupt on rising edge of (positive) Wiegand pulses
    GPIOConfigureInterrupt(GPIO2 | GPIO3,true,TRIGGER_RISING);
    // Hook on required interrupts
	SetInterruptHandler(SystickHandler,INTNO_SYSTICK);
	SetInterruptHandler(Data0Handler,INTNO_GPIO2_TRIGGERED);
	SetInterruptHandler(Data1Handler,INTNO_GPIO3_TRIGGERED);

	while (true)
	{
		// Any character from terminal program triggers a Wiegand cycle
		ReadByte(CHANNEL_USB);
		
		// Send some test bits
		const byte Bits[] = { 0xAA,0x55,0x0F,0xF0 };
		SendWiegand(GPIO0,GPIO1,10,100,Bits,sizeof(Bits)*8);
	}
}
