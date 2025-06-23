#include "system_handle.h"

uint32_t SystemClock;
uint32_t HCLK;  //AHB
uint32_t PCLK1; 
uint32_t PCLK2;

__IO uint32_t SysTick;

void Systick_init(void)
{
    STK->CTRL &= ~(1<<0); // Disable systick
    STK->CTRL |= (1<<2); // use core clock, External Clock (AHB/8) :0, Processor Clock (AHB) :1
    STK->CTRL |= (1<<1); // Enable interrupt
    STK->LOAD = 0x000000; // clear reload value
    STK->VAL = 0x000000; // clear current value
    STK->LOAD = (HCLK / 1000) - 1; // LOAD = HCLK*target(ms) - 1: target 1ms: HCLK/1000-1
    STK->CTRL |= (1<<0); // Enable systick    
}
uint32_t Get_systick(void)
{
    return SysTick;
}
void Sys_init(void)
{   
    SystemClock=HSI_FREQ;
    HCLK=SystemClock/1;
    Systick_init();
}
void Systick_delay_ms(uint32_t delay)
{
    uint32_t start_tick = 0;
    start_tick = Get_systick();
    while((Get_systick()-start_tick) != delay);
}