#include "adc.h"

void ADC_Init(void) {
    // SADC1: 
    // SAINS[2:0] = 000 (Kênh ngoài ANn)
    // SAVRS[1:0] = 01  (Tham chiếu là VDD)
    // SACKS[2:0] = 011 (Clock ADC = fSYS/8; Nếu fSYS=8MHz thì tADCK=1us, hợp lệ)
    _sadc1 = 0x0B;

    // SADC0: Bật bộ chuyển đổi ADC (ADCEN = 1), Căn phải dữ liệu (ADRFS = 1)
    _sadc0 = (1 << 5) | (1 << 4);
}

unsigned short ADC_ReadChannel(adc_channel_t ch) {
    // 1. Cấu hình pin sang ngõ vào Analog thông qua ACERL
    _acerl |= (1 << ch);

    // 2. Chọn kênh chuyển đổi qua SACS[3:0] trong SADC0
    _sadc0 = (_sadc0 & 0xF0) | (ch & 0x0F);

    // 3. Tạo xung trên bit START (0 -> 1 -> 0) để bắt đầu chuyển đổi
    _start = 0;
    _start = 1;
    _start = 0;

    // 4. Chờ cờ bận ADBZ xóa về 0
    while (_adbz);

    // 5. Đọc kết quả 12-bit (với ADRFS=1: SADOH chứa 4 bit cao, SADOL chứa 8 bit thấp)
    unsigned short result = ((unsigned short)(_sadoh & 0x0F) << 8) | _sadol;
    return result;
}