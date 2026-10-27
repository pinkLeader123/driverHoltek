#ifndef _ISR_H_
#define _ISR_H_

#include "BS86C08C.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Danh sách địa chỉ Vector ngắt chuẩn của BS86C08C */
#define VECTOR_EXT_INT          0x04
#define VECTOR_TOUCH_KEY        0x08
#define VECTOR_TIMEBASE0        0x0C
#define VECTOR_CTM0_COMP_P      0x10
#define VECTOR_PTM0_COMP_A      0x14
#define VECTOR_UART0            0x18
#define VECTOR_LVD              0x1C
#define VECTOR_EEPROM           0x20
#define VECTOR_I2C              0x24
#define VECTOR_ADC              0x28
#define VECTOR_CTM0_COMP_A      0x2C
#define VECTOR_PTM0_COMP_P      0x30

/* Kiểu trigger cho External Interrupt (INT pin PA4) */
typedef enum {
    EXT_INT_TRIGGER_DISABLE = 0,
    EXT_INT_TRIGGER_RISING  = 1,
    EXT_INT_TRIGGER_FALLING = 2,
    EXT_INT_TRIGGER_BOTH    = 3
} ext_int_trigger_t;

/* Chu kỳ cho Time Base 0 (TBC register) */
typedef enum {
    TB0_PERIOD_2_8  = 0, // 2^8  clocks
    TB0_PERIOD_2_9  = 1, // 2^9  clocks
    TB0_PERIOD_2_10 = 2, // 2^10 clocks
    TB0_PERIOD_2_11 = 3, // 2^11 clocks
    TB0_PERIOD_2_12 = 4, // 2^12 clocks
    TB0_PERIOD_2_13 = 5, // 2^13 clocks
    TB0_PERIOD_2_14 = 6, // 2^14 clocks
    TB0_PERIOD_2_15 = 7  // 2^15 clocks
} tb0_period_t;

/* Quản lý ngắt toàn cục */
void ISR_GlobalEnable(void);
void ISR_GlobalDisable(void);

/* Quản lý External Interrupt (INT pin PA4) */
void ISR_ExtInt_Init(ext_int_trigger_t trigger);
void ISR_ExtInt_Enable(void);
void ISR_ExtInt_Disable(void);

/* Quản lý Time Base 0 */
void ISR_TimeBase0_Init(tb0_period_t period);
void ISR_TimeBase0_Enable(void);
void ISR_TimeBase0_Disable(void);

/* Quản lý cờ ngắt phần mềm (Event Flags) thay thế Callbacks */
unsigned char ISR_GetAndClear_ExtIntFlag(void);
unsigned char ISR_GetAndClear_TimeBase0Flag(void);
unsigned char ISR_GetAndClear_PTM0AFlag(void);
unsigned char ISR_GetAndClear_UART0Flag(void);
unsigned char ISR_GetAndClear_LVDFlag(void);
unsigned char ISR_GetAndClear_ADCFlag(void);

#ifdef __cplusplus
}
#endif

#endif /* _ISR_H_ */