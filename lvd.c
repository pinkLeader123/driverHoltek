#include "lvd.h"

#define LVD_VOLT_MASK       0x07
#define LVD_ENABLE_BIT      (1U << 4)
#define LVD_VBGEN_BIT       (1U << 3)

/**
 * @brief Hàm delay nhỏ đảm bảo LVD ổn định (tLVDS <= 15us)
 */
static void LVD_StabilizeDelay(void) {
    volatile unsigned char i;
    for (i = 0; i < 40; i++) {
        #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
            _nop();
        #else
            __asm__("nop");
        #endif
    }
}

/**
 * @brief Khởi tạo module LVD
 * @param voltage_threshold: Mức điện áp phát hiện (2.0V ~ 4.0V)
 * @param enable_bandgap: Cho phép bật thêm tham chiếu Bandgap nội (1/0)
 */
void LVD_Init(lvd_volt_t voltage_threshold, unsigned char enable_bandgap) {
    unsigned char reg_val = 0;

    // Tắt ngắt trước khi cấu hình
    LVD_DisableInterrupt();
    LVD_ClearInterruptFlag();

    // Cấu hình mức điện áp
    reg_val = (unsigned char)(voltage_threshold & LVD_VOLT_MASK);

    if (enable_bandgap) {
        reg_val |= LVD_VBGEN_BIT;
    }

    // Bật LVD
    reg_val |= LVD_ENABLE_BIT;

    // Ghi vào thanh ghi LVDC
    _lvdc = reg_val;

    // Chờ điện áp so sánh ổn định
    LVD_StabilizeDelay();
}

/**
 * @brief Kích hoạt LVD
 */
void LVD_Enable(void) {
    _lvden = 1;
    LVD_StabilizeDelay();
}

/**
 * @brief Vô hiệu hóa LVD nhằm tiết kiệm năng lượng
 */
void LVD_Disable(void) {
    _lvden = 0;
}

/**
 * @brief Đổi ngưỡng phát hiện điện áp trong quá trình chạy
 */
void LVD_SetVoltage(lvd_volt_t voltage_threshold) {
    unsigned char temp = _lvdc;
    temp &= ~LVD_VOLT_MASK;
    temp |= (voltage_threshold & LVD_VOLT_MASK);
    _lvdc = temp;
    LVD_StabilizeDelay();
}

/**
 * @brief Kiểm tra xem nguồn VDD hiện tại có đang thấp hơn mức ngưỡng cấu hình hay không
 * @return 1 nếu VDD <= VLVD (Điện áp thấp), 0 nếu VDD > VLVD (Điện áp bình thường)
 */
unsigned char LVD_IsLowVoltage(void) {
    return (_lvdo == 1);
}

/**
 * @brief Bật ngắt LVD (khi VDD tụt dưới ngưỡng sẽ phát sinh ngắt)
 */
void LVD_EnableInterrupt(void) {
    LVD_ClearInterruptFlag();
    _lve = 1; // Cho phép ngắt LVD trong INTC2
    _emi = 1; // Cho phép ngắt toàn cục
}

/**
 * @brief Vô hiệu hóa ngắt LVD
 */
void LVD_DisableInterrupt(void) {
    _lve = 0;
}

/**
 * @brief Xóa cờ ngắt LVD
 */
void LVD_ClearInterruptFlag(void) {
    _lvf = 0;
}