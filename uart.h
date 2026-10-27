#ifndef _UART_H_
#define _UART_H_

#include "BS86C08C.H"
#include "build-in.h"
#include <stdint.h>


typedef enum {
    UART_ERR_OK       = 0x00,
    UART_ERR_OVERRUN  = 0x01,
    UART_ERR_FRAMING  = 0x02,
    UART_ERR_PARITY   = 0x04,
    UART_ERR_NOISE    = 0x08
} uart_err_t;

/* Tùy chọn cấu hình độ dài Stop bit */
typedef enum {
    UART_STOP_1BIT = 0,
    UART_STOP_2BIT = 1
} uart_stop_t;

/* Tùy chọn cấu hình Parity */
typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_EVEN = 1,
    UART_PARITY_ODD  = 2
} uart_parity_t;

/* Khởi tạo UART & Baud Rate Generator */
void UART_Init(uint32_t fsys_hz, uint32_t baudrate, uart_parity_t parity, uart_stop_t stop_bits);
void UART_Enable(void);
void UART_Disable(void);

/* Truyền dữ liệu (Transmitter) */
void UART_WriteByte(uint8_t data);
void UART_WriteString(const char *str);
unsigned char UART_IsTxBufferEmpty(void);
unsigned char UART_IsTxIdle(void);
void UART_SendBreak(void);

/* Nhận dữ liệu (Receiver) */
unsigned char UART_IsDataAvailable(void);
uint8_t UART_ReadByte(void);
unsigned char UART_ReceiveByte_WithCheck(uint8_t *data, uart_err_t *err);
unsigned char UART_IsBreakDetected(void);

/* Quản lý lỗi (Managing Receiver Errors) */
uart_err_t UART_GetErrors(void);
void UART_ClearErrors(void);

/* Cấu hình ngắt (Interrupt Structure) */
void UART_EnableRxInterrupt(void);
void UART_DisableRxInterrupt(void);
void UART_EnableTxInterrupt(void);
void UART_DisableTxInterrupt(void);

/* Tiết kiệm điện & Đánh thức (Power Down & Wake-up) */
void UART_EnableWakeup(void);
void UART_DisableWakeup(void);

#endif /* _UART_H_ */