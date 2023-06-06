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
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    SetVolume(20);
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

    SetVolume(20);
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

	char OldCardString[MAXCARDSTRINGLEN+1]; 
    OldCardString[0] = 0;

    BLEInit(1);     //0 = used custom mode, 1 = advertisement mode, 5 = discover mode

    TBLEUUID bleUUIDService;
    bleUUIDService.UUID[0] = 0x2A;
    bleUUIDService.UUID[1] = 0xC3;
    bleUUIDService.UUIDLength = 2;

    TBLEUUID bleUUIDChara;
    bleUUIDChara.UUID[0] = 0x29;
    bleUUIDChara.UUID[1] = 0x02;
    bleUUIDChara.UUIDLength = 2;

    int attrHandle;

    byte receivedUserData[200];
    byte receivedAttrOpcode[2];
    int receivedUserDataLength;

    /*const byte UUID_SPP[16] = {0x5a,0x44,0xc0,0x04,0x41,0x12,0x42,0x74,0x88,0x0e,0xcd,0x9b,0x3d,0xae,0xdf,0x8e};
    const byte UUID_SPP_DATA[16] = {0x43,0xc2,0x9e,0xdf,0x2f,0x0a,0x4c,0x43,0xaa,0x22,0x48,0x9d,0x16,0x9e,0xc7,0x52};

    BLESetStreamingUUID(UUID_SPP, 16, UUID_SPP_DATA, 16);
    BLESetStreamingMode(BLE_STREAM_CONN_ADVERTISE, BLE_STREAM_GATT_CLIENT, BLE_STREAM_TRANSFER_BLOCKWISE);*/

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
				// (Re-)start timeout
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
                if(BLEDiscover(BLE_DISC_SERVICE_WITH_UUID, 0, &bleUUIDService)){
                    HostWriteString("Service discovered with uuid");
                    HostWriteString("\r");
                }

                /*const byte uuid[2] = {0x02,0x29};
                if(BLEFindGattServerAttribute(&uuid, 2, &attrHandle)){
                    HostWriteString("Attribute find");
                    HostWriteString("\r");
                } else {
                    HostWriteString("Error find attribute");
                    HostWriteString("\r");                    
                }*/
                
                const byte uuid[2] = {0x18,0x1C};
                if(BLEGattGetAttribute(&uuid, &attrHandle)){
                    HostWriteString("Attribute find");
                    HostWriteString("\r");
                } else {
                    HostWriteString("Attribute not find");
                    HostWriteString("\r");                    
                }

                if(BLEDiscover(BLE_DISC_CHARAC_WITH_UUID, attrHandle, &bleUUIDChara)){
                    HostWriteString("Characteristic find");
                    HostWriteString("\r");
                } else {
                    HostWriteString("Characteristic not find");
                    HostWriteString("\r");                    
                }

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

                if(BLEGattGetValue(0, (unsigned long) attrHandle, &uuid, &receivedAttrOpcode, &receivedUserData, &receivedUserDataLength, 200)){
                    HostWriteString("Characteristic value read");
                    HostWriteString("\r");
                } else {
                    HostWriteString("Error characteristic value read");
                    HostWriteString("\r");
                }

                for(uint8_t i = 0; i < receivedUserDataLength; i++){ 
                    HostWriteByte(receivedUserData[i]);
                }
                HostWriteString("\r");
        }
    }
}

