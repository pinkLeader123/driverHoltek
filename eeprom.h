#ifndef _EEPROM_H_
#define _EEPROM_H_
#include "build-in.h"
#include "BS86C08C.H"

#define EEC_ADDR_SECTOR1  0x40
#define EEC_WREN_BIT      (1U << 3)

unsigned char EEPROM_ReadByte(unsigned char addr);
void EEPROM_WriteByte(unsigned char addr, unsigned char data);
void EEPROM_WriteProtectEnable(void);
void EEPROM_WriteProtectDisable(void);
void EEPROM_WriteInterruptEnable(void);
void EEPROM_WriteInterruptDisable(void);
void EEPROM_ClearInterruptFlag(void);

#endif