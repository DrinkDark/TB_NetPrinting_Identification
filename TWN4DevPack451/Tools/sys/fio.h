#ifndef __FIO_H__
#define __FIO_H__

// ******************************************************************
// ****** Useful text input functions *******************************
// ******************************************************************

bool TestByte(int Channel);
bool TestChar(int Channel);
char ReadChar(int Channel);

// ******************************************************************
// ****** Useful text output functions ******************************
// ******************************************************************

void WriteChar(int Channel,char Char);
void WriteByteHex(int Channel,byte Byte);
void WriteUInt16Hex(int Channel,uint16_t UInt16);
void WriteUInt32Hex(int Channel,uint32_t UInt32);
void WriteUInt64Hex(int Channel,uint64_t UInt64);

void WriteUIntSignRadixWidth(int Channel,uint32_t UInt32,int Sign,int Radix,int Width);
void WriteUIntDecW(int Channel,uint32_t UInt32,int Width);
void WriteIntDecW(int Channel,int32_t Int32,int Width);
void WriteUIntDec(int Channel,uint32_t UInt32);
void WriteIntDec(int Channel,int32_t Int32);
void WriteUInt64SignRadixWidth(int Channel,uint64_t UInt64,int Sign,int Radix,int Width);
void WriteUInt64DecW(int Channel,uint64_t UInt64,int Width);
void WriteInt64DecW(int Channel,int64_t Int64,int Width);
void WriteUInt64Dec(int Channel,uint64_t UInt64);
void WriteInt64Dec(int Channel,int64_t Int64);

//*******************************************************************
//****** String Output Functions ************************************
//*******************************************************************

void WriteString(int Channel,const char *String);
void WriteLF(int Channel);
void WriteStringLF(int Channel,const char *String);
void WriteRadix(int Channel,const byte *ID,int BitCnt,int DigitCnt,int Radix);
void WriteBin(int Channel,const byte *ID,int BitCnt,int DigitCnt);
void WriteDec(int Channel,const byte *ID,int BitCnt,int DigitCnt);
void WriteHex(int Channel,const byte *ID,int BitCnt,int DigitCnt);
void WriteVersion(int Channel);

//*******************************************************************
//****** Dump Functions *********************************************
//*******************************************************************

void WriteBytesHex(int Channel,const void *Data,int Length);
void WriteDataLine(int Channel,const char *What,const void *Data,int Length);
void WriteDataLineLF(int Channel,const char *What,const void *Data,int Length);
void WriteDataBlock(int Channel,const char *What,const void *Data,int Length,int Width);
void WriteMemory(int Channel,const char *What,const void *Data,int Length,int Width);

// ******************************************************************
// ****** Formated Output *******************************************
// ******************************************************************

int sprintf(char *Buffer,const char *Format,...);
int cprintf(int Channel,const char *Format,...);
int hprintf(const char *Format,...);

// ******************************************************************
// ****** Host Functions ********************************************
// ******************************************************************

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