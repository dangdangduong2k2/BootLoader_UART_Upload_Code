#ifndef __SYSTEM_HANDLE_H__
#define __SYSTEM_HANDLE_H__
#include "hw_stm32f407vet6.h"
/*Sys */
#define SYS_TIMEOUT 0u
#define SYS_OK      1u
#define SYS_ERROR   2u


#define HSI_FREQ          16000000   // Tần số HSI (16 MHz)
#define HSE_FREQ          8000000    // Tần số HSE (8 MHz)

#define HSI        0u
#define HSE        1u
#define PLL        2u

extern uint32_t SystemClock;
extern uint32_t HCLK;
extern __IO uint32_t SysTick;
extern uint32_t PCLK1; 
extern uint32_t PCLK2;

uint32_t Get_systick(void);
void Systick_init(void);
void Sys_init(void);
void Systick_delay_ms(uint32_t delay);

#endif