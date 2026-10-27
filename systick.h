#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "BS86C08.h"
#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Khởi tạo bộ đếm nhịp hệ thống 1ms (Tương tự SysTick của ARM) */
void SysTick_Init(uint32_t fsys_hz);

/* Trả về thời gian hệ thống tính bằng mili-giây kể từ lúc khởi động */
uint32_t SysTick_GetTick(void);

/* Hàm trễ mili-giây chuẩn xác (Tương đương HAL_Delay trên STM32) */
void SysTick_Delay_ms(uint32_t ms);

/* Hàm trễ micro-giây dựa trên chu kỳ lệnh phần cứng */
void SysTick_Delay_us(uint16_t us);

#ifdef __cplusplus
}
#endif

#endif /* _SYSTICK_H_ */