#include "gpio.h"

void Gpio_Init(unsigned char port, unsigned char pin, unsigned char direction, unsigned char pull_up)
{
    if(port != PORT_A && port != PORT_B && port != PORT_C && port != PORT_D)
        return;
    if(pin > 7)
        return;
    unsigned char mark_pin = 0; 
    mark_pin =  (unsigned char)(1U << pin);
    switch (port) {
    case PORT_A:
        if (pin != 5 && pin != 6) {
            switch (direction) {
                case GPIO_INPUT:
                    _pac |= mark_pin;
                    break;
                case GPIO_OUTPUT:
                    _pac &= ~mark_pin;
                    break;
                default:
                    break;
            }
            switch (pull_up) {
                case PULL_UP_ON:
                    _papu |= mark_pin;
                    break;
                case PULL_UP_OFF:
                    _papu &= ~mark_pin;
                    break;
                default:
                    break;
            }
        }
        break;

    case PORT_B:
        switch (direction) {
            case GPIO_INPUT:
                _pbc |= mark_pin;
                break;
            case GPIO_OUTPUT:
                _pbc &= ~mark_pin;
                break;
            default:
                break;
        }
        switch (pull_up) {
            case PULL_UP_ON:
                _pbpu |= mark_pin;
                break;
            case PULL_UP_OFF:
                _pbpu &= ~mark_pin;
                break;
            default:
                break;
        }
        break;

    case PORT_C:
        switch (direction) {
            case GPIO_INPUT:
                _pcc |= mark_pin;
                break;
            case GPIO_OUTPUT:
                _pcc &= ~mark_pin;
                break;
            default:
                break;
        }
        switch (pull_up) {
            case PULL_UP_ON:
                _pcpu |= mark_pin;
                break;
            case PULL_UP_OFF:
                _pcpu &= ~mark_pin;
                break;
            default:
                break;
        }
        break;

    case PORT_D:
        switch (direction) {
            case GPIO_INPUT:
                _pdc |= mark_pin;
                break;
            case GPIO_OUTPUT:
                _pdc &= ~mark_pin;
                break;
            default:
                break;
        }
        switch (pull_up) {
            case PULL_UP_ON:
                _pdpu |= mark_pin; // Lưu ý: giữ nguyên tên biến _dcpu theo code gốc của bạn
                break;
            case PULL_UP_OFF:
                _pdpu &= ~mark_pin;
                break;
            default:
                break;
        }
        break;
            default:
                break;
    }
}
    
void Gpio_Write(unsigned char port, unsigned char pin, unsigned char value)
{
    if(port != PORT_A && port != PORT_B && port != PORT_C && port != PORT_D)
        return;
    if(pin > 7)
        return;
    unsigned char mark_pin = 0; 
    mark_pin =  (unsigned char)(1U << pin);
    switch (port) {
    case PORT_A:
        switch (value) {
            case ON:
                _pa |= mark_pin;
                break;
            case OFF:
                _pa &= ~mark_pin;
                break;
            default:
                break;
        }
        break;

    case PORT_B:
        switch (value) {
            case ON:
                _pb |= mark_pin;
                break;
            case OFF:
                _pb &= ~mark_pin;
                break;
            default:
                break;
        }
        break;

    case PORT_C:
        switch (value) {
            case ON:
                _pc |= mark_pin;
                break;
            case OFF:
                _pc &= ~mark_pin;
                break;
            default:
                break;
        }
        break;

    case PORT_D:
        switch (value) {
            case ON:
                _pd |= mark_pin;
                break;
            case OFF:
                _pd &= ~mark_pin;
                break;
            default:
                break;
        }
        break;

    default:
        break;
    }
}

void Gpio_Read(unsigned char port, unsigned char pin, unsigned char *value)
{
    if(port != PORT_A && port != PORT_B && port != PORT_C && port != PORT_D)
        return;
    if(pin > 7)
        return;
    unsigned char mark_pin = 0; 
    mark_pin =  (unsigned char)(1U << pin);
    switch (port) {
    case PORT_A:
        *value = (_pa & mark_pin) ? ON : OFF;
        break;

    case PORT_B:
        *value = (_pb & mark_pin) ? ON : OFF;
        break;

    case PORT_C:
        *value = (_pc & mark_pin) ? ON : OFF;
        break;

    case PORT_D:
        *value = (_pd & mark_pin) ? ON : OFF;
        break;

    default:
        break;
    }
}

void Gpio_Toggle(unsigned char port, unsigned char pin)
{
    if(port != PORT_A && port != PORT_B && port != PORT_C && port != PORT_D)
        return;
    if(pin > 7)
        return;
    unsigned char mark_pin = 0; 
    mark_pin =  (unsigned char)(1U << pin);
    switch (port) {
    case PORT_A:
        _pa ^= mark_pin;
        break;

    case PORT_B:
        _pb ^= mark_pin;
        break;

    case PORT_C:
        _pc ^= mark_pin;
        break;

    case PORT_D:
        _pd ^= mark_pin;
        break;

    default:
        break;
    }
}