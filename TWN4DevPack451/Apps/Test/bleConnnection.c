#include "twn4.sys.h"
#include "apptools.h"

//------------------------------------------------------------------------------------
//------------------------------  CONFIGURATION  -------------------------------------

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

void OnStartup(void)
{
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    SetVolume(50);
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
    LEDBlink(REDLED,200,200);

    SetVolume(50);
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

int main(void)
{
	OnStartup();    	

    //------------------------------------------------------------------------------------
    //--------------------------------  CARD INIT  ---------------------------------------
    
	const byte Params[] = { SUPPORT_CONFIGCARD, 1, CONFIGENABLED, TLV_END };
	SetParameters(Params,sizeof(Params));

	SetTagTypes(LFTAGTYPES,HFTAGTYPES & ~BLE_MASK);

	char OldCardString[MAXCARDSTRINGLEN+1]; 
    OldCardString[0] = 0;


    BLEInit(1);     //Init BLE in advertise mode



    while (true)
    {
        //------------------------------------------------------------------------------------
        //--------------------------------  CARD VALUE  --------------------------------------
		int TagType;
		int IDBitCnt;
		byte ID[32];

        //------------------------------------------------------------------------------------
        //---------------------------------  BLE VALUE  --------------------------------------
        int attrHandle;
        int attrStatusFlag;
        int attrConfigFlag;
        byte receivedUserData[200];
        int receivedUserDataLength;

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
                //HostWriteString("Attribute changed");
                //HostWriteString("\r");

                //Get attribute handle of the modified value
                BLEGetGattServerCharacteristicStatus(&attrHandle, &attrStatusFlag, &attrConfigFlag);

                //Attribute handle bit 15 set to 1 = BLE_EVENT_GATT_SERVER_ATTRIBUTE_VALUE
                attrHandle += (int)(0b1000000000000000);   
                
                //Read the modified value based on the read handle attribute
                if(BLEGetGattServerAttributeValue(attrHandle, &receivedUserData, &receivedUserDataLength, 200)){
                    //HostWriteString("Characteristic value read");
                    //HostWriteString("\r");

                    //Print read value on the output
                    for(uint8_t j = 0; j < receivedUserDataLength; j++){ 
                        HostWriteByte(receivedUserData[j]);
                    }
                    HostWriteString("\r");
                } else {
                    //HostWriteString("Error characteristic value read");
                    //HostWriteString("\r");
                }

            break;

            case BLE_EVENT_CONNECTION_OPENED :
                //HostWriteString("Device connected");
                //HostWriteString("\r");
                LEDOff(GREENLED);
                LEDBlink(REDLED,200,200);
                Beep(50, 1500, 100, 100);
                Beep(50, 1500, 100, 100);
                break;
            
            case BLE_EVENT_CONNECTION_CLOSED :
                //HostWriteString("Device disconnected");
                //HostWriteString("\r");  
                LEDOff(REDLED);
                LEDOn(GREENLED);
                Beep(50, 800, 500, 100);
                break;

            case BLE_EVENT_SM_PASSKEY_REQUEST :
                HostWriteString("Passkey request");
                HostWriteString("\r");  
            break;
        
        }
    }
}

