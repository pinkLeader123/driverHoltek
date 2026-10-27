#include "delay.h"

void Delay_ms(unsigned int ms) {
    // Delay software phụ thuộc fSYS
    volatile unsigned int i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < 300; j++) {
            #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
                _nop();
            #else
                __asm__("nop");
            #endif
        }
    }
}