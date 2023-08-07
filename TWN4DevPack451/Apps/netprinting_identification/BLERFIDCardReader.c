//////////////////////////////////////////////////////////////////////////////////
//                          RFID/BLE CARD READER FIRMWARE
//                        
//
// - Read MIFARE card and print ID
// - Authenticate and identify via BLE
//      o Advertise
//      o Connect
//      o Authentify himself and phone via double authentication using random 16 bytes numbers
//      o Print ID
//      o Disconnect from device
//////////////////////////////////////////////////////////////////////////////////

#include "twn4.sys.h"
#include "apptools.h"

//////////////////////////////////////////////////////////////////////////////////////
//                                DEFINE CONSTANT
//////////////////////////////////////////////////////////////////////////////////////

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

#define LENGTH_8_BYTES			8       // 8 bytes length
#define LENGTH_16_BYTES			16      // 16 bytes length
#define LENGTH_32_BYTES			32      // 32 bytes length
#define LENGTH_64_BYTES			64      // 64 bytes length


//////////////////////////////////////////////////////////////////////////////////////
//                                DEFINE VARIABLES
//////////////////////////////////////////////////////////////////////////////////////

//--------------------------  STATE MACHINE VARIABLES  -------------------------------

// Enumeration for states of the state machine
enum States {
    ST_OnIdle,                  // Wait a connected device
    ST_WaitAppRandNum,          // Wait the application random number
    ST_DeviceAuthentication,    // Device (card reader) authentication is proceeding
    ST_WaitDeviceAuthenticated, // The Device is authenticated
    ST_AppAuthentication,       // App authentication is proceeding
    ST_WaitAppAuthentication,   // Wait the app's response for the app authentication
    ST_AppAuthenticated,        // The app is authenticated
    ST_WaitIdentification,      // Authentication protocole succeeded and finished
    ST_Identification,          // App identifies himself (transmits ID)
    ST_AuthenticationFailed     // A error occurred during the authentication process
} currentState;

uint64_t readerCurrentTime = 1690495200;     // Current time in Unix format (seconds since 1 januar 1970)

uint32_t elapsedTime = 0;
uint32_t lastSysTicks = 0;


//-----------------------------  CRYPTO VARIABLES  -----------------------------------

const byte aesKey[] = {0xbf, 0xc1, 0xc1, 0x8b, 0x3c, 0x60, 0x50, 
    0x2a, 0x4f, 0x08, 0xdf, 0xb6, 0xe0, 0xd9, 0xd1, 0x1f};          // 128 bits AES shared key 


byte encryptedData[LENGTH_16_BYTES];
byte decryptedData[LENGTH_16_BYTES];

byte randNum[LENGTH_16_BYTES];



//-------------------------------  CARD VARIABLES  -----------------------------------

const byte Params[] = {SUPPORT_CONFIGCARD, 1, CONFIGENABLED, TLV_END};      // Array that contain parameters 
char OldCardString[MAXCARDSTRINGLEN+1];                                     // Array with the old card string

int TagType;
int IDBitCnt;
byte ID[LENGTH_32_BYTES];


//------------------------------  BLE VARIABLES  -------------------------------------

// BLE parameters
TBLEConfig BLEConfig =  {
    .ConnectTimeout = 12000,   // Timout of an established connection in milliseconds
    .Power = 20,               // TX power : 0 to 80 (0.0dBm to 8.0dBm)
    .BondableMode = 0x00,      // Bonding : 0 = off, 1 = on
    .AdvInterval = 200,        // Advertisement interval : values 20ms to 10240ms
    .ChannelMap = 0x07,        // Advertisement Bluetooth channels : 7 = CH37 + CH38 + CH39
    .DiscoverMode = 0x02,      // Discoverable Mode : 2 = LE_GAP_GENERAL_DISCOVERABLE
    .ConnectMode = 0x02,       // Connectable mode : 2 = LE_GAP_CONNECTABLE_SCANNABLE
    .SecurityFlags = 0x00,     // Security requirement bitmask
    .IOCapabilities = 0x04,    // Security Management related I/O capabilities : 4 = keyboard / display
    .Passkey = 0x00000000,     // Passkey if security is configured
}; 

int attrHandle;
int attrStatusFlag;
int attrConfigFlag;

byte receivedDataBLE32[LENGTH_32_BYTES];
byte receivedDataBLE64[LENGTH_64_BYTES];

int receivedDataBLELength;

byte transformedReceivedDataBLE16[LENGTH_16_BYTES];
byte transformedReceivedDataBLE32[LENGTH_32_BYTES];

bool receivedDataLength64 = false;          // The received data lenth is 64 bytes, else 32 bytes

bool BLEDeviceConnected = false;            // A BLE device is connected


//////////////////////////////////////////////////////////////////////////////////////
//                                    FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////

/**
 * Startup fonction for the card reader
 * 
 * Function called at startup to initialize the reader
 * - Set the leds to green and emit a low and high beeps
 * 
*/
void OnStartup(void)
{
    //--------------------------------  CARD INIT  ---------------------------------------

	SetParameters(Params,sizeof(Params));

	SetTagTypes(0, HFTAGTYPES);
	
    OldCardString[0] = 0;

    //---------------------------------  BLE INIT  ---------------------------------------

    BLEPresetConfig(&BLEConfig);

    BLEInit(BLE_MODE_CUSTOM);   // BLE_MODE_CUSTOM use BLEPresetConfig param

    //--------------------------------  CRYPTO INIT  -------------------------------------

    Crypto_Init(CRYPTO_ENV0, CRYPTOMODE_CBC_AES128, &aesKey, sizeof(aesKey));   // Enable encryption initialisation with CRYPTO_ENV0 for init vector, CBC-AES128 encryption and the key


    currentState = ST_OnIdle;

    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    SetVolume(50);
    BeepLow();
    BeepHigh();
}

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
void deviceConnected(void) {
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

    currentState = ST_WaitAppRandNum;

    StartTimer(10000);  // Set the disconnect device timeout to 10s for the BLE
}

/**
 * Device disconnected
 * 
 * Callback function called when a BLE device is disconnected
 * - Set leds and beeps
 * 
*/
void deviceDisconnected(void) {
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
 * @param sourceArray pointer to the array to transform
 * @param arraySize size of the array to merge
 * @param destinationArray pointer to the transformed array
 * 
*/
void transformByteArray(byte* sourceArray, int arraySize, byte* destinationArray){   
    for (int i = 0; i < arraySize; i += 2) {
        char high = (sourceArray)[i];
        char low = (sourceArray)[i + 1];

        high = ScanHexChar(high);
        low = ScanHexChar(low);

        destinationArray[i / 2] = ((high << 4) | (low & 0x0F));
    }
}

/**
 * Generate a random number 
 * 
 * Function to generate a random 16 bytes number of the argument size
 * 
 * @param randNum pointer to the random number
 * 
*/
void generateRandNum(byte* randNum){
    // Set seed for random number based on the system ticks
    // Can produce different sequences of random numbers each time the function is called, as long as the system ticks are different.
    srand(GetSysTicks());

    for (int i = 0; i < LENGTH_16_BYTES; i++) {
        randNum[i] = rand() % 256;      // % 256 ensures that the random number is within the range of 0 to 255 (one byte)
    }
}

/**
 * Get specific bytes
 * 
 * @param sourceArray pointer to the source array
 * @param startIndex index to start
 * @param endIndex index to end
 * @param destinationArray pointer to the destination array
 * 
*/
void getBytes(byte* sourceArray, int startIndex, int endIndex, byte* destinationArray) {
    for (int i = 0; i < (endIndex - startIndex + 1); i++) {
        destinationArray[i] = sourceArray[startIndex + i];
    }
}

/**
 * Convert from byte array to uint64_t
 * 
 * @param sourceArray pointer to the source array
 * @param arraySize size of the source array
 * 
 * @return result value convert in uint64_t
*/
uint64_t byteArrayToUint64_t(byte* sourceArray, int arraySize) {
    uint64_t result = 0;

    for (int i = 0 ; i < arraySize ; i++ ){
        result = (result << 8) | sourceArray[i];
    }
    return result;
}

/**
 * Update time function
 * 
 * If the elapsed sysTicks are greater than 1000 (> 1 second), the readerCurrentTime is incremented (the minimal unit in unix time format is a seconde)
*/
void updateTime(void) {
    uint64_t sysTicks = (uint64_t) GetSysTicks();

    // GetSysTicks() return a value who will restart at 0 after 2^32 system ticks
    // Manage the case when the last sysTicks is almost at the max and the new has restart
    if(sysTicks > lastSysTicks) {
        elapsedTime += (sysTicks - lastSysTicks);
    } else {
        elapsedTime += ((4294967296 - lastSysTicks) + sysTicks);
    }
    lastSysTicks = sysTicks;

    // Update time only every second minimum (minimal time unit)
    if (elapsedTime >= 1000)
    {
        readerCurrentTime = (elapsedTime / 1000);     // Add elapsed seconds
        elapsedTime = elapsedTime % 1000;                    // Store the remaining time (when less than a second remaining)
    } 
}

/**
 * Scanning for a card
 * 
 * Search a card, read his value and print his value if the a BLE device is not connected
*/
void scanCard(void) {
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
}

/**
 * Verify timeout
 * 
 * Control if the timer is used for the BLE or for a card
*/
void verifyTimeout(void) {
    // 
    if (TestTimer())
    {
        // Control if timer used for card or BLE
        if(BLEDeviceConnected) {
            currentState = ST_AuthenticationFailed;
        } else {
            OnCardTimeout(OldCardString);
            OldCardString[0] = 0;
        }
    }
}

/**
 *
 * Choose the state machine state
 * 
*/
void chooseSMstate(void) {
    if(BLEDeviceConnected) {
        switch(currentState) {

            // -------------------------------------------------------------------------------------
            // Start the device authentication
            //
            // Called when a device is connected and write a random number in a charachteristic
            // Encrypt the data if correct length and send them to the device via a notification 
            // -------------------------------------------------------------------------------------
            case ST_DeviceAuthentication:
                //HostWriteString("DeviceAuthentication");
                //HostWriteString("\r");

                Encrypt(CRYPTO_ENV0, (const) &transformedReceivedDataBLE16, &encryptedData, sizeof(encryptedData));
                CBC_ResetInitVector(CRYPTO_ENV0);

                BLESetGattServerAttributeValue(attrHandle, 0, &encryptedData, sizeof(encryptedData));       // Write the encrypt data in the attribute and send a notification to the device

                currentState = ST_WaitDeviceAuthenticated;
                
            break;

            // -------------------------------------------------------------------------------------
            // App being authenticated
            //
            // Called when the device has been authenticated
            // Send a random number to the device via a notification 
            // -------------------------------------------------------------------------------------
            case ST_AppAuthentication:
                //HostWriteString("AppAuthentication");
                //HostWriteString("\r");

                generateRandNum(&randNum);

                BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));   // Write the random number in the attribute and send a notification to the device

                currentState = ST_WaitAppAuthentication;

                break;

            // -------------------------------------------------------------------------------------
            // App authenticated
            //
            // Called when the app has return the encrypt random number
            // Decrypt and compare to received data to the send random number
            // -------------------------------------------------------------------------------------
            case ST_AppAuthenticated:
                //HostWriteString("AppAuthenticated");
                //HostWriteString("\r");

                Decrypt(CRYPTO_ENV0, (const) &transformedReceivedDataBLE16, &decryptedData, sizeof(decryptedData));

                // The Decrypt function return the data in the incorrect format. It as to be transformed. 
                //byte transformedDecryptedData[BLE_DATA_SIZE];
                //transformByteArray(&decryptedData, &transformedDecryptedData);
                
                // Compare the received decrypt data with the send random number
                if (memcmp(&decryptedData, &randNum, sizeof(decryptedData)) == 0) {    
                
                    // Write a random number in the attribute and send a notification to the device
                    // to sigifie the the success of the authentication procedure
                    generateRandNum(&randNum);                                            
                    BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));

                    receivedDataLength64 = true;

                    currentState = ST_WaitIdentification;
                } else {
                    currentState = ST_AuthenticationFailed;
                }

                break;

            // -------------------------------------------------------------------------------------
            // Identification procedure
            //
            // Called when the app is authenticate and it has send is signed message
            // Control the current time and the exiration time and output the ID
            // -------------------------------------------------------------------------------------
            case ST_Identification:
            {
                //HostWriteString("Identification");
                //HostWriteString("\r");

                //Get user ID from the signed message
                byte userID[16];
                memcpy(userID, receivedDataBLE64, 16);

                // Need to read the curent and eypiration time in the format 0x11, 0x22, ... instead of 0x1, 0x1, 0x2, 0x2, ...
                transformByteArray(&receivedDataBLE64, sizeof(receivedDataBLE64), &transformedReceivedDataBLE32);

                // Get current time from the signed message (bytes 8 to 15)
                byte messageCurrentTime[8];
                getBytes(&transformedReceivedDataBLE32, 8, 15, &messageCurrentTime);

                // Get expiration time from the signed message (bytes 16 to 23)
                byte messageExpirationTime[8];
                getBytes(&transformedReceivedDataBLE32, 16, 23, &messageExpirationTime);

                // The message's expiration time must be in the future compare to the message's current time 
                // and the reader's current time else signed message is not valid
                if(byteArrayToUint64_t(messageExpirationTime, sizeof(messageExpirationTime)) >= byteArrayToUint64_t(messageCurrentTime, sizeof(messageCurrentTime)) && 
                    byteArrayToUint64_t(messageExpirationTime, sizeof(messageExpirationTime)) >= readerCurrentTime) {

                    // If the reader's current time is in the past compare to the message's current time,
                    // the time from the reader is updated.
                    if(messageCurrentTime > readerCurrentTime) {
                        readerCurrentTime = messageCurrentTime;
                    }

                    // Write userID
                    for (int i = 0; i < 16; i++)
                    {
                        // Didn't write padding bytes
                        if(userID[i] != '0'){
                            HostWriteChar(userID[i]);
                        }
                        
                    }
                    HostWriteString("\r");

                    // Write a random number in the attribute to signify the succeed of the authentication procedure
                    generateRandNum(&randNum);
                    BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));

                    receivedDataLength64 = false;

                    currentState = ST_OnIdle;
                } else {
                    currentState = ST_AuthenticationFailed;
                }
                break;
            }
                

            // -------------------------------------------------------------------------------------
            // Authentification failed
            //
            // Called when a error occurred in the authentication process 
            // Overwrite the value in the attribute with a dumb value and disconnect from device
            // -------------------------------------------------------------------------------------
            case ST_AuthenticationFailed:
                //HostWriteString("AuthenticationFailed");
                //HostWriteString("\r");
                
                // Write a dumb value in the attribute to overwrite the make disappear the ID
                attrHandle -= (int)(0b1000000000000000);     // bit 15 of the attribute handle to 0 -> write without notification 
                generateRandNum(&randNum);
                BLESetGattServerAttributeValue(attrHandle, 0, &randNum, sizeof(randNum));

                BLEDisconnectFromDevice();
                deviceDisconnected();       // Call callback (normally called by the BLECheckEvent)
                BLEInit(BLE_MODE_CUSTOM);   // Init BLE to ensure that BLE is working properly on the next connection

                currentState = ST_OnIdle;
                break;

            default:
                break;
        }
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
        case ST_WaitAppRandNum:
            //HostWriteString("WaitAuthentication");
            //HostWriteString("\r");

            if(dataReceived){
                currentState = ST_DeviceAuthentication;
            } else {
                currentState = ST_AuthenticationFailed;
            }

            break;

        // -------------------------------------------------------------------------------------
        // Device authenticated
        //
        // Called when the device is authenticated
        // -------------------------------------------------------------------------------------
        case ST_WaitDeviceAuthenticated:
            //HostWriteString("DeviceAuthenticated");
            //HostWriteString("\r");

            if(dataReceived){
                currentState = ST_AppAuthentication;
            } else {
                currentState = ST_AuthenticationFailed;
            }

            break;

        // -------------------------------------------------------------------------------------
        // Wait app authentication
        //
        // Called when the app return the encrypt random number
        // -------------------------------------------------------------------------------------
        case ST_WaitAppAuthentication:
            //HostWriteString("WaitAppAuthentication");
            //HostWriteString("\r");

            if(dataReceived){
                currentState = ST_AppAuthenticated;
            } else {
                currentState = ST_AuthenticationFailed;
            }
            break;

        // -------------------------------------------------------------------------------------
        // Wait identification
        //
        // Called when both the device and the app are authenticated
        // -------------------------------------------------------------------------------------
        case ST_WaitIdentification:
            //HostWriteString("Authenticated");
            //HostWriteString("\r");

            if(dataReceived){
                currentState = ST_Identification;
            } else {
                currentState = ST_AuthenticationFailed;
            }
            break;

        default:
            break;
    }
}

/**
 * Check the actual BLE event in the module
 * 
 * Only the needed case are implemented. 
*/
void checkBLEEvent(void) {
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

            bool dataReceived = false;

            //Read the modified 32 or 64 bytes value based on the read attribute handle
            if(receivedDataLength64) {
                dataReceived = BLEGetGattServerAttributeValue(attrHandle, &receivedDataBLE64, &receivedDataBLELength, sizeof(receivedDataBLE64));
            } else {
                dataReceived = BLEGetGattServerAttributeValue(attrHandle, &receivedDataBLE32, &receivedDataBLELength, sizeof(receivedDataBLE32));  
                transformByteArray(&receivedDataBLE32, sizeof(receivedDataBLE32), &transformedReceivedDataBLE16);       // The data is transmit in the incorrect format. It as to be transformed.
            }

            chooseSMstateAttributeChanged(dataReceived);    // Choose the correct next state machine state

            break;

        default:
            break;
    }
}


int main(void)
{
	OnStartup();    	

    while (true)
    {
        updateTime();
        verifyTimeout();
        scanCard();   
        chooseSMstate();
        checkBLEEvent();
    }
}

