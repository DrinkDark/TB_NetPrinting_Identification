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

TBLEUUID* bleUUIDService;
TBLEUUID* bleUUIDChara;
unsigned long gattHandle;Characteristic value receivedAttrOpcodeError characteristic value ReadCardData


byte *receivedUserData;
byte *receivedAttrOpcode;
int *receivedUserDataLength;

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
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    SetVolume(30);
    BeepLow();
    BeepHigh();
}

void OnNewCardFound(const char *CardString)
{
	// Send card string including prefix (actually no prefix) and suffix ("\r")
    HostWriteString(CardString);
    HostWriteString("\r");

    LEDOff(GREENLED);
    LEDOn(REDLED);
    LEDBlink(REDLED,500,500);

    SetVolume(30);
    BeepHigh();
}

void OnCardTimeout(const char *CardString)
{
    LEDOn(GREENLED);
    LEDOff(REDLED);
}

void OnCardDone(void)
{
}

#endif

// ******************************************************************
// ****** Main Program Loop *****************************************
// ******************************************************************

int main(void)
{
	OnStartup();    	

	const byte Params[] = { SUPPORT_CONFIGCARD, 1, CONFIGENABLED, TLV_END };
	SetParameters(Params,sizeof(Params));

	SetTagTypes(LFTAGTYPES,HFTAGTYPES & ~BLE_MASK);

    /*TBLEConfig BLEparams;

    BLEparams.Power = 100;          //Timout of an established connection in milliseconds.
    BLEparams.ConnectTimeout = 40;  //TX power in 0.1dBm steps in the range 0 to 80 (0.0dBm to 8.0dBm).
    BLEparams.BondableMode = 0;     //Bonding mode (0 = off, 1 = on)
    BLEparams.AdvInterval = 200;    //Advertisement interval (values 20ms to 10240ms)
    BLEparams.ChannelMap = 1;       //Advertisement Bluetooth channels
    BLEparams.DiscoverMode = 2;     //Discoverable Modes, which dictate how the device is visible to other devices
    BLEparams.ConnectMode = 0;      //Connectable Modes for the LE (Low Energy) GAP (Generic Access Profile)
    BLEparams.SecurityFlags = 0x00; //Security requirement bitmask
    BLEparams.IOCapabilities = 0;   //Security Management related I/O capabilities
    BLEparams.Passkey = 0;          //Passkey: If security is configured, the application needs to display or ask user to enter a passkey during the bonding process

    BLEPresetConfig(&BLEparams);*/
    //BLEPresetUserData(0x00, (const) receivedUserData, 4);

    TBLEUUID service;
    service.UUID[0] = 0x18;
    service.UUID[1] = 0x1C;
    service.UUIDLength = 2;

    bleUUIDService = &service;

    TBLEUUID chara;
    chara.UUID[0] = 0x29;
    chara.UUID[1] = 0x02;
    chara.UUIDLength = 2;
    bleUUIDChara = &chara;

    byte receivedUserData1[200];
    byte receivedAttrOpcode1[2];
    int receivedUserDataLength1;

    receivedUserData = &receivedUserData1;
    receivedAttrOpcode = &receivedAttrOpcode1;
    receivedUserDataLength = &receivedUserDataLength1;

    gattHandle = 0;

    BLEInit(1);     //0 = used custom mode, 1 = advertisement mode, 5 = discover mode
	
    char OldCardString[MAXCARDSTRINGLEN+1]; 
    OldCardString[0] = 0;

    while (true)
    {
		int TagType;
		int IDBitCnt;
		byte ID[32];

		// Search a transponder
	    if (SearchTag(&TagType,&IDBitCnt,ID,sizeof(ID)) || SEARCH_BLE(&TagType,&IDBitCnt,ID,sizeof(ID)))
	    {
			// A transponder was found. Read data from transponder and convert
			// it into an ASCII string according to configuration
			char NewCardString[MAXCARDSTRINGLEN+1];
			if (ReadCardData(TagType,ID,IDBitCnt,NewCardString,sizeof(NewCardString)-1))
			{
				// Is this a newly found transponder?
				if (strcmp(NewCardString,OldCardString) != 0)
				{
					// Yes. Save new card string
					strcpy(OldCardString,NewCardString);
					OnNewCardFound(NewCardString);
				}
				// (Re-)start timeoutAdvertisement received
                
			   	StartTimer(CARDTIMEOUT);
			}
			OnCardDone();
	    }
    	
        if (TestTimer())
        {
		    OnCardTimeout(OldCardString);
		    OldCardString[0] = 0;
        }


        switch(BLECheckEvent()) {
            case BLE_EVENT_GATT_SERVER_ATTRIBUTE_VALUE :
                HostWriteString("Attribute changed");
                HostWriteString("\r");
                break;

            case BLE_EVENT_CONNECTION_OPENED :
                HostWriteString("Device connected");
                HostWriteString("\r");
                break;
            
            case BLE_EVENT_CONNECTION_CLOSED :
                HostWriteString("Device disconnected");
                HostWriteString("\r");  
                break;
            
            case BLE_EVENT_LE_GAP_EXTENDED_SCAN_RESPONSE :
            case BLE_EVENT_LE_GAP_SCAN_RESPONSE :
                HostWriteString("Advertisement received");
                HostWriteString("\r");  
                break;

            case BLE_EVENT_SM_BONDED :
                HostWriteString("Bonding completed");
                HostWriteString("\r");  
                break;

            case BLE_EVENT_SM_BONDING_FAILED :
                HostWriteString("Bonding failed");
                HostWriteString("\r");  
                break;

            case BLE_EVENT_GATT_CHARACTERISTIC :
                HostWriteString("Characteristic discovered");
                HostWriteString("\r");  
                break;

            case BLE_EVENT_GATT_SERVICE :
                HostWriteString("Service discovered");
                HostWriteString("\r");
                break;
                
            case BLE_EVENT_GATT_PROCEDURE_COMPLETED :
                HostWriteString("Discover completed");
                HostWriteString("\r");
                break;

            case BLE_EVENT_GATT_CHARACTERISTIC_VALUE :
                HostWriteString("Characteristic value received");
                HostWriteString("\r"); 
                BLEGattGetValue(0, gattHandle, (const) bleUUIDChara, receivedAttrOpcode, receivedUserData, receivedUserDataLength, 32); 

                for(uint8_t i = 0; i < (*receivedUserDataLength); i++){ 
                    HostWriteByte(receivedUserData[i]);
                    HostWriteString("i");
                }
                HostWriteString("\r");
                break;
        }
    }
}

