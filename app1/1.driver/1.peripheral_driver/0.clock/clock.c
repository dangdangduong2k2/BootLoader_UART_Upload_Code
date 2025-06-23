#include "clock.h"

void ClockInit(uint8_t SystemClockSource,//HSI,HSE,PLL
                uint8_t AHB,
                uint8_t APB1,
                uint8_t APB2,
                uint8_t PLLEnable,//HSI,HSE
                uint8_t ClockInputPLL,
                uint16_t PLLM,
                uint16_t PLLN,
                uint16_t PLLP)
{
    uint32_t start_tick = 0;
    /*HSE or HSI on*/
    if(ClockInputPLL==HSI || SystemClockSource == HSI)
    {
        RCC->CR |= 1<<0;  
        start_tick = Get_systick();
        while (!(RCC->CR & (1<<1)))
        {
            if(Get_systick()-start_tick > 1000)
            {
                start_tick = 0;
                break;
            }
        }
    }
    else if(ClockInputPLL==HSE || SystemClockSource == HSE)
    {        
        RCC->CR |= 1<<16;  
        start_tick = Get_systick();
        while (!(RCC->CR & (1<<17)))
        {
            if(Get_systick()-start_tick > 1000)
            {
                start_tick = 0;
                break;
            }
        }
    }  
    /*Power CLock On*/
    RCC->APB1ENR |= 1<<28;
    /*Voltage scaling config*/
    PWR->CR |= 3<<14;
    /*FLASH PREFETCH*/
    FLASH->ACR = (1<<8) | (1<<9)| (1<<10)| (5<<0);
    /*Prescaler config*/
    RCC->CFGR |= (AHB<<4); /*AHB PR 1*/
    RCC->CFGR |= (APB1<<10); /*APB1 PR 4*/
    RCC->CFGR |= (APB2<<13); /*APB2 PR 2*/
    /*PLL MNP*/
    RCC->PLLCFGR = (PLLM <<0) | (PLLN << 6) | ((PLLP/2-1) <<16); //25/336/2
    if(ClockInputPLL==HSI)
    {
        /*PLL use HSE or HSI*/
        RCC->PLLCFGR &= ~(1<<22);
    }
    else if(ClockInputPLL==HSE)
    {
        /*PLL use HSE or HSE*/
        RCC->PLLCFGR |= (1<<22);
    }  
    /*PLL On*/
    if(PLLEnable==PLLDISABLE)
    {
        RCC->CR &= ~(1<<24);
    }
    else if (PLLEnable==PLLENABLE)
    {
        RCC->CR |= (1<<24);
        start_tick = Get_systick();
        while (!(RCC->CR & (1<<25)))//wait flag
        {
            if(Get_systick()-start_tick > 1000)
            {
                start_tick = 0;
                break;
            }
        }

    }
    /*PLL to sysclk*/
    RCC->CFGR |= (SystemClockSource<<0);
    start_tick = Get_systick();
    while (!(RCC->CFGR & (SystemClockSource<<2))) //wait flag
    {
        if(Get_systick()-start_tick > 1000)
        {
            start_tick = 0;
            break;
        }
    }

    SystemClock = 0;           // System clock (final system clock)
    HCLK =0;
    uint32_t PLLClock = 0;     // Clock from PLL
    uint32_t PLLClockOutput = 0;  // Output clock of PLL


    // Determine the system clock source
    if (SystemClockSource == HSI) 
    {   // Use HSI (HSI = 16 MHz)
        SystemClock = HSI_FREQ;
    }
    else if (SystemClockSource == HSE) 
    {   // Use HSE
        SystemClock = HSE_FREQ;
    }
    else if (SystemClockSource == PLL)
    {   // Use PLL
        if (PLLEnable == PLLENABLE) 
        {
            // Calculate PLL output clock
            if (ClockInputPLL == HSI) 
            {   // If PLL source is HSI
                PLLClock = HSI_FREQ;
            }
            else if (ClockInputPLL == HSE) 
            {   // If PLL source is HSE
                PLLClock = HSE_FREQ;
            }
            // Calculate PLL clock before division by PLLP
            PLLClockOutput = (PLLClock / PLLM) * PLLN;
            // Divide PLLClockOutput by PLLP to get system clock
            PLLClockOutput = PLLClockOutput / PLLP;
            // System clock is the output of PLL
            SystemClock = PLLClockOutput;
            HCLK = SystemClock/GET_AHB_DIV(AHB);
            PCLK1 = HCLK/GET_APB1_DIV(APB1);
            PCLK2 = HCLK/GET_APB2_DIV(APB2);
        }
    }
    //re init systick
    Systick_init();
}

