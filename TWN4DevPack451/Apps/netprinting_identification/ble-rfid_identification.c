//////////////////////////////////////////////////////////////////////////////////
//                          RFID/BLE CARD READER FIRMWARE
//                        
//
// - Read MIFARE card and print ID
// - Communicate via BLE
//      o Advertise
//      o Connect
//      o Authentify himself and phone via double authentication using random 16 bytes numbers
//      o Print ID
//      o Disconnect from device
//////////////////////////////////////////////////////////////////////////////////

#include "twn4.sys.h"
#include "apptools.h"


//------------------------------------------------------------------------------------
//                                  DEFINE CONSTANT  
//------------------------------------------------------------------------------------

#if APPEXTCONFIG

#include "appconfig.h"

#else

#ifndef CONFIGENABLED
  #define CONFIGENABLED         SUPPORT_CONFIGCARD_OFF
#endif  
       		
#define HFTAGTYPES       		(TAGMASK(HFTAG_MIFARE))

#define CARDTIMEOUT				2000UL	// Timeout in milliseconds
#define MAXCARDIDLEN            32		// Length in bytes
#define MAXCARDSTRINGLEN		128   	// Length W/O null-termination

#define BLE_DATA_SIZE			16      // Max length of a BLE packet

// Enumeration for states of the state machine
enum States {
    WaitAuthentication,     // Wait the start of the authentication protocol
    DeviceAuthentication,   // Device (card reader) authentication is proceeding
    DeviceAuthenticated,    // The Device is authenticated
    AppAuthentication,      // App authentication is proceeding
    WaitAppAuthentication,  // Wait the app's response for the app authentication
    AppAuthenticated,       // The app is authenticated
    Authenticated,          // Authentication protocole succeeded and finished
    Identification,         // App identifies himself (transmits ID)
    AuthenticationFailed    // A error occurred during the authentication process
} currentState;

bool BLEDeviceConnected = false;    // A BLE device is connected


/**
 * Read card data function
 * 
 * Function to read card data and convert it to an ASCII string
 * 
 * @param TagType : indicating the type of RFID card
 * @param ID : pointer to the ID of the card
 * @param IDBitCnt : bit length of the ID
 * @param CardString : pointer to the card data
 * @param MaxCardStringLen : max length of the card data
 * 
 * @return true
*/
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


/**
 * Startup fonction for the card reader
 * 
 * Function called at startup to initialize the reader
 * - Set the leds to green and emit a low and high beeps
 * 
*/
void OnStartup(void)
{
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    SetVolume(50);
    BeepLow();
    BeepHigh();
}

/**
 * New card found
 * 
 * Function called when a new card is found
 * - Sends the card string via UART and updates LEDs and beep to signal the send
 * 
 * @param CardString : pointer to the card data
 * 
*/
void OnNewCardFound(const char *CardString)
{
	// Output card string and suffix ("\r")
    HostWriteString(CardString);
    HostWriteString("\r");

    LEDOff(GREENLED);
    LEDOn(REDLED);
    LEDBlink(REDLED,200,200);

    SetVolume(50);
    BeepHigh();
}

/**
 * Card timeout
 * 
 * Function called when the card timeout occurs
 * - Resets LEDs
 * 
 * @param CardString : pointer to the card data
 * 
*/
void OnCardTimeout(const char *CardString)
{
    LEDOn(GREENLED);
    LEDOff(REDLED);
}

/**
 * Card processing done
 * 
 * Function called when card processing is done
 * 
*/
void OnCardDone(void)
{
}

#endif

/**
 * Device connected
 * 
 * Callback function called when a BLE device is connected
 * - Set leds and beeps
 * - Reset Init Vector for the encryption
 * - Start timer (for the timeout)
 * 
*/
void deviceConnected() {
    //HostWriteString("Device connected");
    //HostWriteString("\r");

    LEDOff(GREENLED);
    LEDBlink(REDLED,200,200);

    SetVolume(50);
    BeepHigh();

    // Reset IV for encryption environnement 0 :
    // The IV is incremented after every encryption or decryption -> not possible in this case
    // Different devices use the same reader and can not folloow the incrementation 
    CBC_ResetInitVector(CRYPTO_ENV0);

    BLEDeviceConnected = true;

    StartTimer(10000);  // Set the disconnect device timeout to 10s for the BLE
}

/**
 * Device disconnected
 * 
 * Callback function called when a BLE device is disconnected
 * - Set leds and beeps
 * 
*/
void deviceDisconnected() {
    //HostWriteString("Device disconnected");
    //HostWriteString("\r");  

    LEDOff(REDLED);
    LEDOn(GREENLED);

    BLEDeviceConnected = false;
}


/**
 * Transform the byte array
 * 
 * Function to transform a byte array by merging every two bytes into one byte
 * ex: {0x1, 0x2, 0x3, 0x4, ...} -> {0x12, 0x34, ...}
 * 
 * @param array : pointer to the array to transform
 * @param transformedArray : pointer to the transformed array
 * 
*/
void transformByteArray(byte* array, byte* transformedArray){   
    for (size_t i = 0; i < (BLE_DATA_SIZE * 2); i += 2) {
        char high = (array)[i];
        char low = (array)[i + 1];

        high = ScanHexChar(high);
        low = ScanHexChar(low);

        transformedArray[i / 2] = ((high << 4) | (low & 0x0F));
    }
}

/**
 * Generate a random number 
 * 
 * Function to generate a random bytes number of the argument size
 * 
 * @param randNum : pointer to the random number
 * 
*/
void generateRandNum(byte* randNum){
    // Set seed for random number based on the system ticks
    // Can produce different sequences of random numbers each time the function is called, as long as the system ticks are different.
    srand(GetSysTicks());

    for (int i = 0; i < sizeof(randNum); i++) {
        randNum[i] = rand() % 256;      // % 256 ensures that the random number is within the range of 0 to 255 (one byte)
    }

}

/**
 * Choose the state machine state after a attribute change
 * 
 * @param dataReceived data as been received
 * 
*/
void chooseSMstateAttributeChanged(bool dataReceived) {
    switch(currentState) {

        // -------------------------------------------------------------------------------------
        // Wait authentication response
        //
        // Called when the app has return device authentication confirmation (write a random number in the attribute)
        // -------------------------------------------------------------------------------------
        case WaitAuthentication:
            //HostWriteString("WaitAuthentication");
            //HostWriteString("\r");

            if(dataReceived){
                currentState = DeviceAuthentication;
            } else {
                currentState = AuthenticationFailed;
            }

            break;

        // -------------------------------------------------------------------------------------
        // Device authenticated
        //
        // Called when the device is authenticated
        // -------------------------------------------------------------------------------------
        case DeviceAuthenticated:
            //HostWriteString("DeviceAuthenticated");
            //HostWriteString("\r");

            if(dataReceived){
                currentState = AppAuthentication;
            } else {
                currentState = AuthenticationFailed;
            }

            break;

        // -------------------------------------------------------------------------------------
        // Wait app authentication
        //
        // Called when the app return the encrypt random number
        // -------------------------------------------------------------------------------------
        case WaitAppAuthentication:
            //HostWriteString("WaitAppAuthentication");
            //HostWriteString("\r");

            if(dataReceived){
                currentState = AppAuthenticated;
            } else {
                currentState = AuthenticationFailed;
            }
            break;

        // -------------------------------------------------------------------------------------
        // Authenticated
        //
        // Called when both the device and the app are authenticated
        // -------------------------------------------------------------------------------------
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
}

int main(void)
{
	OnStartup();    	

    //------------------------------------------------------------------------------------
    //--------------------------------  CARD INIT  ---------------------------------------
    
	const byte Params[] = {SUPPORT_CONFIGCARD, 1, CONFIGENABLED, TLV_END};    // Array that contain parameters 
	SetParameters(Params,sizeof(Params));

	SetTagTypes(0, HFTAGTYPES);

	char OldCardString[MAXCARDSTRINGLEN+1];     // Array with the old card string
    OldCardString[0] = 0;


    //------------------------------------------------------------------------------------
    //---------------------------------  BLE INIT  ---------------------------------------

    // BLE parameters
    TBLEConfig BLEConfig =  {
        .ConnectTimeout = 12000,   // Timout of an established connection in milliseconds
        .Power = 20,               // TX power : 0 to 80 (0.0dBm to 8.0dBm)
        .BondableMode = 0x00,      // Bonding : 0 = off, 1 = on
        .AdvInterval = 200,        // Advertisement interval : values 20ms to 10240ms
        .ChannelMap = 0x07,        // Advertisement Bluetooth channels : 7 = CH37 + CH38 + CH39
        .DiscoverMode = 0x02,      // Discoverable Mode : 2 = LE_GAP_GENERAL_DISCOVERABLE
        .ConnectMode = 0x02,       // Connectable mode : 2 = LE_GAP_CONNECTABLE_SCANNABLE
        .SecurityFlags = 0x00,     // Security requirement bitmask : Bit 0 = 0 Allow bonding without MITM protection, Bit 1 = 0 Allow encryption without bonding
        .IOCapabilities = 0x04,    // Security Management related I/O capabilities : 4 = keyboard / display
        .Passkey = 0x00000000,     // Passkey if security is configured
    }; 

    BLEPresetConfig(&BLEConfig);

    BLEInit(BLE_MODE_CUSTOM);   // BLE_MODE_CUSTOM use BLEPresetConfig param

    //------------------------------------------------------------------------------------
    //------------------------------  CRYPTO INIT  ---------------------------------------

    const byte aesKey[] = {0xbf, 0xc1, 0xc1, 0x8b, 0x3c, 0x60, 0x50, 
        0x2a, 0x4f, 0x08, 0xdf, 0xb6, 0xe0, 0xd9, 0xd1, 0x1f};          // 128 bits AES shared key 

    Crypto_Init(CRYPTO_ENV0, CRYPTOMODE_CBC_AES128, &aesKey, sizeof(aesKey));   // Enable encryption initialisation with CRYPTO_ENV0 for init vector, CBC-AES128 encryption and the key

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

		// Search a card if no BLE device are connected
	    if (SearchTag(&TagType,&IDBitCnt,ID,sizeof(ID)) && !BLEDeviceConnected)
	    {
			// A transponder was found. Read data from transponder and convert
			// it into an ASCII string according to configuration
			char NewCardString[MAXCARDSTRINGLEN+1];

			if (ReadCardData(TagType,ID,IDBitCnt,NewCardString,sizeof(NewCardString)-1))
			{
				// Control if new card
				if (strcmp(NewCardString,OldCardString) != 0)
				{
					strcpy(OldCardString,NewCardString);
					OnNewCardFound(NewCardString);
				}
				// (Re-)start timeout
			   	StartTimer(CARDTIMEOUT);
			}
			OnCardDone();
	    }
    	
        // Test the timer value
        if (TestTimer())
        {
            // Control if timer for card or BLE use
            if(BLEDeviceConnected) {
                currentState = AuthenticationFailed;
            } else {
                OnCardTimeout(OldCardString);
		        OldCardString[0] = 0;
            }
        }

        //------------------------------------------------------------------------------------
        //---------------------------------  STATE MACHINE  ----------------------------------
        if(BLEDeviceConnected){
            switch(currentState) {

                // -------------------------------------------------------------------------------------
                // Start the device authentication
                //
                // Called when a device is connected and write a random number in a charachteristic
                // Encrypt the data if correct length and send them to the device via a notification 
                // -------------------------------------------------------------------------------------
                case DeviceAuthentication:
                    //HostWriteString("DeviceAuthentication");
                    //HostWriteString("\r");

                    // Control received data size
                    if(sizeof(trandformedReceivedDataBLE) == BLE_DATA_SIZE) {
                        Encrypt(CRYPTO_ENV0, (const) &trandformedReceivedDataBLE, &encryptedData, sizeof(encryptedData));

                        BLESetGattServerAttributeValue(attrHandle, 0, &encryptedData, sizeof(encryptedData));       // Write the encrypt data in the attribute and send a notification to the device

                        currentState = DeviceAuthenticated;
                    } else {
                        currentState = AuthenticationFailed; 
                    }

                break;

                // -------------------------------------------------------------------------------------
                // App being authenticated
                //
                // Called when the device has been authenticated
                // Send a random number to the device via a notification 
                // -------------------------------------------------------------------------------------
                case AppAuthentication:
                    //HostWriteString("AppAuthentication");
                    //HostWriteString("\r");

                    generateRandNum(&randNum);

                    BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));   // Write the random number in the attribute and send a notification to the device

                    currentState = WaitAppAuthentication;

                    break;

                // -------------------------------------------------------------------------------------
                // App authenticated
                //
                // Called when the app has return the encrypt random number
                // Decrypt and compare to received data to the send random number
                // -------------------------------------------------------------------------------------
                case AppAuthenticated:
                    //HostWriteString("AppAuthenticated");
                    //HostWriteString("\r");

                    Decrypt(CRYPTO_ENV0, (const) &trandformedReceivedDataBLE, &decryptedData, sizeof(decryptedData));

                    // The Decrypt function return the data in the incorrect format. It as to be transformed. 
                    byte transformedDecryptedData[BLE_DATA_SIZE];
                    transformByteArray(&decryptedData, &transformedDecryptedData);
                    
                    // Compare the received decrypt data with the send random number
                    if (memcmp(&transformedDecryptedData, &randNum, sizeof(transformedDecryptedData))) {    
                    
                        // Write a random number in the attribute and send a notification to the device
                        // to sigifie the the success of the authentication procedure
                        generateRandNum(&randNum);                                            
                        BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));

                        currentState = Authenticated;
                    } else {
                        currentState = AuthenticationFailed;
                    }

                    break;

                // -------------------------------------------------------------------------------------
                // Identification procedure
                //
                // Called when the app is authenticate and it has send is ID 
                // Output the ID and overwrite the ID in the attribute with a dumb value
                // -------------------------------------------------------------------------------------
                case Identification:
                    //HostWriteString("Identification");
                    //HostWriteString("\r");

                    //Output read value byte by byte 
                    for(uint8_t j = 0; j < receivedDataBLELength; j++){ 
                        HostWriteByte(receivedDataBLE[j]);
                    }
                    HostWriteString("\r");

                    // Write a dumb value in the attribute to overwrite the make disappear the ID
                    attrHandle -= (int)(0b1000000000000000);    // bite 15 of the attribute handle to 0 -> write without notification
                    generateRandNum(&randNum);
                    BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));

                    currentState = WaitAuthentication;
                    break;

                // -------------------------------------------------------------------------------------
                // Authentification failed
                //
                // Called when a error occurred in the authentication process 
                // Overwrite the value in the attribute with a dumb value and disconnect from device
                // -------------------------------------------------------------------------------------
                case AuthenticationFailed:
                    //HostWriteString("AuthenticationFailed");
                    //HostWriteString("\r");
                    
                    // Write a dumb value in the attribute to overwrite the make disappear the ID
                    attrHandle -= (int)(0b1000000000000000);     // bite 15 of the attribute handle to 0 -> write without notification 
                    generateRandNum(&randNum);
                    BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));

                    BLEDisconnectFromDevice();
                    deviceDisconnected();       // Call callback (normally called by the BLECheckEvent)
                    BLEInit(BLE_MODE_CUSTOM);   // Init BLE to ensure that BLE is working properly on the next connection

                    currentState = WaitAuthentication;
                    break;

                default:
                    break;
            }
        }
    
        //------------------------------------------------------------------------------------
        //----------------------------------  BLE EVENT  -------------------------------------
    
        //Check all the BLE events. Only the needed case are implemented. 
        switch(BLECheckEvent()) {

            // -------------------------------------------------------------------------------------
            // Device connected to the card reader
            // -------------------------------------------------------------------------------------
            case BLE_EVENT_CONNECTION_OPENED :
                deviceConnected();

                break;
            
            // -------------------------------------------------------------------------------------
            // Device disconnected from the card reader
            // -------------------------------------------------------------------------------------
            case BLE_EVENT_CONNECTION_CLOSED :
                deviceDisconnected();

                break;  

            // -------------------------------------------------------------------------------------
            // Characteristic modified in the GATT server
            //
            // Read the value of the modify attribute and transform it
            // -------------------------------------------------------------------------------------
            case BLE_EVENT_GATT_SERVER_ATTRIBUTE_VALUE :
                //HostWriteString("Attribute changed");
                //HostWriteString("\r");

                BLEGetGattServerCharacteristicStatus(&attrHandle, &attrStatusFlag, &attrConfigFlag);    //Get attribute handle of the modified characteristic
                
                attrHandle += (int)(0b1000000000000000);    //Attribute handle bit 15 have to be set to 1 when event BLE_EVENT_GATT_SERVER_ATTRIBUTE_VALUE

                bool dataReceived = BLEGetGattServerAttributeValue(attrHandle, &receivedDataBLE, &receivedDataBLELength, sizeof(receivedDataBLE));  //Read the modified value based on the read attribute handle

                transformByteArray(&receivedDataBLE, &trandformedReceivedDataBLE);  // The data is transmit in the incorrect format. It as to be transformed.

                chooseSMstateAttributeChanged(dataReceived);    // Choose the correct next state machine state

            break;

            default:
                break;
      
        }
    }
}

