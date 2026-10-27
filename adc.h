#ifndef _ADC_H_
#define _ADC_H_
#include "build-in.h"
#include "BS86C08C.h"

typedef enum {
    ADC_CH0 = 0,
    ADC_CH1,
    ADC_CH2,
    ADC_CH3,
    ADC_CH4,
    ADC_CH5,
    ADC_CH6,
    ADC_CH7
} adc_channel_t;

void ADC_Init(void);
unsigned short ADC_ReadChannel(adc_channel_t ch);

#endif