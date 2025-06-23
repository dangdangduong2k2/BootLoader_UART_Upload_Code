#ifndef __UART_H__
#define __UART_H__

#include "gpio.h"

typedef struct
{
    uint8_t datauart;
    uint8_t index;
    
    uint8_t buffer[10];
    
} uart;

extern uart Uart1;
void Uart1Init(uint32_t baudrate);
void Uart1Sendchar(uint8_t ch);
void Uart1SendString(uint8_t* str);
#endif