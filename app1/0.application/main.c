#include "main.h"
#include <string.h>
#include <stdio.h>
//aplication

int main(void)
{      
    
    Sys_init();
    delay_ms(100);
    ClockInit(PLL, AHBDIV1, APB1DIV4, APB2DIV2, PLLENABLE, HSE, 4, 168, 2);
    GpioInitOutput(GPIOA, GPIO_PIN_6, PUSHPULL, MEDIUMSPEED);
    
    GpioInitITinput(GPIOE, GPIO_PIN_4, PULLUP, RISINGFALLING, 15);
    GpioInitITinput(GPIOE, GPIO_PIN_3, PULLUP, RISINGFALLING, 15);

    
    Uart1Init(115200);  
    GpioWritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    delay_ms(1000);
    Uart1SendString("in app\n");
    while (1)
    {      
      BootCheck();
    }
}

