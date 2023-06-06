#include "twn4.sys.h"
#include "apptools.h"

// ******************************************************************
// ****** Configuration Area ****************************************
// ******************************************************************

#if APPEXTCONFIG

#include "appconfig.h"

#else

#ifndef CONFIGENABLED
  #define CONFIGENABLED         SUPPORT_CONFIGCARD_OFF
#endif  

#define LFTAGTYPES        		(ALL_LFTAGS & ~(TAGMASK(LFTAG_TIRIS) | TAGMASK(LFTAG_ISOFDX) | TAGMASK(LFTAG_PAC) | TAGMASK(LFTAG_COTAG) | TAGMASK(LFTAG_DEISTER)))
#define HFTAGTYPES       		(ALL_HFTAGS & ~(TAGMASK(HFTAG_NFCP2P) | TAGMASK(HFTAG_BLE)))

#define CARDTIMEOUT				2000UL	// Timeout in milliseconds
#define MAXCARDIDLEN            32		// Length in bytes
#define MAXCARDSTRINGLEN		128   	// Length W/O null-termination

#define SEARCH_BLE(a,b,c,d)		false
#define BLE_MASK				0

bool ReadCardData(int TagType,const byte* ID,int IDBitCnt,char *CardString,int MaxCardStringLen)
{
	// Select data from card (take any ID from any transponder)
	byte CardData[32];
	int CardDataBitCnt;
    CardDataBitCnt = MIN(IDBitCnt,sizeof(CardData)*8);
    CopyBits(CardData,0,ID,0,CardDataBitCnt);

	// Modify card data (do not modify, just copy)

  	// Convert data to ASCII
    ConvertBinaryToString(CardData,0,CardDataBitCnt,CardString,16,(CardDataBitCnt+7)/8*2,MaxCardStringLen);
    return true;
}

// ****** Event Handler *********************************************
void OnStartup(void)
{
    LEDInit(REDLED | YELLOWLED | GREENLED);
    LEDOff(REDLED);
    LEDOff(YELLOWLED);
    LEDOn(GREENLED);
    SetVolume(30);
}
// BLE Mode
const TBLEConfig BleConfig = {
    .ConnectTimeout = 0,
    .Power = 50,
    .BondableMode = 0xA1,
    .AdvInterval = 0x00a0,
    .ChannelMap = 0x07,
    .DiscoverMode = 0x04,
    .ConnectMode = 0x02,
    .SecurityFlags = 0x00,
    .IOCapabilities = 0x00,
    .Passkey = 0x00000000,
};

// User data
byte userdata[30] = {
    0x02, // Length
    0x01, // # Flags
    0x06, // Device in LE General discoverable mode
    0x09, // Length
    0x09, // # Name
    ’T’,’W’,’N’,’4’,’ ’,’B’,’L’,’E’,
    0x0E, // Length of the Manufacturer Data field
    0xff, // Data type - manufacturer specific data - Manufacturer Data field
    0x52,0x07, // Manufacturer data, Company ID field - 0x0752 = ELATEC GmbH SIG
    // Application Code 1 (example from ELATEC)
    0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

const byte UUID_SPP[16] = {0x8e,0xdf,0xae,0x3d,0x9b,0xcd,0x0e,0x88,0x74,0x42,0x12,0x41,0x04,0xc0,0x44,0x5a};
const byte UUID_SPP_DATA[16] = {0x52,0xc7,0x9e,0x16,0x9d,0x48,0x22,0xaa,0x43,0x4c,0x0a,0x2f,0xdf,0x9e,0xc2,0x43};

// Main program
int main(void)
{
    OnStartup();
    BLEPresetUserData(0, userdata, sizeof(userdata));
    BLEPresetConfig(&BleConfig);
    if(BLEInit(BLE_MODE_CUSTOM)){
        BeepHigh();
    }

    if(BLESetStreamingUUID(UUID_SPP, 16, UUID_SPP_DATA, 16)){
        BeepHigh();
    }

    if(BLESetStreamingMode(BLE_STREAM_CONN_ADVERTISE, BLE_STREAM_GATT_SERVER, BLE_STREAM_TRANSFER_BLOCKWISE)){
        BeepHigh();
    }
    
    int HostChannel = CHANNEL_BLE;

    // Simple Protocol is running in ASCII mode W/O CRC.
    SimpleProtoInit(HostChannel,PRS_COMM_MODE_ASCII | PRS_COMM_CRC_OFF);
    // Main loop

    while (true) {
        if (SimpleProtoTestCommand()) {
            // SimpleProtoMessage (MessageLength now contains command from host)
            SimpleProtoExecuteCommand();
            // SimpleProtoMessage (MessageLength now contains response to host)
            SimpleProtoSendResponse();
        }
    }
}


