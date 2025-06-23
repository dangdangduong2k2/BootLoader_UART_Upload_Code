#ifndef __BOOT_H__
#define __BOOT_H__

#include "peripheral_driver_handle.h"

#define BOOT_ADDRESS 0x08000000

void Set_MSP(uint32_t topOfMainStack);
void JumpMemory(void);
void BootCheck(void);

#endif 