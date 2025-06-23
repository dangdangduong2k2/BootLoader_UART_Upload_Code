#include "delay.h"

void delay_ms(uint32_t ms) {
    uint32_t cycles_per_ms = SystemClock / 10000;
    uint32_t total_cycles = (cycles_per_ms*ms);
    while (total_cycles--) {
        __asm("nop");
    }
}