// ******************************************************************
//
//    File: App_SMFS100_Sample_File_System.c
//    Date: 2015-06-09
// Version: 1.01
//
// Purpose:
//
// Show some basic operations on the storage and file system of TWN4.
// This sample is showing, how storing some kind of configuration
// could be implemented.
//
// In order to test the program, choose CDC as USB and connect to TWN4
// with a terminal program.
//
// Feel free to modify this program for your specific purposes!
//
// V1.00:
// ------
// - Initial release
//
// V1.01:
// ------
// - Show concrete error text
// - More specific handling of error code from FSMount: There is
//   no need to create new file system in case of file system is 
//   already mounted.
//
// ******************************************************************

// ******************************************************************
// ****** Definitions ***********************************************
// ******************************************************************

#include "twn4.sys.h"
#include "apptools.h"

// This is the file ID, under which the file can be found in the directory:
#define CONFIG_FILEID	42

// Implement a minimum header for the configuration
#define CONFIG_VERSION	1

// This is the structure, which holds our imaginary configuration. It
// is stored 1:1 on the file system.
typedef struct __attribute__((__packed__))
{
	byte Version;
	int Baudrate;
	byte LoginData[6];
	byte MoreStuff[20];
} TConfig;

// ******************************************************************
// ****** Global Variables ******************************************
// ******************************************************************

// The default configuration, which is used, once the App starts up
// the first time
const TConfig DefaultConfig =
{
	.Version = CONFIG_VERSION,
	.Baudrate = 115200,
	.LoginData = { 0x12,0x34,0x56,0x78,0x12,0x34 },
	.MoreStuff =
	{
		0x12,0x34,0x56,0x78,0x90,0x12,0x34,0x56,0x78,0x90,
		0x12,0x34,0x56,0x78,0x90,0x12,0x34,0x56,0x78,0x90,
	}
};

// The storage in memory, once configuration is read from a file
static TConfig Config;

// ******************************************************************
// ****** Error Handler *********************************************
// ******************************************************************

const char *GetErrorString(int ErrorCode)
{
	switch (ErrorCode)
	{
	case ERR_NONE: return "No error";
	case ERR_OUTOFMEMORY: return "Out of memory";
	case ERR_ISALREADYINIT: return "Already initialized";
	case ERR_NOTINIT: return "Not initialized";
	case ERR_ISALREADYOPEN: return "Already opened";
	case ERR_NOTOPEN: return "Not opened";
	case ERR_RANGE: return "Out of range";
	case ERR_PARAMETER: return "Wrong parameter";
	case ERR_UNKNOWNSTORAGEID: return "Unknown storage ID";
	case ERR_WRONGINDEX: return "Wrong index";
	case ERR_FLASHERASE: return "Failed to erase flash memory";
	case ERR_FLASHWRITE: return "Failed to write to flash memory";
	case ERR_SECTORNOTFOUND: return "Sector was not found";
	case ERR_STORAGEFULL: return "Storage is full";
	case ERR_STORAGEINVALID: return "Storage is invalid";
	case ERR_TRANSACTIONLIMIT: return "Limit for transaction was reached";
	case ERR_UNKNOWNFS: return "Unknown file system";
	case ERR_FILENOTFOUND: return "File was not found";
	case ERR_FILEALREADYEXISTS: return "File already exists";
	case ERR_ENDOFFILE: return "End of file";
	case ERR_STORAGENOTFOUND: return "Storage not found";
	case ERR_STORAGEALREADYMOUNTED: return "Storage is already mounted";
	case ERR_ACCESSDENIED: return "Access denied";
	case ERR_FILECORRUPT: return "File is corrupt";
	case ERR_INVALIDFILEENV: return "Invalid file environment";
	case ERR_INVALIDFILEID: return "Invalid file ID";
	case ERR_RESOURCELIMIT: return "Resource limit was reached";
	}
	return "Unknown error";
}

void WriteError(int ErrorCode)
{
	HostWriteString("Error: ");
	HostWriteString(GetErrorString(ErrorCode));
	HostWriteString("\r\n");
}

// ******************************************************************

bool ReadConfig(TConfig *Config)
{
	// ------ Open File ---------------------------------------------

	HostWriteString("Open file for read: ");
    if (!FSOpen(FILE_ENV0,SID_INTERNALFLASH,CONFIG_FILEID,FS_READ))
    {
    	WriteError(GetLastError());
		return false;
    }
	HostWriteString("OK\r\n");

	// ------ Read Data ---------------------------------------------

	HostWriteString("Read data: ");
	int BytesRead;
    if (!FSReadBytes(FILE_ENV0,Config,sizeof(TConfig),&BytesRead))
    {
    	WriteError(GetLastError());
		// File must be closed even in case of an error
		FSClose(FILE_ENV0);
		return false;
    }
	HostWriteString("OK\r\n");
    // Save error for later evaluation
    int ReadError = GetLastError();

	// ------ Close File --------------------------------------------

	HostWriteString("Close file: ");
	if (!FSClose(FILE_ENV0))
	{
    	WriteError(GetLastError());
		return false;
	}
	HostWriteString("OK\r\n");

	// ------ Basic Checks ------------------------------------------

	HostWriteString("Checking config: ");
	if (ReadError != ERR_ENDOFFILE)
	{
    	WriteError(ReadError);
		return false;
	}
	if (BytesRead < sizeof(TConfig))
	{
		HostWriteString("File is too short!\r\n");
		return false;
	}
	if (Config->Version != CONFIG_VERSION)
	{
		HostWriteString("Incompatible configuration!\r\n");
		return false;
	}
	HostWriteString("OK\r\n");

	// ------ Done --------------------------------------------------

    return true;
}

bool WriteConfig(TConfig *Config)
{
	// ------ Open File ---------------------------------------------

	HostWriteString("Open file for write: ");
    if (!FSOpen(FILE_ENV0,SID_INTERNALFLASH,CONFIG_FILEID,FS_WRITE))
    {
    	WriteError(GetLastError());
		return false;
    }
	HostWriteString("OK\r\n");

	// ------ Write Data --------------------------------------------

	HostWriteString("Write data: ");
	int BytesWritten;
    if (!FSWriteBytes(FILE_ENV0,Config,sizeof(TConfig),&BytesWritten))
    {
    	WriteError(GetLastError());
		// File must be closed even in case of an error
		FSClose(FILE_ENV0);
		return false;
    }
	HostWriteString("OK\r\n");

	// ------ Close File --------------------------------------------

	HostWriteString("Close file: ");
	if (!FSClose(FILE_ENV0))
	{
    	WriteError(GetLastError());
		return false;
	}
	HostWriteString("OK\r\n");

	// ------ Done --------------------------------------------------

    return true;
}

bool DeleteConfig(void)
{
	HostWriteString("Delete file: ");
	if (!FSDelete(SID_INTERNALFLASH,CONFIG_FILEID))
	{
    	WriteError(GetLastError());
		return false;
	}
	HostWriteString("OK\r\n");
	return true;
}

bool StartupConfig(void)
{
	HostWriteString("Mounting storage: ");
	if (!FSMount(SID_INTERNALFLASH,FS_MOUNT_READWRITE))
	{
		int LastError = GetLastError();
    	WriteError(LastError);
    	if (LastError != ERR_STORAGEALREADYMOUNTED)
    	{
			HostWriteString("Format/create file system: ");
			if (!FSFormat(SID_INTERNALFLASH,FS_FORMATMAGICVALUE))
			{
		    	WriteError(GetLastError());
				return false;
			}
			HostWriteString("OK\r\n");
		}
	}
	else
		HostWriteString("OK\r\n");

	if (!ReadConfig(&Config))
	{
		Config = DefaultConfig;
		if (!WriteConfig(&Config))
			return false;
	}
	return true;
}

void HostDumpConfig(TConfig *Config)
{
	byte *Data = (byte*)Config;
	for (int Length = sizeof(TConfig); Length > 0; )
	{
		int BlockLength = MIN(Length,16);
		HostWriteHex(Data,BlockLength*8,BlockLength*2);
		HostWriteString("\r\n");
		Data += BlockLength;
		Length -= BlockLength;
	}
}

int main(void)
{
	// Wait for any key from host before starting up
	HostWriteString("Hit a key to start: ");
    HostReadChar();
    // Prepare file system and read configuration
	StartupConfig();
	HostWriteString("\r\n");
	HostWriteString("Please choose wisely:\r\n");
	HostWriteString(" 1: Write Config\r\n");
	HostWriteString(" 2: Read Config\r\n");
	HostWriteString(" 3: Delete Config\r\n");
	HostWriteString(" 4: Dump Config\r\n");
	while (true)
	{
		HostWriteChar('>');
    	char Char = HostReadChar();
		HostWriteChar(Char);
		HostWriteString("\r\n");
    	switch (Char)
    	{
    	case '1':
			WriteConfig(&Config);
    		break;
    	case '2':
			ReadConfig(&Config);
    		break;
    	case '3':
			DeleteConfig();
    		break;
    	case '4':
			HostDumpConfig(&Config);
    		break;
	    }
	}
}
