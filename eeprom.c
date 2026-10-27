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

}
void EEPROM_WriteProtectDisable(void){

}
void EEPROM_WriteInterruptEnable(void){
    
}