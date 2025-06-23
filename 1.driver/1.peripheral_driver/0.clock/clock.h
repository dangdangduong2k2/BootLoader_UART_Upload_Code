#ifndef __CLOCK_H__
#define __CLOCK_H__
#include "system_handle.h"

#define PLLENABLE  1u
#define PLLDISABLE 0u

#define AHBDIV1    0u
#define AHBDIV2    8u
#define AHBDIV4    9u
#define AHBDIV8    10u
#define AHBDIV16   11u
#define AHBDIV64   12u
#define AHBDIV128  13u
#define AHBDIV256  14u
#define AHBDIV512  15u

#define GET_AHB_DIV(x) \
    ((x) == AHBDIV1 ? 1 : \
     (x) == AHBDIV2 ? 2 : \
     (x) == AHBDIV4 ? 4 : \
     (x) == AHBDIV8 ? 8 : \
     (x) == AHBDIV16 ? 16 : \
     (x) == AHBDIV64 ? 64 : \
     (x) == AHBDIV128 ? 128 : \
     (x) == AHBDIV256 ? 256 : \
     (x) == AHBDIV512 ? 512 : 1)

#define APB1DIV1    0u
#define APB1DIV2    4u
#define APB1DIV4    5u
#define APB1DIV8    6u
#define APB1DIV16   7u

#define GET_APB1_DIV(x) \
    ((x) == APB1DIV1 ? 1 : \
     (x) == APB1DIV2 ? 2 : \
     (x) == APB1DIV4 ? 4 : \
     (x) == APB1DIV8 ? 8 : \
     (x) == APB1DIV16 ? 16 : 1)
      
      
#define APB2DIV1    0u
#define APB2DIV2    4u
#define APB2DIV4    5u  
#define APB2DIV8    6u
#define APB2DIV16   7u

#define GET_APB2_DIV(x) \
    ((x) == APB2DIV1 ? 1 : \
     (x) == APB2DIV2 ? 2 : \
     (x) == APB2DIV4 ? 4 : \
     (x) == APB2DIV8 ? 8 : \
     (x) == APB2DIV16 ? 16 : 1)



void ClockInit(uint8_t SystemClockSource,
                uint8_t AHB,
                uint8_t APB1,
                uint8_t APB2,
                uint8_t PLLEnable,
                uint8_t ClockInputPLL,
                uint16_t PLLM,
                uint16_t PLLN,
                uint16_t PLLP);
#endif