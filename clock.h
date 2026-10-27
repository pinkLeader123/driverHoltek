#ifndef CLOCK_H
#define CLOCK_H
#include "build-in.h"
#include "BS86C08C.H"

typedef enum {
    SYS_HIRC_8MHZ  = 0,
    SYS_HIRC_12MHZ = 1,
    SYS_HIRC_16MHZ = 2
} sys_hirc_t;

void SYS_Clock_Init(sys_hirc_t freq);
void SYS_Enter_Sleep(void);
void SYS_Enter_Idle(void);


#endif