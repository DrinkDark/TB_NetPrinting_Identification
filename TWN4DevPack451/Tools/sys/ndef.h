#ifndef __NDEF_H__
#define __NDEF_H__

#include "apptools.h"

typedef struct __attribute__((__packed__))
{
    byte* Buffer;
    int Length;
    byte TypeLength;
    byte IDLength;
    int PayloadLength;
    int TypeOffset;
    int IDOffset;
    int PayloadOffset;
} NDEFRecord;

void NDEF_ParseRecord(byte* Buffer, NDEFRecord* record);
void NDEF_CreateRecord(byte TNF, bool MessageBegin, bool MessageEnd, bool ShortRecord, byte* Type, byte TypeLength, byte* ID, byte IDLength, byte* Payload, int PayloadLength, NDEFRecord* record);

#endif
