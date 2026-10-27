#include "74hc595.h"

void hc595_init(void)
{
    Gpio_Init(HC595_DS_PORT, HC595_DS_PIN, GPIO_OUTPUT, PULL_UP_ON);
    Gpio_Init(HC595_SHCP_PORT, HC595_SHCP_PIN, GPIO_OUTPUT, PULL_UP_ON);
    Gpio_Init(HC595_STCP_PORT, HC595_STCP_PIN, GPIO_OUTPUT, PULL_UP_ON);
}

void hc595_write(uint8_t data)
{
    HC595_SendByte(data);
    HC595_Latch();
}

void HC595_ShiftOut(uint8_t data)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        Gpio_Write(HC595_DS_PORT, HC595_DS_PIN, (data & (1 << (7 - i))) ? ON : OFF);
        Gpio_Write(HC595_SHCP_PORT, HC595_SHCP_PIN, ON);
        Gpio_Write(HC595_SHCP_PORT, HC595_SHCP_PIN, OFF);
    }
}

void HC595_Latch(void)
{
    Gpio_Write(HC595_STCP_PORT, HC595_STCP_PIN, ON);
    Gpio_Write(HC595_STCP_PORT, HC595_STCP_PIN, OFF);
}

void HC595_SendByte(uint8_t data)
{
    HC595_ShiftOut(data);
}

void HC595_SendMultipleBytes(uint8_t *pData, uint8_t size)
{
    uint8_t i;
    for (i = 0; i < size; i++)
    {
        HC595_SendByte(pData[i]);
    }
    HC595_Latch();
}