#include "gpio.h"
/*GPIO*/
static void GpioEnable(GPIO_TypeDef *GPIOx)
{
    switch ((uint32_t)GPIOx)
    {
        case (uint32_t)GPIOA:
            RCC->AHB1ENR |= (1 << 0); 
            break;
        case (uint32_t)GPIOB:
            RCC->AHB1ENR |= (1 << 1); 
            break;
        case (uint32_t)GPIOC:
            RCC->AHB1ENR |= (1 << 2); 
            break;
        case (uint32_t)GPIOD:
            RCC->AHB1ENR |= (1 << 3); 
            break;
        case (uint32_t)GPIOE:
            RCC->AHB1ENR |= (1 << 4); 
            break;
        case (uint32_t)GPIOF:
            RCC->AHB1ENR |= (1 << 5); 
            break;
        case (uint32_t)GPIOG:
            RCC->AHB1ENR |= (1 << 6); 
            break;
        case (uint32_t)GPIOH:
            RCC->AHB1ENR |= (1 << 7);
            break;
        case (uint32_t)GPIOI:
            RCC->AHB1ENR |= (1 << 8); 
            break;
        default:
            break;
    }
}

void GpioInitOutput(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t OutType,
                    uint8_t OutSpeed)
{
    GpioEnable(GPIOx);
    GPIOx->MODER   |= OUTPUT   << (Pin*2);
    GPIOx->OTYPER  |= OutType  << (Pin);
    GPIOx->OSPEEDR |= OutSpeed << (Pin*2);
}

void GpioInitALT(GPIO_TypeDef *GPIOx,
                 uint8_t Pin,
                 uint8_t OutType,
                 uint8_t OutSpeed)
{
    GpioEnable(GPIOx);
    GPIOx->MODER   |= ALT   << (Pin*2);
    GPIOx->OTYPER  |= OutType  << (Pin);
    GPIOx->OSPEEDR |= OutSpeed << (Pin*2);
}

void GpioInitInput(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t Pullmode)
{
    GpioEnable(GPIOx);
    GPIOx->MODER   |= INPUT    << (Pin*2);
    GPIOx->PUPDR   |= Pullmode << (Pin*2);
}

void GpioInitITinput(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t Pullmode,
                    uint8_t TrigerMode,
                    uint8_t Priority)
{
    //Config GPIO
    GpioInitInput(GPIOx, Pin, Pullmode);
    // Enable SYSCFG clock
    RCC->APB2ENR |= (1 << 14);

    // Scan IO port to get GPIO_bit_value
    uint8_t GPIO_bit_value = 0xFF;
    switch ((uint32_t)GPIOx)
    {
        case (uint32_t)GPIOA:
            GPIO_bit_value = 0;
            break;
        case (uint32_t)GPIOB:
            GPIO_bit_value = 1;
            break;
        case (uint32_t)GPIOC:
            GPIO_bit_value = 2;
            break;  
        case (uint32_t)GPIOD:
            GPIO_bit_value = 3; 
            break;
        case (uint32_t)GPIOE:   
            GPIO_bit_value = 4;
            break;
        case (uint32_t)GPIOF:
            GPIO_bit_value = 5;
            break;
        case (uint32_t)GPIOG:
            GPIO_bit_value = 6;
            break;
        case (uint32_t)GPIOH:
            GPIO_bit_value = 7;
            break;
        case (uint32_t)GPIOI:
            GPIO_bit_value = 8;
            break;
        
        default:
            break;
    }
    // set bit register follow GPIO_bit_value
    // set bit register follow GPIO_bit_value
    SYSCFG->EXTICR[Pin / 4] &= ~(0xF << (Pin % 4) * 4); // clear 4 bit pin in register
    SYSCFG->EXTICR[Pin / 4] |= (GPIO_bit_value << (Pin % 4) * 4); // set 4 bit pin in register

    // enable interrupt 
    EXTI->IMR  |= (1 << Pin);  
    switch (TrigerMode)
    {
        case RISING:
            EXTI->RTSR |=  (1 << Pin);  // Kích hoạt cạnh lên (Rising edge)    
            EXTI->FTSR &= ~(1 << Pin); // Không kích hoạt cạnh xuống (Falling edge)
            break;
        case FALLLING:
            EXTI->FTSR &= ~(1 << Pin);  // Kích hoạt cạnh xuống (Falling edge)
            EXTI->RTSR |=  (1 << Pin); // Không kích hoạt cạnh lên (Rising edge)
            break;
        case RISINGFALLING:
            EXTI->RTSR |=  (1 << Pin);  // Kích hoạt cạnh lên (Rising edge)
            EXTI->FTSR |=  (1 << Pin);  // Kích hoạt cạnh xuống (Falling edge)
            break;
        default:
            break;
    }
    // Set priority
    if(15<Priority)
    {
        Priority=15;
    }
  
    switch (Pin)
    {
        case GPIO_PIN_0:
            NVIC->IP[6] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<6);
            break;
        case GPIO_PIN_1:
            NVIC->IP[7] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<7);
            break;
        case GPIO_PIN_2:    
            NVIC->IP[8] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<8);
            break;
        case GPIO_PIN_3:
            NVIC->IP[9] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<9);
            break;
        case GPIO_PIN_4:
            NVIC->IP[10] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<10);
            break;
        case GPIO_PIN_5:
            NVIC->IP[23] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<23);
            break;
        case GPIO_PIN_6:
            NVIC->IP[23] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<23);
            break;
        case GPIO_PIN_7:
            NVIC->IP[23] |= (Priority<<4);  
            NVIC->ISER[0] |= (1<<23);
            break;
        case GPIO_PIN_8:
            NVIC->IP[23] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<23);
            break;
        case GPIO_PIN_9:
            NVIC->IP[23] |= (Priority<<4);
            NVIC->ISER[0] |= (1<<23);
            break;
        case GPIO_PIN_10:
            NVIC->IP[40] |= (Priority<<4);
            NVIC->ISER[1] |= (1<<8);
            break;  
        case GPIO_PIN_11:
            NVIC->IP[40] |= (Priority<<4);  
            NVIC->ISER[1] |= (1<<9);
            break;  
        case GPIO_PIN_12:   
            NVIC->IP[40] |= (Priority<<4);
            NVIC->ISER[1] |= (1<<10);
            break;
        case GPIO_PIN_13:
            NVIC->IP[40] |= (Priority<<4);
            NVIC->ISER[1] |= (1<<11);
            break;
        case GPIO_PIN_14:
            NVIC->IP[40] |= (Priority<<4);
            NVIC->ISER[1] |= (1<<12);
            break;
        case GPIO_PIN_15:
            NVIC->IP[40] |= (Priority<<4);
            NVIC->ISER[1] |= (1<<13);
            break;
        default:
            break;
    }
    __asm("cpsie i");
}

void GpioWritePin(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t State)
{
   if(State==GPIO_PIN_SET)
   {
      GPIOx->BSRR    |= 1 << Pin;
      GPIOx->BSRR    &= ~(1 << (Pin+16));
   }
   if(State==GPIO_PIN_RESET)
   {
      GPIOx->BSRR    &= ~(1 << Pin);
      GPIOx->BSRR    |= (1 << (Pin+16));
   }
}

uint8_t GpioReadPin(GPIO_TypeDef *GPIOx,
                    uint8_t Pin)
{
   return ((GPIOx->IDR) >> Pin) & 0x1UL;
}


