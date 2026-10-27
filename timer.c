#include "timer.h"

/* Ghi giá trị 10-bit vào thanh ghi CCRA (Byte thấp trước, Byte cao sau) */
void PTM0_WriteCCRA(uint16_t value) {
    value &= 0x03FF;
    _ptm0al = (unsigned char)(value & 0xFF);
    _ptm0ah = (unsigned char)((value >> 8) & 0x03);
}

/* Ghi giá trị 10-bit vào thanh ghi CCRP (Byte thấp trước, Byte cao sau) */
void PTM0_WriteCCRP(uint16_t value) {
    value &= 0x03FF;
    _ptm0rpl = (unsigned char)(value & 0xFF);
    _ptm0rph = (unsigned char)((value >> 8) & 0x03);
}

/* Đọc giá trị 10-bit từ thanh ghi CCRA (Byte cao trước, Byte thấp sau) */
uint16_t PTM0_ReadCCRA(void) {
    uint8_t high = _ptm0ah & 0x03;
    uint8_t low = _ptm0al;
    return (((uint16_t)high << 8) | low);
}

/* Đọc giá trị bộ đếm hiện tại của PTM0 */
uint16_t PTM0_ReadCounter(void) {
    uint8_t high = _ptm0dh & 0x03;
    uint8_t low = _ptm0dl;
    return (((uint16_t)high << 8) | low);
}

void PTM0_Start(void) {
    _pt0on = 1;
}

void PTM0_Stop(void) {
    _pt0on = 0;
}

void PTM0_Pause(void) {
    _pt0pau = 1;
}

void PTM0_Resume(void) {
    _pt0pau = 0;
}

/* --------------------------------------------------------------------------
 * 1. Timer / Counter Mode:
 * PT0M[1:0] = 11, PT0CCLR = 1 (Xóa counter khi khớp CCRA)
 * -------------------------------------------------------------------------- */
void PTM0_Config_TimerCounter(ptm_clock_t clk, uint16_t compare_period) {
    PTM0_Stop();

    // Thiết lập xung nhịp (Bits 6~4)
    _ptm0c0 = (unsigned char)((clk & 0x07) << 4);

    // PT0M[1:0] = 11 (Timer/Counter Mode), PT0CCLR = 1 (Clear by CCRA)
    _ptm0c1 = (3U << 6) | (1U << 0);

    PTM0_WriteCCRA(compare_period);

    _ptm0af = 0; // Xóa cờ ngắt Comparator A
}

/* --------------------------------------------------------------------------
 * 2. PWM Output Mode:
 * PT0M[1:0] = 10, PT0IO[1:0] = 10 (PWM output)
 * CCRP = Chu kỳ (Period), CCRA = Độ rộng xung (Duty)
 * -------------------------------------------------------------------------- */
void PTM0_Config_PWM(ptm_clock_t clk, uint16_t period, uint16_t duty, unsigned char active_high) {
    PTM0_Stop();

    // 1. Cấu hình chân PTP0 (PD5) làm ngõ ra PWM thông qua TMPC
    _ptm0pc0 = 1; // 1: Enable chân chức năng PTP0
    _pdc5 = 0;    // Đảm bảo cấu hình là CMOS Output

    // 2. Thiết lập Clock
    _ptm0c0 = (unsigned char)((clk & 0x07) << 4);

    // 3. Thiết lập chế độ PWM:
    // Bits 7~6: PT0M[1:0] = 10 (PWM Mode)
    // Bits 5~4: PT0IO[1:0] = 10 (PWM Output)
    // Bit 3   : PT0OC (1: Active High, 0: Active Low)
    // Bit 2   : PT0POL = 0 (Non-inverted)
    unsigned char c1 = (2U << 6) | (2U << 4);
    if (active_high) {
        c1 |= (1U << 3);
    }
    _ptm0c1 = c1;

    // 4. Nạp giá trị chu kỳ (CCRP) và độ rộng (CCRA)
    PTM0_WriteCCRP(period);
    PTM0_WriteCCRA(duty);
}

void PTM0_SetPWM_Duty(uint16_t duty) {
    PTM0_WriteCCRA(duty);
}

/* --------------------------------------------------------------------------
 * 3. Single Pulse Output Mode:
 * PT0M[1:0] = 10, PT0IO[1:0] = 11 (Single Pulse)
 * CCRA = Độ rộng xung (Pulse width)
 * -------------------------------------------------------------------------- */
void PTM0_Config_SinglePulse(ptm_clock_t clk, uint16_t pulse_width, unsigned char active_high) {
    PTM0_Stop();

    // Bật ngõ ra chân PTP0 (PD5)
    _ptm0pc0 = 1;
    _pdc5 = 0;

    _ptm0c0 = (unsigned char)((clk & 0x07) << 4);

    // Bits 7~6: PT0M[1:0] = 10
    // Bits 5~4: PT0IO[1:0] = 11 (Single Pulse Output)
    // Bit 3   : PT0OC
    unsigned char c1 = (2U << 6) | (3U << 4);
    if (active_high) {
        c1 |= (1U << 3);
    }
    _ptm0c1 = c1;

    PTM0_WriteCCRA(pulse_width);
}

void PTM0_Trigger_SinglePulse(void) {
    // Phát xung bằng cách kéo PT0ON lên 1 (Sau khi khớp CCRA phần cứng tự xóa PT0ON về 0)
    _pt0on = 0;
    _pt0on = 1;
}

/* --------------------------------------------------------------------------
 * 4. Capture Input Mode:
 * PT0M[1:0] = 01 (Capture Input)
 * PT0IO[1:0] = Edge select, PT0CAPTS = Input source select
 * -------------------------------------------------------------------------- */
void PTM0_Config_Capture(ptm_clock_t clk, ptm_capture_src_t src, ptm_capture_edge_t edge) {
    PTM0_Stop();

    // Cấu hình chân input tương ứng
    if (src == CAPTURE_SRC_PTPOI) {
        _pac4 = 1; // PA4 làm ngõ vào
    } else {
        _pac7 = 1; // PA7 làm ngõ vào
    }

    _ptm0c0 = (unsigned char)((clk & 0x07) << 4);

    // Bits 7~6: PT0M[1:0] = 01 (Capture Mode)
    // Bits 5~4: PT0IO[1:0] (Lựa chọn cạnh kích hoạt)
    // Bit 1   : PT0CAPTS (0: PTPOI, 1: PTCK0)
    unsigned char c1 = (1U << 6) | ((edge & 0x03) << 4);
    if (src == CAPTURE_SRC_PTCK0) {
        c1 |= (1U << 1);
    }
    _ptm0c1 = c1;

    // Counter đếm tự do đến giá trị max (0x3FF) bằng cách đặt CCRP = 0
    PTM0_WriteCCRP(0);

    _ptm0af = 0; // Xóa cờ báo capture
}

unsigned char PTM0_IsCaptureOccurred(void) {
    return (_ptm0af == 1);
}

void PTM0_ClearCaptureFlag(void) {
    _ptm0af = 0;
}

uint16_t PTM0_GetCapturedValue(void) {
    return PTM0_ReadCCRA();
}