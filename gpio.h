#ifndef GPIO_H
#define GPIO_H
#include "build-in.h"
#include "BS86C08C.H"

#define ON              1
#define OFF             0
#define PULL_UP_ON      1
#define PULL_UP_OFF     0
#define GPIO_INPUT      1
#define GPIO_OUTPUT     0
#define PORT_A          0
#define PORT_B          1
#define PORT_C          2
#define PORT_D          3



void Gpio_Init(unsigned char port, unsigned char pin, unsigned char direction, unsigned char pull_up);
void Gpio_Write(unsigned char port, unsigned char pin, unsigned char value);
void Gpio_Read(unsigned char port, unsigned char pin, unsigned char *value);
void Gpio_Toggle(unsigned char port, unsigned char pin);



#endif