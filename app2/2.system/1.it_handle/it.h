#ifndef __IT_H__
#define __IT_H__    


#include "peripheral_driver_handle.h"

#define clr_line0_flag() do{EXTI->PR |= (1<<0);}while(0)
#define clr_line1_flag() do{EXTI->PR |= (1<<1);}while(0)
#define clr_line2_flag() do{EXTI->PR |= (1<<2);}while(0)
#define clr_line3_flag() do{EXTI->PR |= (1<<3);}while(0)
#define clr_line4_flag() do{EXTI->PR |= (1<<4);}while(0)
#define clr_line5_flag() do{EXTI->PR |= (1<<5);}while(0)
#define clr_line6_flag() do{EXTI->PR |= (1<<6);}while(0)
#define clr_line7_flag() do{EXTI->PR |= (1<<7);}while(0)
#define clr_line8_flag() do{EXTI->PR |= (1<<8);}while(0)
#define clr_line9_flag() do{EXTI->PR |= (1<<9);}while(0)
#define clr_line10_flag() do{EXTI->PR |= (1<<10);}while(0)
#define clr_line11_flag() do{EXTI->PR |= (1<<11);}while(0)
#define clr_line12_flag() do{EXTI->PR |= (1<<12);}while(0)
#define clr_line13_flag() do{EXTI->PR |= (1<<13);}while(0)
#define clr_line14_flag() do{EXTI->PR |= (1<<14);}while(0)
#define clr_line15_flag() do{EXTI->PR |= (1<<15);}while(0)

#define line0_is_pending() ((EXTI->PR & (1<<0)) != 0)
#define line1_is_pending() ((EXTI->PR & (1<<1)) != 0)
#define line2_is_pending() ((EXTI->PR & (1<<2)) != 0)
#define line3_is_pending() ((EXTI->PR & (1<<3)) != 0)
#define line4_is_pending() ((EXTI->PR & (1<<4)) != 0)
#define line5_is_pending() ((EXTI->PR & (1<<5)) != 0)
#define line6_is_pending() ((EXTI->PR & (1<<6)) != 0)
#define line7_is_pending() ((EXTI->PR & (1<<7)) != 0)
#define line8_is_pending() ((EXTI->PR & (1<<8)) != 0)
#define line9_is_pending() ((EXTI->PR & (1<<9)) != 0)
#define line10_is_pending() ((EXTI->PR & (1<<10)) != 0)
#define line11_is_pending() ((EXTI->PR & (1<<11)) != 0)
#define line12_is_pending() ((EXTI->PR & (1<<12)) != 0)
#define line13_is_pending() ((EXTI->PR & (1<<13)) != 0)
#define line14_is_pending() ((EXTI->PR & (1<<14)) != 0)
#define line15_is_pending() ((EXTI->PR & (1<<15)) != 0)


#endif