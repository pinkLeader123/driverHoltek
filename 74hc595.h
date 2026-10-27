#ifndef _74HC595_H_
#define _74HC595_H_

#include "gpio.h"
#include "stdint.h"

#define HC595_DS_PORT PORT_C
#define HC595_DS_PIN  3
#define HC595_SHCP_PORT PORT_C
#define HC595_SHCP_PIN 1
#define HC595_STCP_PORT PORT_C
#define HC595_STCP_PIN 2

void hc595_init(void);
void hc595_write(uint8_t data);
void HC595_ShiftOut(uint8_t data); 
void HC595_Latch(void); 
void HC595_SendByte(uint8_t data); 
void HC595_SendMultipleBytes(uint8_t *pData, uint8_t size); 


#endif