#include "it.h"


void EXTI0_IRQHandler(void)
{
    //start to do code

    //end to do code
    clr_line0_flag();
}
void EXTI1_IRQHandler(void)
{
    //start to do code

    //end to do code
    clr_line1_flag();
}
void EXTI2_IRQHandler(void)
{
    //start to do code

    //end to do code
    clr_line2_flag();
}
void EXTI3_IRQHandler(void)
{
    //start to do code

    //end to do code
    clr_line3_flag();
}
void EXTI4_IRQHandler(void)
{
    //start to do code
    
    //end to do code
    clr_line4_flag();
}
void EXTI9_5_IRQHandler(void)
{
    //line 5 interrupt
    if(line5_is_pending())
    {
        //start to do code

        //end to do code
        clr_line5_flag();   
    }
    //line 6 interrupt
    if(line6_is_pending())
    {
        //start to do code

        //end to do code
        clr_line6_flag();
    }
    //line 7 interrupt
    if(line7_is_pending())
    {
        //start to do code

        //end to do code
        clr_line7_flag();
    }   
    //line 8 interrupt
    if(line8_is_pending())  
    {
        //start to do code

        //end to do code
        clr_line8_flag();
    }
    //line 9 interrupt
    if(line9_is_pending())
    {
        //start to do code
        
        //end to do code
        clr_line9_flag();
    }
}
void EXTI15_10_IRQHandler(void)
{
    //line 10 interrupt
    if(line10_is_pending())
    {
        //start to do code

        //end to do code
        clr_line10_flag();
    }   
    //line 11 interrupt
    if(line11_is_pending())
    {
        //start to do code

        //end to do code
        clr_line11_flag();
    }
    //line 12 interrupt
    if(line12_is_pending())
    {
        //start to do code

        //end to do code
        clr_line12_flag();
    }
    //line 13 interrupt
    if(line13_is_pending())
    {
        //start to do code

        //end to do code
        clr_line13_flag();
    }
    //line 14 interrupt
    if(line14_is_pending())
    {
        //start to do code

        //end to do code
        clr_line14_flag();
    }
    //line 15 interrupt
    if(line15_is_pending())
    {
        //start to do code

        //end to do code
        clr_line15_flag();
    }
}
void USART1_IRQHandler(void)
{
    if (USART1->SR & (1<<5)) 
    {
        
        Uart1.datauart = USART1->DR;
        if (Uart1.datauart == 0x0A) //enter
        {    
            Uart1.index = 0;
        } 
        else 
        {
            Uart1.buffer[Uart1.index++] = Uart1.datauart;
            if (Uart1.index >= 10) 
            {  
                Uart1.index = 0;
            }
        }
    } 
}
void SysTick_Handler(void)
{
    SysTick++;
}
void hard_fault_handler_c(uint32_t *stack_frame)
{
  
  // Log giá tr? pc và các thanh ghi khác
  while(1);
}
void HardFault_Handler(void)
{
  __asm volatile(
    "TST LR, #4\n"
    "ITE EQ\n"
    "MRSEQ R0, MSP\n"
    "MRSNE R0, PSP\n"
    "B hard_fault_handler_c\n"
  );
}