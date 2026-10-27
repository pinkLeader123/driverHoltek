#ifndef _LVD_H_
#define _LVD_H_

#include "BS86C08C.H"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Định nghĩa 8 mức điện áp phát hiện của LVD (VLVD[2:0])
 */
typedef enum {
    LVD_VOLT_2_0V = 0, // 000: 2.0V
    LVD_VOLT_2_2V = 1, // 001: 2.2V
    LVD_VOLT_2_4V = 2, // 010: 2.4V
    LVD_VOLT_2_7V = 3, // 011: 2.7V
    LVD_VOLT_3_0V = 4, // 100: 3.0V
    LVD_VOLT_3_3V = 5, // 101: 3.3V
    LVD_VOLT_3_6V = 6, // 110: 3.6V
    LVD_VOLT_4_0V = 7  // 111: 4.0V
} lvd_volt_t;

/* Khởi tạo LVD */
void LVD_Init(lvd_volt_t voltage_threshold, unsigned char enable_bandgap);

/* Điều khiển bật / tắt */
void LVD_Enable(void);
void LVD_Disable(void);

/* Thay đổi ngưỡng điện áp phát hiện */
void LVD_SetVoltage(lvd_volt_t voltage_threshold);

/* Kiểm tra trạng thái điện áp nguồn */
unsigned char LVD_IsLowVoltage(void);

/* Điều khiển ngắt LVD */
void LVD_EnableInterrupt(void);
void LVD_DisableInterrupt(void);
void LVD_ClearInterruptFlag(void);

#ifdef __cplusplus
}
#endif

#endif /* _LVD_H_ */