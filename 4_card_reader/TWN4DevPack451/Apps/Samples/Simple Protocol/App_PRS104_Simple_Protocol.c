// ******************************************************************
//
// Purpose:
//
// App for TWN4, which implements the "Simple Protocol".
//
// V1:
// ------
// - Initial release
//
// V1.01:
// ------
// - Changed version numbering scheme
// - Split App into several modules
//
// V1.02:
// ------
// - Abstraction layer for support of binary modes
//
// V1.03:
// ------
// - Support optional use of CRC
// - Renamed functions for simple protocol
// - Split protocol handler into TestCommand/ExecuteCommand/SendResponse
// - External access to received messages
//
// V1.04:
// ------
// - System functions as provided by firmware V1.64
//
// ******************************************************************
//
//      Reference messages for function GetUSBType
//
//      -------------------------------------------------------------------------------
//      Mode      CRC     Command (Host -> TWN4)          Response (TWN4 -> Host)
//      -------------------------------------------------------------------------------
//      ASCII     Off     "0005\r"                        "0001\r"
//      ASCII     On      "000515A7\r"                    "000131E1\r"
//      Binary    Off     0x02 0x00 0x00 0x05             0x02 0x00 0x00 0x01
//      Binary    On      0x04 0x00 0x00 0x05 0x15 0xA7   0x04 0x00 0x00 0x01 0x31 0xE1
//
//      Reference implementations of CRC algorithm
//      -------------------------------------------------------------------------------
//      Language C
//      ----------
//
//  	uint16_t UpdateCRC(uint16_t CRC,byte Byte)
//  	{
//      	// Update CCITT CRC (reverse polynom 0x8408)
//      	Byte ^= (byte)CRC;
//      	Byte ^= (byte)(Byte << 4);
//      	return (uint16_t)(((Byte << 8) | (CRC >> 8)) ^ (Byte >> 4) ^ (Byte << 3));
//  	}
//
//      Language C#
//      -----------
//
//      ushort UpdateCRC(ushort CRC,byte Byte)
//      {
//      	// Update CCITT CRC (reverse polynom 0x8408)
//          Byte ^= (byte)CRC;
//          Byte ^= (byte)(Byte << 4);
//          return (ushort)(((Byte << 8) | (CRC >> 8)) ^ (Byte >> 4) ^ (Byte << 3));
//      }
//
// ******************************************************************

#include "twn4.sys.h"
#include "apptools.h"
#include "prs.h"

// ******************************************************************
// ****** Main Program **********************************************
// ******************************************************************

int main(void)
{
	// COM1 is running @ 9600 Baud 8N1 by default. The code below shows how to
	// modify default baud rate/parity/stop bits.
	/*
	TCOMParameters COMParameters;
	COMParameters.BaudRate = 115200;
  	COMParameters.WordLength = COM_WORDLENGTH_8;
	COMParameters.Parity = COM_PARITY_NONE;
  	COMParameters.StopBits = COM_STOPBITS_1;
	COMParameters.FlowControl = COM_FLOWCONTROL_NONE;
	SetCOMParameters(CHANNEL_COM1,&COMParameters);
	*/

	// Autodetect communication channel USB or COM1 depending on connected cable
	int HostChannel = GetHostChannel();
	// Maybe, you would like to force use of COM2 for communication:
	// HostChannel = CHANNEL_COM2;

	// Simple Protocol is running in ASCII mode W/O CRC.
    SimpleProtoInit(HostChannel,PRS_COMM_MODE_ASCII | PRS_COMM_CRC_OFF);

    //SimpleProtoInit(HostChannel,PRS_COMM_MODE_ASCII | PRS_COMM_CRC_ON);
    //SimpleProtoInit(HostChannel,PRS_COMM_MODE_BINARY | PRS_COMM_CRC_OFF);
    //SimpleProtoInit(HostChannel,PRS_COMM_MODE_BINARY | PRS_COMM_CRC_ON);

    // Main loop
    while (true)
    {
		if (SimpleProtoTestCommand())
		{
			// SimpleProtoMessage/SimpleProtoMessageLength now contains command from host
			SimpleProtoExecuteCommand();
			// SimpleProtoMessage/SimpleProtoMessageLength now contains response to host
			SimpleProtoSendResponse();
		}
    }
}
