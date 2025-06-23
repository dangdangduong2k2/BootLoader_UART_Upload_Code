#include "uart.h"
float uart_div;
uint16_t mantisa, fraction;
uart Uart1;

void Uart1Init(uint32_t baudrate){
  
    RCC->APB2ENR |= (1 << 4);      // USART1

    // Init PA9 (TX), PA10 (RX) as AF
    GpioInitALT(GPIOA, GPIO_PIN_9, NOPULL, HIGHSPEED);
    GpioInitALT(GPIOA, GPIO_PIN_10, NOPULL, HIGHSPEED);

    // Set AF7 for USART1 (PA9, PA10)
    GPIOA->AFR[1] &= ~(0xF << 4);  // Clear PA9
    GPIOA->AFR[1] |=  (7 << 4);
    GPIOA->AFR[1] &= ~(0xF << 8);  // Clear PA10
    GPIOA->AFR[1] |=  (7 << 8);

    // Disable USART before config
    USART1->CR1 &= ~(1 << 13);

    // Baudrate setting
    uart_div = (float)PCLK2 / (16 * baudrate);
    mantisa = (uint16_t)uart_div;
    fraction = (uint16_t)((uart_div - mantisa) * 16);
    USART1->BRR = (mantisa << 4) | (fraction & 0xF);

    // Enable TX, RX, RXNE interrupt
    USART1->CR1 |= (1 << 3) | (1 << 2) | (1 << 5);

    // Enable USART
    USART1->CR1 |= (1 << 13);
    NVIC->ISER[1] = (1 << 5); // iser[0]: IP0->IP31,iser[1]: IP32->IP63; 37-32=5 -> 1<<5 
    NVIC->IP[37] = (1 << 4);  // Priority = 1
}

void Uart1Sendchar(uint8_t ch)
{
    uint32_t start_tick;
    start_tick = Get_systick();
    while (!((USART1->SR >> 7) & 1)) //wait flag
    {
        if(Get_systick()-start_tick > 100)
        {
            start_tick = 0;
            break;
        }
    }
    USART1->DR = ch;
}
void Uart1SendString(uint8_t* str)
{
    uint32_t start_tick;
    start_tick = Get_systick();
    while (*str != '\0')
    {
        if(Get_systick()-start_tick > 100)
        {
            start_tick = 0;
            break;
        }
        Uart1Sendchar(*str); 
        str++; 
    }
}
void Uart1ClearBuffer(void)
{
  for(int i=0; i<18;i++)
  {
    Uart1.datauart =0x00;
    Uart1.buffer[i]=0x00;
    Uart1.index=0;
  }
}