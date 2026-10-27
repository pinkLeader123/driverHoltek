#include "systick.h"

// Biến đếm nhịp mili-giây hệ thống (tương đương uwTick trên STM32)
static volatile uint32_t s_tick_count = 0;

/**
 * @brief Khởi tạo PTM0 tạo ngắt đúng 1ms
 * @param fsys_hz Tần số xung fSYS hiện tại (ví dụ: 8000000UL)
 */
void SysTick_Init(uint32_t fsys_hz) {
    // 1. Tắt Timer trước khi cài đặt
    _pt0on = 0;

    // 2. Chọn xung Timer = fSYS / 4
    // PT0CK[2:0] = 000b (fSYS/4)
    _ptm0c0 = 0x00;

    // 3. PT0M[1:0] = 00 (Compare Match Output), PT0CCLR = 1 (Clear counter khi khớp CCRA)
    _ptm0c1 = 0x01;

    // 4. Tính toán giá trị nạp vào CCRA cho chu kỳ 1ms (1000us)
    // f_timer = fSYS / 4
    // Chu kỳ 1 count = 4 / fSYS
    // Compare Value = 1ms / (4 / fSYS) = fSYS / 4000
    uint16_t compare_val = (uint16_t)(fsys_hz / 4000UL);

    // Ghi CCRA: Theo tài liệu Holtek, ghi Byte thấp trước, Byte cao sau
    _ptm0al = (unsigned char)(compare_val & 0xFF);
    _ptm0ah = (unsigned char)((compare_val >> 8) & 0x03);

    // 5. Cấu hình ngắt PTM0A
    _ptm0af = 0; // Xóa cờ ngắt Comparator A
    _ptm0ae = 1; // Cho phép ngắt PTM0A (trong INTC1)
    _emi    = 1; // Cho phép ngắt toàn cục

    // 6. Kích hoạt bộ đếm
    _pt0on  = 1;
}

/**
 * @brief Trình phục vụ ngắt PTM0 Comparator A Match (Vector 0x14)
 */
DEFINE_ISR(SysTick_ISR, 0x14) {
    _ptm0af = 0; // Bắt buộc: Xóa cờ ngắt bằng phần mềm
    s_tick_count++;
}

/**
 * @brief Đọc giá trị tick 1ms hiện tại
 */
uint32_t SysTick_GetTick(void) {
    uint32_t tick;
    unsigned char emi_status = _emi;
    
    // Bảo vệ vùng đọc biến 32-bit tránh bị ngắt làm sai lệch dữ liệu
    _emi = 0;
    tick = s_tick_count;
    _emi = emi_status;
    
    return tick;
}

/**
 * @brief Hàm trễ mili-giây chuẩn xác (Có tích hợp nuôi Watchdog)
 */
void SysTick_Delay_ms(uint32_t ms) {
    uint32_t start_tick = SysTick_GetTick();
    
    while ((SysTick_GetTick() - start_tick) < ms) {
        // Nuôi Watchdog trong lúc chờ để chống reset hệ thống
        #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
            _clrwdt();
        #else
            __asm__("clr wdt");
        #endif
    }
}

/**
 * @brief Hàm trễ micro-giây (tính toán dựa trên fSYS = 8MHz)
 * 1 instruction cycle = 4 / 8MHz = 0.5us
 */
void SysTick_Delay_us(uint16_t us) {
    volatile uint16_t i;
    while (us--) {
        // Thêm 2 NOP ~ 1us (tại 8MHz HIRC)
        #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
            _nop();
            _nop();
        #else
            __asm__("nop");
            __asm__("nop");
        #endif
    }
}