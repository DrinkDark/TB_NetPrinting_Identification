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

#define BLE_DATA_SIZE			16

enum States {
    WaitAuthentication,
    DeviceAuthentication,
    DeviceAuthenticated,
    AppAuthentication,
    WaitAppAuthentication,
    AppAuthenticated,
    Authenticated,
    Identification,
    AuthenticationFailed
} currentState;

//Read the card value from the readed card
bool ReadCardData(int TagType,const byte* ID,int IDBitCnt,char *CardString,int MaxCardStringLen)
{
	// Select data from card (take any ID from any transponder)
	byte CardData[32];
	int CardDataBitCnt;
    CardDataBitCnt = MIN(IDBitCnt,sizeof(CardData)*8);
    CopyBits(CardData,0,ID,0,CardDataBitCnt);

  	// Convert data to ASCII
    ConvertBinaryToString(CardData,0,CardDataBitCnt,CardString,16,(CardDataBitCnt+7)/8*2,MaxCardStringLen);
    return true;
}

//Startup fonction for the card reader
void OnStartup(void)
{
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    SetVolume(50);
    BeepLow();
    BeepHigh();
}

//New card found
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

void deviceConnected() {
    //HostWriteString("Device connected");
    //HostWriteString("\r");

    LEDOff(GREENLED);
    LEDBlink(REDLED,200,200);

    Beep(50, 1500, 100, 100);
    Beep(50, 1500, 100, 100);
}

void deviceDisconnected() {
    //HostWriteString("Device disconnected");
    //HostWriteString("\r");  

    LEDOff(REDLED);
    LEDOn(GREENLED);

    Beep(50, 800, 500, 100);
}

//Transform the byte array by merging every two bytes into one byte
//ex: {0x1, 0x2, 0x3, 0x4, ...} -> {0x12, 0x34, ...}
void transformByteArray(byte* array, byte* transformedArray){   
    for (size_t i = 0; i < (BLE_DATA_SIZE * 2); i += 2) {
        char high = (array)[i];
        char low = (array)[i + 1];

        high = ScanHexChar(high);
        low = ScanHexChar(low);

        transformedArray[i / 2] = ((high << 4) | (low & 0x0F));
    }
}

void getRandNum(byte* randNum){
    byte buf[BLE_DATA_SIZE];

    //Set seed for random number based on the system ticks
    srand(GetSysTicks());

    for (int i = 0; i < sizeof(buf); i++) {
        randNum[i] = rand() % 256;
    }

}

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


    //------------------------------------------------------------------------------------
    //---------------------------------  BLE INIT  ---------------------------------------
    TBLEConfig BLEConfig =  {
        .ConnectTimeout = 10000,   //Timout of an established connection in milliseconds
        .Power = 20,               //TX power : 0 to 80 (0.0dBm to 8.0dBm)
        .BondableMode = 0x00,      //Bonding : 0 = off, 1 = on
        .AdvInterval = 200,        //Advertisement interval : values 20ms to 10240ms
        .ChannelMap = 0x07,        //Advertisement Bluetooth channels : 7 = CH37 + CH38 + CH39
        .DiscoverMode = 0x02,      //Discoverable Mode : 2 = LE_GAP_GENERAL_DISCOVERABLE
        .ConnectMode = 0x02,       //Connectable mode : 2 = LE_GAP_CONNECTABLE_SCANNABLE
        .SecurityFlags = 0x00,     //Security requirement bitmask : Bit 0 = 0 Allow bonding without MITM protection, Bit 1 = 0 Allow encryption without bonding
        .IOCapabilities = 0x04,    //Security Management related I/O capabilities : 4 = keyboard / display
        .Passkey = 0x00000000,      //Passkey if security is configured
    };

    BLEPresetConfig(&BLEConfig);

    BLEInit(BLE_MODE_CUSTOM);   //BLE_MODE_CUSTOM use BLEPresetConfig param

    //------------------------------------------------------------------------------------
    //------------------------------  CRYPTO INIT  ---------------------------------------
    const byte aesKey[] = {0xbf, 0xc1, 0xc1, 0x8b, 0x3c, 0x60, 0x50, 
        0x2a, 0x4f, 0x08, 0xdf, 0xb6, 0xe0, 0xd9, 0xd1, 0x1f};          //Shared 128 bits AES shared key 

    const byte data[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,      
            0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00, };      //Data to send (to be remplace by a random number)

    Crypto_Init(CRYPTO_ENV0, CRYPTOMODE_CBC_AES128, &aesKey, sizeof(aesKey));   //Enable encryption initialisation with CRYPTO_ENV0 for init vector, CBC-AES128 encryption and the key

    byte encryptedData[BLE_DATA_SIZE];
    byte decryptedData[BLE_DATA_SIZE * 2];

    byte randNum[BLE_DATA_SIZE];

    currentState = WaitAuthentication;

    while (true)
    {
        //------------------------------------------------------------------------------------
        //--------------------------------  CARD VALUES  -------------------------------------
		int TagType;
		int IDBitCnt;
		byte ID[32];

        //------------------------------------------------------------------------------------
        //--------------------------------  BLE VALUES  --------------------------------------
        int attrHandle;
        int attrStatusFlag;
        int attrConfigFlag;

        byte receivedDataBLE[BLE_DATA_SIZE * 2];
        int receivedDataBLELength;

        byte trandformedReceivedDataBLE[BLE_DATA_SIZE];

        //------------------------------------------------------------------------------------
        //------------------------------  CARD IDENTIFICATION  -------------------------------

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

        //------------------------------------------------------------------------------------
        //---------------------------------  STATE MACHINE  ----------------------------------

        switch(currentState) {

            case DeviceAuthentication:
                //HostWriteString("DeviceAuthentication");
                //HostWriteString("\r");

                Encrypt(CRYPTO_ENV0, (const) &trandformedReceivedDataBLE, &encryptedData, sizeof(encryptedData));

                BLESetGattServerAttributeValue(attrHandle, 0, &encryptedData, sizeof(encryptedData));

                currentState = DeviceAuthenticated;
                break;


            case AppAuthentication:
                //HostWriteString("AppAuthentication");
                //HostWriteString("\r");

                getRandNum(&randNum);
                BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(data));

                currentState = WaitAppAuthentication;
                break;

            case AppAuthenticated:
                //HostWriteString("AppAuthenticated");
                //HostWriteString("\r");

                Decrypt(CRYPTO_ENV0, (const) &trandformedReceivedDataBLE, &decryptedData, sizeof(decryptedData));

                byte transformedDecryptedData[BLE_DATA_SIZE];
                transformByteArray(&decryptedData, &transformedDecryptedData);
                 
                if (memcmp(&transformedDecryptedData, &randNum, sizeof(transformedDecryptedData))) {    
                    getRandNum(&randNum);                                            
                    BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(data));
                    currentState = Authenticated;
                } else {
                    currentState = AuthenticationFailed;
                }
                break;

            case Identification:
                //HostWriteString("Identification");
                //HostWriteString("\r");

                CBC_ResetInitVector(CRYPTO_ENV0);

                //Print read value on the output byte by byte 
                for(uint8_t j = 0; j < receivedDataBLELength; j++){ 
                    HostWriteByte(receivedDataBLE[j]);
                }
                HostWriteString("\r");

                currentState = WaitAuthentication;
                break;

            case AuthenticationFailed:
                //HostWriteString("AuthenticationFailed");
                //HostWriteString("\r");

                CBC_ResetInitVector(CRYPTO_ENV0);

                BLEDisconnectFromDevice();
                return;

            default:
                break;
        }

        //------------------------------------------------------------------------------------
        //-------------------------------  BLE IDENTIFICATION  -------------------------------
        
        //Check all the BLE events. Only the needed case are implemented. 
        switch(BLECheckEvent()) {

            //Device connected to the card reader
            case BLE_EVENT_CONNECTION_OPENED :
                deviceConnected();
                break;
            
            //Device disconnected from the card reader
            case BLE_EVENT_CONNECTION_CLOSED :
                deviceDisconnected();
                break;  

            //Characteristic modified in the GATT server
            //Get the attribute handle of the 
            case BLE_EVENT_GATT_SERVER_ATTRIBUTE_VALUE :
                //HostWriteString("Attribute changed");
                //HostWriteString("\r");

                //Get attribute handle of the modified characteristic
                BLEGetGattServerCharacteristicStatus(&attrHandle, &attrStatusFlag, &attrConfigFlag);

                //Attribute handle bit 15 have to be set to 1 when event BLE_EVENT_GATT_SERVER_ATTRIBUTE_VALUE
                attrHandle += (int)(0b1000000000000000);   

                //Read the modified value based on the read attribute handle
                bool dataReceived = BLEGetGattServerAttributeValue(attrHandle, &receivedDataBLE, &receivedDataBLELength, sizeof(receivedDataBLE));

                transformByteArray(&receivedDataBLE, &trandformedReceivedDataBLE);

                switch(currentState) {
                    case WaitAuthentication:
                        //HostWriteString("WaitAuthentication");
                        //HostWriteString("\r");

                        if(dataReceived){
                            currentState = DeviceAuthentication;
                        } else {
                            currentState = AuthenticationFailed;
                        }
                        break;

                    case DeviceAuthenticated:
                        //HostWriteString("DeviceAuthenticated");
                        //HostWriteString("\r");

                        if(dataReceived){
                            currentState = AppAuthentication;
                        } else {
                            currentState = AuthenticationFailed;
                        }
                        break;

                    case WaitAppAuthentication:
                        //HostWriteString("WaitAppAuthentication");
                        //HostWriteString("\r");

                        if(dataReceived){
                            currentState = AppAuthenticated;
                        } else {
                            currentState = AuthenticationFailed;
                        }
                        break;

                    case Authenticated:
                        //HostWriteString("Authenticated");
                        //HostWriteString("\r");

                        if(dataReceived){
                            currentState = Identification;
                        } else {
                            currentState = AuthenticationFailed;
                        }
                        break;

                        default:
                            break;
                }

            break;

            default:
                break;

                  
        }
    }
}

