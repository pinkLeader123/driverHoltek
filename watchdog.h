#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "build-in.h"
#include "BS86C08C.H"

#define WDT_KEY_ENABLE       (0x0Au << 3)
#define WDT_TIMEOUT_MASK     (0x07)

typedef enum {
    WDT_TIME_8MS   = 0, // 2^8  / 32kHz
    WDT_TIME_32MS  = 1, // 2^10 / 32kHz
    WDT_TIME_128MS = 2, // 2^12 / 32kHz
    WDT_TIME_512MS = 3, // 2^14 / 32kHz
    WDT_TIME_1S    = 4, // 2^15 / 32kHz
    WDT_TIME_2S    = 5, // 2^16 / 32kHz
    WDT_TIME_4S    = 6, // 2^17 / 32kHz
    WDT_TIME_8S    = 7  // 2^18 / 32kHz
} wdt_timeout_t;



/* Khởi tạo và nạp thời gian chờ */
void WDT_Init(wdt_timeout_t timeout);
void WDT_Feed(void);
void WDT_SoftwareReset(void);

#endif