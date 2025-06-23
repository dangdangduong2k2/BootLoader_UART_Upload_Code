#include "boot.h"
void BootCheck(void)
{
  if(Uart1.buffer[0]=='b' && Uart1.buffer[1]=='o' && Uart1.buffer[2]=='o' && Uart1.buffer[3]=='t') JumpMemory();
}


void Set_MSP(uint32_t topOfMainStack) {
    __asm volatile (
        "msr msp, %0\n"
        "bx lr\n"
        :
        : "r" (topOfMainStack)
    );
}
void JumpMemory(void) {
    uint32_t app_sp = *(volatile uint32_t*)(BOOT_ADDRESS);
    uint32_t app_pc = *(volatile uint32_t*)(BOOT_ADDRESS + 4);
    typedef void (*pFunction)(void);
    pFunction start_app = (pFunction)app_pc;
     __asm volatile ("msr msp, %0" : : "r" (app_sp));
    SCB->VTOR = BOOT_ADDRESS;
    start_app();
}