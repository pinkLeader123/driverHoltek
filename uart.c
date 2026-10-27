#include "uart.h"

/**
 * @brief Khởi tạo Baud Rate Generator và các thông số hoạt động của UART
 * @param fsys_hz: Tần số xung nhịp hệ thống (Hz)
 * @param baudrate: Tốc độ baud mong muốn (vd: 9600, 115200)
 * @param parity: Chế độ kiểm tra chẵn lẻ
 * @param stop_bits: Số lượng Stop bit
 */
void UART_Init(uint32_t fsys_hz, uint32_t baudrate, uart_parity_t parity, uart_stop_t stop_bits) {
    uint32_t brg_val;

    // 1. Tắt tạm thời UART để cấu hình
    _uarten0 = 0;

    // 2. Cấu hình chân chức năng UART0 trên Pin-remapping (IFS0 / IFS2)
    // Cấu hình mặc định: TX0 trên PA3, RX0 trên PA1
    _tx0ps1 = 0; _tx0ps0 = 0;   // TX0 on PA3
    _rx0ps1 = 0; _rx0ps0 = 0;   // RX0 on PA1
    _rx0en  = 1;                // Kích hoạt cổng đệm nhận RX0 (IFS2)

    // Cấu hình hướng chân IO
    _pac3 = 0;                  // PA3: Output (TX0)
    _pac1 = 1;                  // PA1: Input  (RX0)
    _papu1 = 1;                 // Bật pull-up cho chân RX0 chống nhiễu

    // 3. Tính toán Baud Rate Generator (BRG0) ở chế độ High-Speed (BRGH0 = 1)
    // Công thức: Baud = fSYS / (16 * (N + 1))  ==>  N = (fSYS / (16 * Baud)) - 1
    _brgh0 = 1;
    brg_val = (fsys_hz / (16UL * baudrate));
    if (brg_val > 0) {
        brg_val -= 1;
    }
    if (brg_val > 255) {
        // Nếu xung nhịp quá nhanh làm tràn 8-bit, chuyển về Low-Speed (BRGH0 = 0)
        _brgh0 = 0;
        brg_val = (fsys_hz / (64UL * baudrate));
        if (brg_val > 0) {
            brg_val -= 1;
        }
    }
    _brg0 = (unsigned char)(brg_val & 0xFF);

    // 4. Cấu hình Frame Format trong U0CR1
    _bno0 = 0;                  // Mặc định 8 data bits (chưa tính parity)
    _stops0 = (stop_bits == UART_STOP_2BIT) ? 1 : 0;

    switch (parity) {
        case UART_PARITY_EVEN:
            _pren0 = 1;         // Enable parity
            _prt0  = 0;         // Even parity
            _bno0  = 1;         // Dùng khung 9-bit để chứa parity
            break;
        case UART_PARITY_ODD:
            _pren0 = 1;         // Enable parity
            _prt0  = 1;         // Odd parity
            _bno0  = 1;         // Dùng khung 9-bit để chứa parity
            break;
        case UART_PARITY_NONE:
        default:
            _pren0 = 0;         // Tắt parity
            _bno0  = 0;         // 8-bit data
            break;
    }

    // 5. Cấu hình Transmitter & Receiver Enable trong U0CR2
    _txen0 = 1;                 // Cho phép Transmitter
    _rxen0 = 1;                 // Cho phép Receiver

    // 6. Kích hoạt toàn bộ module UART0
    _uarten0 = 1;

    // 7. Xóa sạch các cờ ngắt tồn đọng ban đầu
    _ur0f = 0;
}

void UART_Enable(void) {
    _uarten0 = 1;
}

void UART_Disable(void) {
    _uarten0 = 0;
}

/* ==========================================================================
 * TRANSMITTER FUNCTIONS (Truyền dữ liệu & Break)
 * ========================================================================== */

/**
 * @brief Kiểm tra xem bộ đệm truyền TXR_RXR0 có trống để nạp byte mới không
 */
unsigned char  UART_IsTxBufferEmpty(void) {
    return (_txif0 == 1);
}

/**
 * @brief Kiểm tra xem bộ phát đã hoàn thành truyền toàn bộ dữ liệu (Idle) chưa
 */
unsigned char  UART_IsTxIdle(void) {
    return (_tidle0 == 1);
}

/**
 * @brief Truyền 1 byte qua UART (Chế độ Polling có bảo vệ)
 */
void UART_WriteByte(uint8_t data) {
    // Chờ cho đến khi bộ đệm truyền trống (_txif0 = 1)
    while (_txif0 == 0);

    // Nạp dữ liệu vào thanh ghi truyền
    _txr_rxr0 = data;
}

/**
 * @brief Truyền một chuỗi ký tự kết thúc bằng null-terminator
 */
void UART_WriteString(const char *str) {
    while (*str) {
        UART_WriteByte((uint8_t)*str);
        str++;
    }
}

/**
 * @brief Gửi tín hiệu Transmit Break (Kéo đường truyền TX xuống LOW)
 */
void UART_SendBreak(void) {
    // Chờ các byte đang phát hoàn tất
    while (_tidle0 == 0);

    // Kích hoạt phát Break bit
    _txbrk0 = 1;

    // Duy trì mức Low ít nhất 1 khung dữ liệu (tạo độ trễ nhỏ)
    volatile uint16_t delay = 1000;
    while (delay--) {
        #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
            _nop();
        #else
            __asm__("nop");
        #endif
    }

    // Tắt Break để trả chân TX về trạng thái High (Idle)
    _txbrk0 = 0;
}

/* ==========================================================================
 * RECEIVER FUNCTIONS (Nhận dữ liệu & Quản lý lỗi)
 * ========================================================================== */

/**
 * @brief Kiểm tra có dữ liệu mới trong bộ đệm nhận không
 */
unsigned char  UART_IsDataAvailable(void) {
    return (_rxif0 == 1);
}

/**
 * @brief Đọc 1 byte từ bộ đệm UART (Polling)
 */
uint8_t UART_ReadByte(void) {
    while (_rxif0 == 0);
    return _txr_rxr0; // Đọc thanh ghi tự động xóa cờ _rxif0
}

/**
 * @brief Đọc 1 byte kèm theo kiểm tra toàn bộ lỗi phần cứng
 */
unsigned char  UART_ReceiveByte_WithCheck(uint8_t *data, uart_err_t *err) {
    if (_rxif0 == 0) {
        return 0;
    }

    // Đọc mã lỗi trước khi đọc data (bởi vì đọc data có thể thay đổi trạng thái)
    *err = UART_GetErrors();

    // Đọc dữ liệu ra khỏi buffer
    *data = _txr_rxr0;

    // Nếu phát hiện lỗi Overrun, cần chủ động xóa bằng phần mềm
    if (*err & UART_ERR_OVERRUN) {
        UART_ClearErrors();
    }

    return 1;
}

/**
 * @brief Thu thập toàn bộ cờ lỗi phần cứng từ thanh ghi U0SR
 */
uart_err_t UART_GetErrors(void) {
    uint8_t err_mask = UART_ERR_OK;

    if (_oerr0) err_mask |= UART_ERR_OVERRUN;
    if (_ferr0) err_mask |= UART_ERR_FRAMING;
    if (_perr0) err_mask |= UART_ERR_PARITY;
    if (_nf0)   err_mask |= UART_ERR_NOISE;

    return (uart_err_t)err_mask;
}

/**
 * @brief Xóa cờ lỗi Overrun phần cứng
 * @note Trên Holtek, OERR được clear bằng cách đọc U0SR rồi đọc tiếp TXR_RXR0
 */
void UART_ClearErrors(void) {
    volatile uint8_t dummy_sr = _u0sr;
    volatile uint8_t dummy_dr = _txr_rxr0;
    (void)dummy_sr;
    (void)dummy_dr;
}

/**
 * @brief Phát hiện xung Break từ bên ngoài truyền vào
 */
unsigned char  UART_IsBreakDetected(void) {
    // Receive Break xảy ra khi bắt lỗi Frame kết hợp giá trị byte nhận là 0x00
    if (_ferr0 && (_txr_rxr0 == 0x00)) {
        return 1;
    }
    return 0;
}

/* ==========================================================================
 * INTERRUPT STRUCTURE (Cấu hình ngắt hệ thống)
 * ========================================================================== */

void UART_EnableRxInterrupt(void) {
    _rie0 = 1;      // Bật cờ ngắt nhận riêng của UART (U0CR2)
    _ur0e = 1;      // Bật ngắt ngoại vi UART0 trong INTC2
    _emi  = 1;      // Bật ngắt toàn cục
}

void UART_DisableRxInterrupt(void) {
    _rie0 = 0;
}

void UART_EnableTxInterrupt(void) {
    _tiie0 = 1;     // Cho phép ngắt khi bộ đệm truyền rỗng
    _ur0e  = 1;     // Bật ngắt UART0 trong INTC2
    _emi   = 1;
}

void UART_DisableTxInterrupt(void) {
    _tiie0 = 0;
}

/* ==========================================================================
 * POWER DOWN & WAKE-UP (Tiết kiệm điện)
 * ========================================================================== */

/**
 * @brief Cho phép UART đánh thức vi điều khiển khi đang ở SLEEP / IDLE
 */
void UART_EnableWakeup(void) {
    _wake0 = 1;     // Kích hoạt RX Pin Falling-Edge Wake-up
}

void UART_DisableWakeup(void) {
    _wake0 = 0;
}