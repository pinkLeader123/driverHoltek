#include "eeprom.h"

unsigned char EEPROM_ReadByte(unsigned char addr)
{
    _eea = (addr & 0x1F); // BS86C08C hỗ trợ 32 bytes (0x00~0x1F)

    // Trỏ tới thanh ghi EEC tại Sector 1 (Địa chỉ 0x140)
    _mp1l = EEC_ADDR_SECTOR1;
    _mp1h = 0x01;

    // Bật bit RDEN (bit 1) và kích hoạt RD (bit 0) qua IAR1
    _iar1 |= (1 << 1); 
    _iar1 |= (1 << 0);

    // Chờ quá trình đọc hoàn tất (phần cứng tự xóa bit RD về 0)
    while (_iar1 & (1 << 0));

    _iar1 = 0;  // Xóa quyền truy cập
    _mp1h = 0;  // Trả con trỏ về Sector 0

    return _eed;

}

void EEPROM_WriteByte(unsigned char addr, unsigned char data) {
    _eea = (addr & 0x1F);
    _eed = data;

    // Trỏ tới thanh ghi EEC tại Sector 1
    _mp1l = EEC_ADDR_SECTOR1;
    _mp1h = 0x01;

    // Vô hiệu hóa ngắt trước khi ghi chuỗi WREN -> WR
    unsigned char emi_backup = _emi;
    _emi = 0;

    // Bật WREN (bit 3), ngay sau đó bật WR (bit 2)
    _iar1 |= (1 << 3);
    _iar1 |= (1 << 2);

    // Khôi phục lại trạng thái ngắt
    _emi = emi_backup;

    // Chờ quá trình ghi vật lý hoàn tất (phần cứng tự xóa WR về 0)
    while (_iar1 & (1 << 2));

    _iar1 = 0;  // Xóa WREN
    _mp1h = 0;  // Trả con trỏ về Sector 0
}

void EEPROM_WriteProtectEnable(void){
    // 1. Trỏ con trỏ bộ nhớ MP1 tới thanh ghi EEC tại Sector 1 (Địa chỉ 0x140)
    _mp1h = 0x01;
    _mp1l = EEC_ADDR_SECTOR1;

    // 2. Xóa bit WREN (Bit 3) về 0 qua thanh ghi gián tiếp IAR1
    _iar1 &= (unsigned char)(~EEC_WREN_BIT);

    // 3. Trả con trỏ bộ nhớ về Sector 0 để đảm bảo an toàn cho các tác vụ khác
    _mp1h = 0x00;

}
void EEPROM_WriteProtectDisable(void){
    _mp1h = 0x01;
    _mp1l = EEC_ADDR_SECTOR1;

    // 2. Bật bit WREN (Bit 3) lên 1 qua thanh ghi gián tiếp IAR1
    _iar1 |= (unsigned char)EEC_WREN_BIT;

    // 3. Trả con trỏ bộ nhớ về Sector 0
    _mp1h = 0x00;
}
void EEPROM_WriteInterruptEnable(void){
    _def = 0;   // Xóa cờ ngắt EEPROM tồn đọng trong INTC2
    _dee = 1;   // Bật bit cho phép ngắt Data EEPROM (Bit 1 trong INTC2)
    _emi = 1;   // Cho phép ngắt toàn cục (INTC0)
}

/**
 * @brief Vô hiệu hóa ngắt ghi EEPROM
 */
void EEPROM_WriteInterruptDisable(void) {
    _dee = 0;   // Tắt bit ngắt Data EEPROM trong INTC2
}

/**
 * @brief Xóa cờ ngắt ghi EEPROM
 */
void EEPROM_ClearInterruptFlag(void) {
    _def = 0;
}