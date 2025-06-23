#include "main.h"
#include <string.h>
#include <stdio.h>
//boot

#define ADDRESS1 0x08008000
#define ADDRESS2 0x08080000
uint32_t tick;
uint8_t process;
uint8_t sum=0xFF;
uint32_t main_address;
uint32_t update_address;
uint32_t temp_update_address;
uint8_t data=0xff;
enum {BOOT,UPDATE};

void Set_MSP(uint32_t topOfMainStack);
void JumpMemory(uint32_t address);
void BootCheck(void);
void FlashInit(void);
void FlashUnlock(void);
void FlashLock(void);
void FlashEraseSector(uint32_t sector);
void FlashWriteWord(uint32_t address, uint32_t data);
void FlashWriteBuffer(uint32_t flash_addr, const uint8_t* buffer, uint32_t length);
uint8_t calc_checksum(uint8_t *data, uint8_t length);
  
int main(void)
{
    Sys_init();
    delay_ms(100);
    ClockInit(PLL, AHBDIV1, APB1DIV4, APB2DIV2, PLLENABLE, HSE, 4, 168, 2);
    GpioInitOutput(GPIOA, GPIO_PIN_6, PUSHPULL, MEDIUMSPEED);
    
    GpioInitITinput(GPIOE, GPIO_PIN_4, PULLUP, RISINGFALLING, 15);
    GpioInitITinput(GPIOE, GPIO_PIN_3, PULLUP, RISINGFALLING, 15);
    GpioWritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);

    FlashInit();
    delay_ms(10);
    
    Uart1Init(115200); 
    Uart1SendString("in boot\n");
    process= BOOT;
    data = (uint8_t)((*(volatile uint32_t *)0x08004000)&0x000000ff);
    if(data==0)
    {
      main_address=ADDRESS2;
      update_address=ADDRESS1;
    }
    else
    {
      main_address=ADDRESS1;
      update_address=ADDRESS2;
    }
    temp_update_address = update_address;
    while (1)
    {
        BootCheck();
    }
}
uint8_t calc_checksum(uint8_t *data, uint8_t length) 
{
    uint16_t sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        sum += data[i];
    }
    uint8_t checksum = (uint8_t)((~sum + 1) & 0xFF);  // Bù 2
    return checksum;
}
void Set_MSP(uint32_t topOfMainStack) 
{
    __asm volatile (
        "msr msp, %0\n"
        "bx lr\n"
        :
        : "r" (topOfMainStack)
    );
}
void JumpMemory(uint32_t address) 
{
    uint32_t app_sp = *(volatile uint32_t*)(address);
    uint32_t app_pc = *(volatile uint32_t*)(address + 4);
    typedef void (*pFunction)(void);
    pFunction start_app = (pFunction)app_pc;
     __asm volatile ("msr msp, %0" : : "r" (app_sp));
    SCB->VTOR = address;
    start_app();
    
}
void BootCheck(void)
{
  switch (process)
  {
  case BOOT:
    if(tick>10000)
    {
      JumpMemory(main_address);
      
      tick=0;
    }
    else        
    {
      if(Uart1.buffer[0]=='j' && Uart1.buffer[1]=='u' && Uart1.buffer[2]=='m' && Uart1.buffer[3]=='p') 
      {
        JumpMemory(main_address);
        tick=0;
      }
      else if(Uart1.buffer[0]=='c' && Uart1.buffer[1]=='h' && Uart1.buffer[2]=='e' && Uart1.buffer[3]=='c'&& Uart1.buffer[4]=='k') 
      {
        if(data==0)
        {
          
          FlashEraseSector(2);
          FlashEraseSector(3);
          FlashEraseSector(4);
          FlashEraseSector(5);
          FlashEraseSector(6);
          FlashEraseSector(7);
        }
        else
        {
          FlashEraseSector(8);
          FlashEraseSector(9);
          FlashEraseSector(10);
          FlashEraseSector(11);
        }
        process=UPDATE;
        Uart1SendString("ok");
        Uart1ClearBuffer();
        tick=0;
      }
      
    }
    tick++;
    Systick_delay_ms(1);
    if(tick%1000 == 0)
    {
       Uart1SendString("bootting \n");
    }
    break;
  case UPDATE:
    if(Uart1.buffer[17]==0xAA)
    {
      sum = calc_checksum(Uart1.buffer,16);
      if(sum == Uart1.buffer[16])
      {
        //// save data
        FlashWriteBuffer(temp_update_address,Uart1.buffer,16);
        Systick_delay_ms(10);
        temp_update_address+=0x10;
        //// 
        Uart1ClearBuffer();
        Uart1SendString("ok");  
      }
      if(sum==0x60 && Uart1.buffer[0]==0xAA && Uart1.buffer[17]==0xAA)
      {
       
        if(data==0)
        {
          data = 0xff;
          
        }
        else
        {
          data=0;
          
        }
        FlashEraseSector(1);
        Systick_delay_ms(10);
        FlashWriteBuffer(0x08004000,&data,1);
        Systick_delay_ms(10);
        JumpMemory(update_address);
      }
    }
    break;
  default:
    break;
  }
}
void FlashInit(void)
{
  FLASH->ACR |= (5<<0) | (1<<8) | (1<<9) | (1<<10);
}
void FlashUnlock(void)
{
  if (FLASH->CR & (1UL << 30))  
  {
      FLASH->KEYR = 0x45670123;
      FLASH->KEYR = 0xCDEF89AB;

      while (FLASH->CR & (1UL << 31));  
  }
}
void FlashLock(void)
{
  FLASH->CR |= (1UL<<31);
}
void FlashEraseSector(uint32_t sector) 
{
    // 1. Unlock flash n?u dang khóa
    if (FLASH->CR & (1U << 31)) {
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }

    // 2. Ch? flash s?n sàng
    while (FLASH->SR & (1U << 16)); // BSY

    // 3. Xóa c? l?i cu n?u có (EOP, PGPERR, WRPRTERR)
    FLASH->SR |= (1U << 5) | (1U << 4) | (1U << 2);

    // 4. Thi?t l?p ch? d? xóa sector
    FLASH->CR &= ~((0xF << 3) | (0x3 << 8));  // Clear SNB và PSIZE
    FLASH->CR |= (1U << 1);                  // SER = 1
    FLASH->CR |= (sector << 3);              // Ch?n sector
    FLASH->CR |= (1U << 16);                 // STRT = 1 (start erase)

    // 5. Ch? xóa xong
    while (FLASH->SR & (1U << 16));  // BSY

    // 6. Clear bit SER sau khi xóa
    FLASH->CR &= ~(1U << 1);

    // 7. Lock l?i n?u c?n
    FLASH->CR |= (1U << 31);  // LOCK
}
void FlashWriteWord(uint32_t address, uint32_t data)
{
    FlashUnlock();

    while (FLASH->SR & (1U << 16)); // BSY

    FLASH->CR &= ~(0x3 << 8);       // clear PSIZE
    FLASH->CR |=  (0x2 << 8);       // set PSIZE = 32-bit

    FLASH->CR |= (1U << 0);         // set PG
    *(__IO uint32_t*)address = data;
    while (FLASH->SR & (1U << 16)); // wait for write done
    FLASH->CR &= ~(1U << 0);        // clear PG

    FlashLock();
}

void FlashWriteBuffer(uint32_t flash_addr, const uint8_t* buffer, uint32_t length) 
{
    FlashUnlock();
    if (FLASH->CR & (1U << 31)) { 
        FLASH->KEYR = 0x45670123;
        FLASH->KEYR = 0xCDEF89AB;
    }

    while (FLASH->SR & (1U << 16)); 

    FLASH->CR &= ~(0x3 << 8);  
    FLASH->CR |=  (0x2 << 8);  

    FLASH->CR |= (1U << 0);  

    for (uint32_t i = 0; i < length; i += 4) {
        uint32_t word = 0xFFFFFFFF;

        for (int j = 0; j < 4; j++) {
            if ((i + j) < length) {
                ((uint8_t*)&word)[j] = buffer[i + j];
            }
        }

        *(__IO uint32_t*)(flash_addr + i) = word;


        while (FLASH->SR & (1U << 16));  // BSY
    }
    FLASH->CR &= ~(1U << 0); 
    FlashLock();
}