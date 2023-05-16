#ifndef __APPTOOLS_H__
#define __APPTOOLS_H__

#include "fio.h"

// ******************************************************************
// ****** Timer Functions *******************************************
// ******************************************************************

void StartTimer(unsigned long Duration);
void StopTimer(void);
bool TestTimer(void);

// ******************************************************************
// ****** Delay Function ********************************************
// ******************************************************************

void Delay(unsigned long Duration);

// ******************************************************************
// ****** Tools *****************************************************
// ******************************************************************

char Nibble2HexChar(byte Nibble);

// ******************************************************************
// ****** Beep Functions ********************************************
// ******************************************************************

void SetVolume(int NewVolume);
int GetVolume(void);
void BeepLow(void);
void BeepHigh(void);

// ******************************************************************
// ****** Compatibility to TWN3 *************************************
// ******************************************************************

int ConvertTagTypeToTWN3(int TagTypeTWN4);

// ******************************************************************
// ****** LEGIC Mini-API ********************************************
// ******************************************************************

bool LEGIC_Idle(void);
bool LEGIC_SearchTXP(int TagType,byte *ID,int *IDLength);
bool LEGIC_Select(int TagType);
bool LEGIC_ReadTagTypeFamily(int TagType,byte *Type,byte *Family);
bool LEGIC_SearchSegmentByStamp(const byte* Stamp,int StampLength);
bool LEGIC_SearchSegmentByIndex(int Index,byte *Stamp,int *StampLength,int *SegmentLength);
bool LEGIC_ReadSegment(int Offset,int ByteCnt,byte *Data);

// ******************************************************************
// ****** NFC *******************************************************
// ******************************************************************

typedef struct
{
    byte* Header;
    byte TypeLength;
    int PayloadLength;
    byte IDLength;
    byte* Type;
    byte* ID;
    byte* Payload;
    byte* NextRecord;
} TNDEFRecord;

#define NDEF_IsMBSet(Header)	(Header & 0x80)
#define NDEF_IsMESet(Header)	(Header & 0x40)
#define NDEF_IsCFSet(Header)	(Header & 0x20)
#define NDEF_IsSRSet(Header)	(Header & 0x10)
#define NDEF_IsILSet(Header)	(Header & 0x08)
#define NDEF_GetTNF(Header)		(Header & 0x07)

bool NDEF_WaitConnect(void);
bool NDEF_ReceiveRecord(byte *RecordType,int *RecordTypeLength,int MaxRecordTypeLength,
                        byte *RecordData,int *RecordDataLength,int MaxRecordDataLength);

// ******************************************************************
// ****** BLE *******************************************************
// ******************************************************************

#define LEN_MOBILE_ID		16

const char *BLEGetEventString(int Event);
void BLECalcWalletKey(const byte *Source,byte *Dest);

// ******************************************************************
// ****** LED Compatibility Functions *******************************
// ******************************************************************

void CompLEDInit(int LEDs);
void CompLEDOn(int LEDs);
void CompLEDOff(int LEDs);
void CompLEDToggle(int LEDs);
void CompLEDBlink(int LEDs,int TimeOn,int TimeOff);

// ******************************************************************
// ****** Host Functions ********************************************
// ******************************************************************

#ifndef __FIO_H__

bool HostTestByte(void);
byte HostReadByte(void);
bool HostTestChar(void);
char HostReadChar(void);

void HostWriteByte(byte Byte);
void HostWriteChar(char Char);
void HostWriteString(const char *String);
void HostWriteRadix(const byte *ID,int BitCnt,int DigitCnt,int Radix);
void HostWriteBin(const byte *ID,int BitCnt,int DigitCnt);
void HostWriteDec(const byte *ID,int BitCnt,int DigitCnt);
void HostWriteHex(const byte *ID,int BitCnt,int DigitCnt);
void HostWriteVersion(void);

#endif

// ******************************************************************
// ****** ApplePay Helpers ******************************************
// ******************************************************************

void ApplePayApp_WaitRemoved(void);

// ******************************************************************
// ****** AppleVAS **************************************************
// ******************************************************************

bool AppleVAS_ReadCardData(byte *CardData,int *CardDataLen,int MaxCardDataLen);

// ******************************************************************
// ****** Transact **************************************************
// ******************************************************************

bool Transact_ReadCardData(const TTransactKeys *Keys,byte *CardData,int *CardDataLen,int MaxCardDataLen);

// ******************************************************************
// ****** KleverKey *************************************************
// ******************************************************************

void InitKleverKey(void);
bool SearchKleverKey(int *TagType,int *IDBitCount,byte *ID,int MaxIDBytes);

// ******************************************************************
// ****** Safetrust *************************************************
// ******************************************************************

void InitSafetrust(void);
bool SearchSafetrust(int *TagType,int *IDBitCount,byte *ID,int MaxIDBytes);


bool ReadCardDataGoogleSmartTap(byte* PassID, int* PassIDByteCnt, int MaxPassIDByteCnt);

// ******************************************************************
// ****** OSDP ******************************************************
// ******************************************************************

#define OSDP_CARD_DATA_BUFFER_SIZE 128    // Transponder ID cannot be longer than this many bytes

/**
 * @brief Well, kinda survived the ages. Still there
 * @author Glep
 * 
 * @return typedef struct 
 */
typedef struct __attribute__((__packed__))
{
    uint8_t     Compliance_ContactStatus        , Num_ContactStatus     ;
    uint8_t     Compliance_OutputControl        , Num_OutputControl     ;
    uint8_t     Compliance_CardDataFormat       , Num_CardDataFormat    ;
    uint8_t     Compliance_LEDControl           , Num_LEDControl        ;
    uint8_t     Compliance_AudibleOutput        , Num_AudibleOutput     ;
    uint8_t     Compliance_TextOutput           , Num_TextOutput        ;
    uint8_t     Compliance_TimeKeeping          , Num_TimeKeeping       ;
    uint8_t     Compliance_CheckCharSupport     , Num_CheckCharSupport  ;
    uint8_t     Compliance_ComSecurity          , Num_ComSecurity       ;
    uint8_t     Compliance_SmartCardSupport     , Num_SmartCardSupport  ;
    uint8_t     Compliance_Readers              , Num_Readers           ;
    uint8_t     Compliance_Biometrics           , Num_Biometrics        ;
    uint8_t     ModelType                       , CardData_Format       ;
    uint8_t     OSDP_Version;
} TOSDPCapabilities;

/*
 * @brief   Represents the key management options you have to connect to the reader
 * @note    Only one can be configured -- choose wisely!
 * 
 */
typedef enum _TKeyOption
{
    Key_Option_None,                        // Security disabled -- No OSDP message encryption enabled
    Key_Option_Fixed,                       // Default option for the SCBK - there is a preconfigured key and this one is used. No default key is allowed
    Key_Option_Fixed_Default,               // there is a preconfigured key which is used. It is also possible to connect via the default handshake method with SCBK_D
    Key_Option_Fixed_InstallationMode,      // It is possible to connect to the reader with a preconfigured key and by using the default key and then set a new SCBK as often you want
    Key_Option_Fixed_InstallationMode_Once, // It is possible to connect to the reader with a preconfigured key and by using the default key and then set a new SCBK but only once (be careful!)
    Key_Option_Individual,                  // This way the SCBK is calculated by the method described in the OSDP specification-Security Apendix B. Each reader then holds a individual key
    __KEY_OPTION_COUNT_ENUM
} TKeyOption;

/**
 * @brief     Defines the types of dirrerentiation between two color mapping modes via
 *            OSDP. ID-Dominant or Color-Dominant.
 *            ID-Dominant: The ID decides which LED is meant (standard)
 *            Color-Dominant:    Each LED is monochrom. Color Red will always switch on
 *                               the red LED, no matter what ID is sent via OSDP message
 */
typedef enum _TOSDPLEDAddressMode
{
    OSDP_LED_ADDRESS_MODE_ID = 0,
    OSDP_LED_ADDRESS_MODE_COLOR = 1
} TOSDPLEDAddressMode;

/**
 * @brief Represents available color options for led configuration
 * 
 */
typedef enum
{
  OSDP_COLOR_OPTION_BLACK   = 0,
  OSDP_COLOR_OPTION_RED     = 1,
  OSDP_COLOR_OPTION_GREEN   = 2,
  OSDP_COLOR_OPTION_AMBER   = 3,
  OSDP_COLOR_OPTION_BLUE    = 4,
  OSDP_COLOR_OPTION_MAGENTA = 5,
  OSDP_COLOR_OPTION_CYAN    = 6,
  OSDP_COLOR_OPTION_WHITE   = 7,
  OSDP_COLOR_OPTION_RGB     = 8
} TOSDPColorOption;

/**
 * @brief Describes all possible OSDP colors (not all necessarely supported)
 * 
 */
typedef enum _TOSDPLedColor
{
    OSDP_LED_COLOR_BLACK    = 0,
    OSDP_LED_COLOR_RED      = 1,
    OSDP_LED_COLOR_GREEN    = 2,
    OSDP_LED_COLOR_AMBER    = 3,
    OSDP_LED_COLOR_BLUE     = 4,
    OSDP_LED_COLOR_MAGENTA  = 5,
    OSDP_LED_COLOR_CYAN     = 6,
    OSDP_LED_COLOR_WHITE    = 7
} TOSDPLedColor;

/**
 * @brief This holds the LED mapping informations coming from AppBlaster
 *
 */
typedef struct __attribute__((__packed__)) _OSDPLEDMapping
{
    byte Color;
    int PhysicalLEDMask;
} TOSDPLEDMapping;

/**
 * @brief Available Tamper Modes for reader
 * 
 */                             
typedef enum __attribute__((__packed__)) _TTamperMode
{
    TAMPER_MODE_OFF         = 0,
    TAMPER_MODE_NORMAL      = 1,
    TAMPER_MODE_INVERTED    = 2
} TTamperMode;

/**
 * @brief Special value in case address is read from DIP switch
 * 
 */
#define OSDP_ADDRESS_DIP -1

/**
 * @brief Special value in case baud rate is read from DIP switch
 * 
 */
#define OSDP_BAUDRATE_DIP -1

typedef enum _TOSDPBias
{
    OSDP_BIAS_DIP = -1,
    OSDP_BIAS_OFF =  0,
    OSDP_BIAS_ON  =  1,
} TOSDPBias;

typedef struct __attribute__((__packed__)) _TOSDPCardData
{
    int TagType;
    int IDBitCnt;
    byte ID[OSDP_CARD_DATA_BUFFER_SIZE];
    int RawBitCnt;
    byte RawBits[OSDP_CARD_DATA_BUFFER_SIZE];
    // Additional byte for null termination (which is not transmitted to the host)
    char FormattedString[OSDP_CARD_DATA_BUFFER_SIZE+1];
} TOSDPCardData;

/**
 * @brief Holds all necessary informations for OSDP App
 * 
 */
typedef struct _OSDPConfig
{
    int Address;               
    int Baudrate;
    TOSDPBias Bias;

    uint32_t ConnectionTimeout; // Holds the max time the reader will stay in connected state, when no new valid message is received with this PDs address
    byte ProjectID[4];          // Project ID - ID in which project this reader is grouped
    byte GroupID[4];            // Group ID - ID in which this reader is grouped

    byte SCBK[16];              // Key for OSDP Encryption
    byte SCBK_D[16];            // Default Key for OSDP Encryption - dependent on KeyOption
    byte MasterKey[16];         // Master Key for Diversified Key generation - dependent on KeyOption
    TKeyOption KeyOption;       // Which key management is choosed @KeyOptions
                                                                            // Key_Option_None (security off)
                                                                            // Key_Option_Fixed
                                                                            // Key_Option_Fixed_Default
                                                                            // Key_Option_Fixed_InstallationMode
                                                                            // Key_Option_Fixed_InstallationMode_Once
                                                                            // Key_Option_Individual

    bool FMT;                   // Which message will be prompted, when a transpnder is found RAW/FMT
    int CardTimeout;            // How long to wait after each card starting when the previous card has left the field

    TTamperMode TamperMode;     // Selected Tamper Mode @TTamperMode

    int BuzzerFrequency;
    int BuzzerVolume;

    TOSDPCapabilities Cap;      // Capabilies from current reader

    const TOSDPLEDMapping *pLEDMapping; // Holds the user configuration for reader LEDs
    TOSDPLEDAddressMode LEDMode;

    // Standard search function must be implemented by the app
    bool (*pAppSearchTag)(TOSDPCardData *Card);
    // Standard function for reading card data must be implemented by the app
    bool (*pReadCardData)(TOSDPCardData *Card);

    void (*pOnStartup)(void);           // Function Pointer to subroutine handling startup signaling
    void (*pOnRestart)(void);           // Function Pointer to subroutine handling signaling before restart
    void (*pOnCardInvalid)(void);       // Function Pointer to subroutine handling signaling 
                                        // when a card was found to be INVALID (failed reading
                                        // data from the card)
    void (*pOnCardValid)(void);         // Function Pointer to subroutine handling signaling 
                                        // when a card was found to be VALID (data was read 
                                        // from card successfully)
    void (*pOnCardTimeout)(void);       // Function Pointer to subroutine handling signaling
                                        // when tag block times out
    void (*pOnOnline)(void);            // Function Pointer to subroutine handling signaling
                                        // when OSDP connection is online
    void (*pOnOffline)(void);           // Function Pointer to subroutine handling signaling
                                        // when OSDP connection times out with 8 sec 
    void (*pOnTamperDetect)(void);      // Function Pointer to subroutine handling signaling
                                        // an tamper alarm
    void (*pOnFirmwareUpdate)(void);    // Function Pointer to subroutine handling signaling
                                        // a running firmware update. 
} TOSDPConfig;

bool OSDPAppSetup(const TOSDPConfig *OSDPConfig);
void OSDPAppBackgroundTasks(void);
void OSDPAppForegroundTasks(void);
void OSDPAppTransponderTask(bool (*ReadCardData)(TOSDPCardData *CardData));
void TickTask(void);
void HighPrioTask(void);

bool OSDPLEDSet(int ID, TOSDPLedColor Color);
bool OSDPLEDBlink(int ID, TOSDPLedColor OnColor, TOSDPLedColor OffColor, int OnTime, int OffTime);

void OSDPSetDefaultTone(int Frequency);
void OSDPSetDefaultVolume(int Volume);
void OSDPBeep(int Count, int OnTime, int OffTime);
void OSDPBuzzerOff(void);

#endif
