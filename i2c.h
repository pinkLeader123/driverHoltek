#ifndef _I2C_H_
#define _I2C_H_

#include "BS86C08.h"
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Trạng thái phản hồi ACK/NACK */
typedef enum {
    I2C_ACK  = 0,
    I2C_NACK = 1
} i2c_ack_t;

/* Cấu hình bộ lọc khử nhiễu Debounce trên chân I2C phần cứng */
typedef enum {
    I2C_DEBOUNCE_DISABLE = 0, // Không lọc
    I2C_DEBOUNCE_1_FSYS  = 1, // Lọc xung ngắn 1 chu kỳ fSYS
    I2C_DEBOUNCE_2_FSYS  = 2, // Lọc xung ngắn 2 chu kỳ fSYS
    I2C_DEBOUNCE_4_FSYS  = 3  // Lọc xung ngắn 4 chu kỳ fSYS
} i2c_debounce_t;

/* ==========================================================================
 * MASTER MODE APIS (Dùng kết nối cảm biến, RTC, OLED, EEPROM ngoài)
 * ========================================================================== */
void I2C_Master_Init(void);
void I2C_Master_Start(void);
void I2C_Master_Stop(void);
bool I2C_Master_WriteByte(uint8_t data);
uint8_t I2C_Master_ReadByte(i2c_ack_t ack_action);

/* Giao tiếp Master mức cao */
bool I2C_Master_WriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
bool I2C_Master_ReadRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data);
bool I2C_Master_WriteBuffer(uint8_t dev_addr, uint8_t reg_addr, const uint8_t *p_buf, uint16_t len);
bool I2C_Master_ReadBuffer(uint8_t dev_addr, uint8_t reg_addr, uint8_t *p_buf, uint16_t len);

/* ==========================================================================
 * HARDWARE SLAVE MODE APIS (Dùng khi MCU này làm Slave cho vi xử lý khác)
 * ========================================================================== */
void I2C_Slave_Init(uint8_t slave_address_7bit, i2c_debounce_t debounce);
void I2C_Slave_EnableTimeout(uint8_t timeout_prescaler);
void I2C_Slave_Disable(void);
void I2C_Slave_EnableInterrupt(void);
void I2C_Slave_DisableInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif /* _I2C_H_ */