#include "watchdog.h"


void WDT_Feed(void) {
    #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
        _clrwdt();
    #else
        __asm__("clr wdt");
    #endif
}

void WDT_SoftwareReset(void) {
    _wdtc = 0x00;
    while(1);
}

void WDT_Init(wdt_timeout_t timeout) {
    _wdtc = (unsigned char)(WDT_KEY_ENABLE | (timeout & 0x07));
    WDT_Feed();
}