#ifndef _TIMER_H
#define _TIMER_H
#include "build-in.h"
#include "BS86C08C.H"
#include <stdint.h>


typedef enum {
    PTM_CLK_FSYS_DIV4 = 0, // 000: fSYS / 4
    PTM_CLK_FSYS      = 1, // 001: fSYS
    PTM_CLK_FH_DIV16  = 2, // 010: fH / 16
    PTM_CLK_FH_DIV64  = 3, // 011: fH / 64
    PTM_CLK_FSUB_1    = 4, // 100: fSUB (32kHz)
    PTM_CLK_FSUB_2    = 5, // 101: fSUB (32kHz)
    PTM_CLK_EXT_RISE  = 6, // 110: PTCK0 rising edge
    PTM_CLK_EXT_FALL  = 7  // 111: PTCK0 falling edge
} ptm_clock_t;

/* Cạnh kích hoạt cho chế độ Input Capture: PT0IO[1:0] */
typedef enum {
    CAPTURE_EDGE_RISING  = 0, // 00: Cạnh lên
    CAPTURE_EDGE_FALLING = 1, // 01: Cạnh xuống
    CAPTURE_EDGE_BOTH    = 2, // 10: Cả hai cạnh
    CAPTURE_DISABLED     = 3  // 11: Tắt capture
} ptm_capture_edge_t;

/* Nguồn chân kích hoạt Input Capture: PT0CAPTS */
typedef enum {
    CAPTURE_SRC_PTPOI = 0,    // 0: Từ chân PTPOI (PA4)
    CAPTURE_SRC_PTCK0 = 1     // 1: Từ chân PTCK0 (PA7)
} ptm_capture_src_t;

/* --- Các hàm điều khiển chung --- */
void PTM0_Start(void);
void PTM0_Stop(void);
void PTM0_Pause(void);
void PTM0_Resume(void);
void PTM0_WriteCCRA(uint16_t value);
void PTM0_WriteCCRP(uint16_t value);
uint16_t PTM0_ReadCCRA(void);
uint16_t PTM0_ReadCounter(void);

/* --- 1. Chế độ Timer / Counter --- */
void PTM0_Config_TimerCounter(ptm_clock_t clk, uint16_t compare_period);

/* --- 2. Chế độ PWM Output --- */
void PTM0_Config_PWM(ptm_clock_t clk, uint16_t period, uint16_t duty, unsigned char active_high);
void PTM0_SetPWM_Duty(uint16_t duty);

/* --- 3. Chế độ Single Pulse Output --- */
void PTM0_Config_SinglePulse(ptm_clock_t clk, uint16_t pulse_width, unsigned char active_high);
void PTM0_Trigger_SinglePulse(void);

/* --- 4. Chế độ Capture Input --- */
void PTM0_Config_Capture(ptm_clock_t clk, ptm_capture_src_t src, ptm_capture_edge_t edge);
unsigned char PTM0_IsCaptureOccurred(void);
void PTM0_ClearCaptureFlag(void);
uint16_t PTM0_GetCapturedValue(void);

#endif