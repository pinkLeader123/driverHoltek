#include "i2c.h"

/* 
 * Định nghĩa cấu hình chân GPIO cho I2C Master:
 * Mặc định sử dụng PA1 (SCL) và PA3 (SDA) trên BS86C08C
 */
#define SCL_PIN_READ()       _pa1
#define SDA_PIN_READ()       _pa3

#define SCL_OUTPUT_LOW()     do { _pac1 = 0; _pa1 = 0; } while(0)
#define SCL_FLOAT_HIGH()     do { _pac1 = 1; _papu1 = 1; } while(0)

#define SDA_OUTPUT_LOW()     do { _pac3 = 0; _pa3 = 0; } while(0)
#define SDA_FLOAT_HIGH()     do { _pac3 = 1; _papu3 = 1; } while(0)

/**
 * @brief Hàm trễ bán chu kỳ xung I2C Master (~100kHz tại fSYS = 8MHz)
 */
static void I2C_Delay(void) {
    volatile uint8_t count = 4;
    while (count--) {
        #if defined(__HOLTEK_C__) || defined(__HT66FXX__)
            _nop();
        #else
            __asm__("nop");
        #endif
    }
}

/* ==========================================================================
 * I2C MASTER DRIVER IMPLEMENTATION
 * ========================================================================== */

void I2C_Master_Init(void) {
    // Đảm bảo khối I2C slave phần cứng tắt để không khóa chân I/O
    _iicen = 0;

    // Cấu hình SCL và SDA ở trạng thái thả nổi kéo lên (Bus Idle: SDA=1, SCL=1)
    SDA_FLOAT_HIGH();
    SCL_FLOAT_HIGH();
    I2C_Delay();
}

void I2C_Master_Start(void) {
    SDA_FLOAT_HIGH();
    SCL_FLOAT_HIGH();
    I2C_Delay();

    // START condition: SDA kéo xuống LOW khi SCL đang HIGH
    SDA_OUTPUT_LOW();
    I2C_Delay();
    SCL_OUTPUT_LOW();
    I2C_Delay();
}

void I2C_Master_Stop(void) {
    // STOP condition: SDA kéo từ LOW lên HIGH khi SCL đang HIGH
    SDA_OUTPUT_LOW();
    I2C_Delay();
    SCL_FLOAT_HIGH();
    I2C_Delay();
    SDA_FLOAT_HIGH();
    I2C_Delay();
}

/**
 * @brief Truyền 1 byte ra bus I2C và đọc phản hồi ACK/NACK
 * @return true nếu Slave phản hồi ACK (0), false nếu NACK (1) hoặc Timeout
 */
bool I2C_Master_WriteByte(uint8_t data) {
    uint8_t i;
    bool ack;

    // Xuất 8 bit dữ liệu (MSB First)
    for (i = 0; i < 8; i++) {
        if (data & 0x80) {
            SDA_FLOAT_HIGH();
        } else {
            SDA_OUTPUT_LOW();
        }
        I2C_Delay();
        SCL_FLOAT_HIGH();
        I2C_Delay();
        SCL_OUTPUT_LOW();
        data <<= 1;
    }

    // Đọc bit thứ 9: ACK/NACK từ phía Slave
    SDA_FLOAT_HIGH(); // Nhả đường SDA để Slave kéo xuống
    I2C_Delay();
    SCL_FLOAT_HIGH();
    I2C_Delay();

    ack = (SDA_PIN_READ() == 0); // Slave kéo SDA xuống 0 là ACK

    SCL_OUTPUT_LOW();
    I2C_Delay();

    return ack;
}

/**
 * @brief Đọc 1 byte từ bus I2C và phản hồi ACK/NACK về Slave
 */
uint8_t I2C_Master_ReadByte(i2c_ack_t ack_action) {
    uint8_t i;
    uint8_t data = 0;

    SDA_FLOAT_HIGH(); // Đảm bảo chân nhận ở chế độ Input

    for (i = 0; i < 8; i++) {
        data <<= 1;
        SCL_FLOAT_HIGH();
        I2C_Delay();
        if (SDA_PIN_READ()) {
            data |= 0x01;
        }
        SCL_OUTPUT_LOW();
        I2C_Delay();
    }

    // Gửi bit thứ 9 (ACK hoặc NACK)
    if (ack_action == I2C_ACK) {
        SDA_OUTPUT_LOW();
    } else {
        SDA_FLOAT_HIGH();
    }
    I2C_Delay();

    SCL_FLOAT_HIGH();
    I2C_Delay();
    SCL_OUTPUT_LOW();
    SDA_FLOAT_HIGH();
    I2C_Delay();

    return data;
}

bool I2C_Master_WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t data) {
    I2C_Master_Start();

    // Gửi địa chỉ thiết bị + bit W (0)
    if (!I2C_Master_WriteByte(dev_addr & 0xFE)) {
        I2C_Master_Stop();
        return false;
    }

    // Gửi địa chỉ thanh ghi đích
    if (!I2C_Master_WriteByte(reg_addr)) {
        I2C_Master_Stop();
        return false;
    }

    // Gửi dữ liệu nạp
    if (!I2C_Master_WriteByte(data)) {
        I2C_Master_Stop();
        return false;
    }

    I2C_Master_Stop();
    return true;
}

bool I2C_Master_ReadRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data) {
    if (data == 0) return false;

    // Giai đoạn 1: Ghi địa chỉ thanh ghi cần đọc
    I2C_Master_Start();
    if (!I2C_Master_WriteByte(dev_addr & 0xFE)) {
        I2C_Master_Stop();
        return false;
    }
    if (!I2C_Master_WriteByte(reg_addr)) {
        I2C_Master_Stop();
        return false;
    }

    // Giai đoạn 2: Repeated Start và đọc giá trị về
    I2C_Master_Start();
    if (!I2C_Master_WriteByte(dev_addr | 0x01)) { // Địa chỉ + Bit R (1)
        I2C_Master_Stop();
        return false;
    }

    *data = I2C_Master_ReadByte(I2C_NACK); // Chỉ đọc 1 byte thì trả về NACK
    I2C_Master_Stop();

    return true;
}

bool I2C_Master_WriteBuffer(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *p_buf, uint16_t len) {
    uint16_t i;
    if (p_buf == 0 || len == 0) return false;

    I2C_Master_Start();
    if (!I2C_Master_WriteByte(dev_addr & 0xFE)) {
        I2C_Master_Stop();
        return false;
    }
    if (!I2C_Master_WriteByte(reg_addr)) {
        I2C_Master_Stop();
        return false;
    }

    for (i = 0; i < len; i++) {
        if (!I2C_Master_WriteByte(p_buf[i])) {
            I2C_Master_Stop();
            return false;
        }
    }

    I2C_Master_Stop();
    return true;
}

bool I2C_Master_ReadBuffer(uint8_t dev_addr, uint8_t reg_addr, uint8_t *p_buf, uint16_t len) {
    uint16_t i;
    if (p_buf == 0 || len == 0) return false;

    I2C_Master_Start();
    if (!I2C_Master_WriteByte(dev_addr & 0xFE)) {
        I2C_Master_Stop();
        return false;
    }
    if (!I2C_Master_WriteByte(reg_addr)) {
        I2C_Master_Stop();
        return false;
    }

    I2C_Master_Start();
    if (!I2C_Master_WriteByte(dev_addr | 0x01)) {
        I2C_Master_Stop();
        return false;
    }

    for (i = 0; i < len; i++) {
        p_buf[i] = I2C_Master_ReadByte((i == len - 1) ? I2C_NACK : I2C_ACK);
    }

    I2C_Master_Stop();
    return true;
}

/* ==========================================================================
 * HARDWARE I2C SLAVE IMPLEMENTATION
 * ========================================================================== */

/**
 * @brief Khởi tạo module I2C phần cứng trên MCU ở vai trò Slave
 * @param slave_address_7bit: Địa chỉ 7-bit của Slave (ví dụ 0x30)
 * @param debounce: Mức lọc nhiễu đường truyền I2C
 */
void I2C_Slave_Init(uint8_t slave_address_7bit, i2c_debounce_t debounce) {
    // 1. Tắt I2C trước khi cấu hình
    _iicen = 0;

    // 2. Cài đặt địa chỉ nhận diện của MCU (Bit 7~1 lưu địa chỉ 7-bit)
    _iica = (unsigned char)((slave_address_7bit & 0x7F) << 1);

    // 3. Cấu hình IICC0 (Bật chức năng I2C và chọn mức lọc Debounce)
    _iicdeb0 = (debounce & 0x01) ? 1 : 0;
    _iicdeb1 = (debounce & 0x02) ? 1 : 0;
    _iicen   = 1; // Bật khối giao tiếp I2C phần cứng

    // 4. Cấu hình IICC1
    _txak = 0;    // Cho phép tự động sinh ACK khi nhận dữ liệu
    _htx  = 0;    // Mặc định ở chế độ Receiver

    // 5. Xóa cờ ngắt I2C trong INTC2
    _iicf = 0;
}

/**
 * @brief Kích hoạt bộ đếm thời gian Time-out bảo vệ chống treo đường truyền
 */
void I2C_Slave_EnableTimeout(uint8_t timeout_prescaler) {
    _iictoc = 0x00;
    _iictos0 = (timeout_prescaler & (1 << 0)) ? 1 : 0;
    _iictos1 = (timeout_prescaler & (1 << 1)) ? 1 : 0;
    _iictos2 = (timeout_prescaler & (1 << 2)) ? 1 : 0;
    _iictos3 = (timeout_prescaler & (1 << 3)) ? 1 : 0;
    _iictos4 = (timeout_prescaler & (1 << 4)) ? 1 : 0;
    _iictos5 = (timeout_prescaler & (1 << 5)) ? 1 : 0;
    _iictof  = 0;
    _iictoen = 1; // Bật Time-out
}

void I2C_Slave_Disable(void) {
    _iicen = 0;
}

void I2C_Slave_EnableInterrupt(void) {
    _iicf = 0;
    _iice = 1; // Bật bit ngắt I2C trong INTC2
    _emi  = 1; // Bật ngắt toàn cục
}

void I2C_Slave_DisableInterrupt(void) {
    _iice = 0;
}