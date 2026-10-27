#include "clock.h"


void SYS_Clock_Init(sys_hirc_t freq) {
    // Cấu hình tần số HIRC trong thanh ghi CTRL (Bit 5~4)
    switch (freq) {
        case SYS_HIRC_12MHZ:
            _hircs1 = 0; _hircs0 = 1;
            break;
        case SYS_HIRC_16MHZ:
            _hircs1 = 1; _hircs0 = 0;
            break;
        case SYS_HIRC_8MHZ:
        default:
            _hircs1 = 0; _hircs0 = 0;
            break;
    }

    // Chọn xung hệ thống fSYS = fH (không chia tần, HLCLK = 1)
    _hlclk = 1;

    // Chờ bộ tạo dao động ổn định (HTO flag = 1)
    while (_hto == 0);
}

void SYS_Enter_Sleep(void) {
    _idlen = 0;     // IDLEN = 0: Vào chế độ SLEEP khi gặp lệnh HALT
    #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
        _halt();
    #else
        __asm__("halt");
    #endif
}

void SYS_Enter_Idle(void) {
    _idlen = 1;     // IDLEN = 1: Cho phép IDLE mode
    _fsyson = 1;    // fSYS vẫn chạy cấp xung cho ngoại vi
    #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
        _halt();
    #else
        __asm__("halt");
    #endif
}