#include "twn4.sys.h"

#ifndef VERSION
  #define VERSION               0x0100
#endif
#ifndef APPCHARS
  #define APPCHARS              APP
#endif

#define APP_MAGIC_V1            0x49A31CF1
#define APP_MAGIC_V2            0x49A31CF2

const unsigned char AppManifest[] __attribute__((weak));

const unsigned char AppManifest[] =
{
    OPEN_PORTS, 1, OPEN_PORT_USB_MSK | OPEN_PORT_COM1_MSK | OPEN_PORT_COM2_MSK,
    TLV_END
};

typedef struct __attribute__((__packed__))
{
    unsigned long           Magic;				// 4
    unsigned long           Crc;				// 4
    unsigned long           Stack;				// 4
    unsigned long           ResetHandler;		// 4
    unsigned short          DeviceType;			// 2
    char                    AppChars[4];		// 4
    unsigned short          AppVersion;			// 2
    const unsigned char *   Manifest;			// 4
    unsigned char           Sign[20];			// Offset = 28
} TAppHeader;

// Data defined by the linker script
extern unsigned long _stext;
extern unsigned long _etext;
extern unsigned long _sdata;
extern unsigned long _edata;
extern unsigned long _sbss;
extern unsigned long _ebss;
extern unsigned long _estack;

void ResetHandler(void);
void _AppToolsInit(void);
int main(void);

#define STR_VALUE(arg)  STR_VALUE2(arg) 
#define STR_VALUE2(arg) #arg 

__attribute__ ((section(".header"), used))
const TAppHeader AppHeader = 
    {
    	APP_MAGIC_V2,
    	0xFFFFFFFF,
    	(unsigned long)&_estack,
    	(unsigned long)ResetHandler,
    	128,
    	STR_VALUE(APPCHARS),
    	VERSION,
    	AppManifest,
    	{
    		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
    	}
    };

void ResetHandler (void)
{
    unsigned long *pSrc;
    unsigned long *pDest;
   
    pSrc  = &_etext;
    pDest = &_sdata;
    while(pDest < &_edata)
        *pDest++ = *pSrc++;
   
    pDest = &_sbss;
    while(pDest < &_ebss)
        *pDest++ = 0;

	_AppToolsInit();
	
    main();
    
    while (1)
        ;
}
