#include "isr.h"

/* Biến lưu trữ cờ sự kiện thay cho Function Pointer */
static volatile uint8_t s_ext_int_event  = 0;
static volatile uint8_t s_tb0_event      = 0;
static volatile uint8_t s_ptm0a_event    = 0;
static volatile uint8_t s_uart0_event    = 0;
static volatile uint8_t s_lvd_event      = 0;
static volatile uint8_t s_adc_event      = 0;

/* ==========================================================================
 * CÁC HÀM CẤU HÌNH HỆ THỐNG NGẮT
 * ========================================================================== */

void ISR_GlobalEnable(void) {
    _emi = 1; // Global Interrupt Enable trong INTC0
}

void ISR_GlobalDisable(void) {
    _emi = 0;
}

/**
 * @brief Cấu hình ngắt ngoài chân INT (PA4)
 */
void ISR_ExtInt_Init(ext_int_trigger_t trigger) {
    // 1. Cấu hình chân PA4 làm ngõ vào
    _pac4 = 1;
    _papu4 = 1; // Bật điện trở kéo lên nội

    // 2. Cài đặt cạnh kích hoạt trong thanh ghi INTEG
    _ints0 = (trigger & 0x01) ? 1 : 0;
    _ints1 = (trigger & 0x02) ? 1 : 0;

    // 3. Xóa cờ ngắt ban đầu
    _intf = 0;
}

void ISR_ExtInt_Enable(void) {
    _intf = 0;
    _inte = 1; // Cho phép ngắt ngoài
    _emi  = 1;
}

void ISR_ExtInt_Disable(void) {
    _inte = 0;
}

/**
 * @brief Cấu hình ngắt định kỳ Time Base 0
 */
void ISR_TimeBase0_Init(tb0_period_t period) {
    _tb0on = 0; // Tắt trước khi nạp tham số

    // Cài đặt chu kỳ đếm trong thanh ghi TBC
    _tb00 = (period & (1 << 0)) ? 1 : 0;
    _tb01 = (period & (1 << 1)) ? 1 : 0;
    _tb02 = (period & (1 << 2)) ? 1 : 0;

    _tb0f = 0; // Xóa cờ ngắt
}

void ISR_TimeBase0_Enable(void) {
    _tb0f  = 0;
    _tb0on = 1; // Bật khối đếm Time Base 0
    _tb0e  = 1; // Cho phép ngắt Time Base 0
    _emi   = 1;
}

void ISR_TimeBase0_Disable(void) {
    _tb0e  = 0;
    _tb0on = 0;
}

/* ==========================================================================
 * KIỂM TRA VÀ XÓA CỜ SỰ KIỆN TỪ CHƯƠNG TRÌNH CHÍNH (POLLING)
 * ========================================================================== */

unsigned char ISR_GetAndClear_ExtIntFlag(void) {
    if (s_ext_int_event) {
        s_ext_int_event = 0;
        return 1;
    }
    return 0;
}

unsigned char ISR_GetAndClear_TimeBase0Flag(void) {
    if (s_tb0_event) {
        s_tb0_event = 0;
        return 1;
    }
    return 0;
}

unsigned char ISR_GetAndClear_PTM0AFlag(void) {
    if (s_ptm0a_event) {
        s_ptm0a_event = 0;
        return 1;
    }
    return 0;
}

unsigned char ISR_GetAndClear_UART0Flag(void) {
    if (s_uart0_event) {
        s_uart0_event = 0;
        return 1;
    }
    return 0;
}

unsigned char ISR_GetAndClear_LVDFlag(void) {
    if (s_lvd_event) {
        s_lvd_event = 0;
        return 1;
    }
    return 0;
}

unsigned char ISR_GetAndClear_ADCFlag(void) {
    if (s_adc_event) {
        s_adc_event = 0;
        return 1;
    }
    return 0;
}

/* ==========================================================================
 * CÁC TRÌNH PHỤC VỤ NGẮT (ISR) THEO ĐÚNG CÚ PHÁP HOLTEK GCC
 * ========================================================================== */

/* 1. External INT (Vector 0x04) */
DEFINE_ISR(External_Interrupt_ISR, VECTOR_EXT_INT) {
    _intf = 0;              // Bắt buộc: Xóa cờ ngắt phần cứng
    s_ext_int_event = 1;    // Kích hoạt cờ sự kiện cho Task chính
}

/* 2. Time Base 0 (Vector 0x0C) */
DEFINE_ISR(TimeBase0_ISR, VECTOR_TIMEBASE0) {
    _tb0f = 0;              // Bắt buộc: Xóa cờ ngắt phần cứng
    s_tb0_event = 1;        // Kích hoạt cờ sự kiện định kỳ
}

/* 3. PTM0 Compare Match A (Vector 0x14) */
DEFINE_ISR(PTM0_CompareMatchA_ISR, VECTOR_PTM0_COMP_A) {
    _ptm0af = 0;            // Bắt buộc: Xóa cờ ngắt phần cứng
    s_ptm0a_event = 1;
}

/* 4. UART0 Interrupt (Vector 0x18) */
DEFINE_ISR(UART0_ISR, VECTOR_UART0) {
    _ur0f = 0;              // Bắt buộc: Xóa cờ ngắt phần cứng
    s_uart0_event = 1;
}

/* 5. Low Voltage Detector (Vector 0x1C) */
DEFINE_ISR(LVD_ISR, VECTOR_LVD) {
    _lvf = 0;               // Bắt buộc: Xóa cờ ngắt phần cứng
    s_lvd_event = 1;
}

/* 6. ADC Conversion Complete (Vector 0x28) */
DEFINE_ISR(ADC_ISR, VECTOR_ADC) {
    _adf = 0;               // Bắt buộc: Xóa cờ ngắt phần cứng
    s_adc_event = 1;
}


