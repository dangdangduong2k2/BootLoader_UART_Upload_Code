#ifndef __GPIO_H__
#define __GPIO_H__
#include "system_handle.h"


/* GPIO */
#define INPUT             0u
#define OUTPUT            1u
#define ALT               2u
#define ANALOG            3u

#define PUSHPULL          0u
#define OPENDRAIN         1u

#define LOWSPEED          0u
#define MEDIUMSPEED       1u
#define HIGHSPEED         2u
#define VERYHIGHSPEED     3u

#define NOPULL            0u
#define PULLUP            1u
#define PULLDOWN          2u

#define GPIO_PIN_0        0u
#define GPIO_PIN_1        1u
#define GPIO_PIN_2        2u
#define GPIO_PIN_3        3u
#define GPIO_PIN_4        4u
#define GPIO_PIN_5        5u
#define GPIO_PIN_6        6u
#define GPIO_PIN_7        7u
#define GPIO_PIN_8        8u
#define GPIO_PIN_9        9u
#define GPIO_PIN_10       10u
#define GPIO_PIN_11       11u
#define GPIO_PIN_12       12u
#define GPIO_PIN_13       13u
#define GPIO_PIN_14       14u
#define GPIO_PIN_15       15u

#define GPIO_PIN_RESET    0u
#define GPIO_PIN_SET      1u

#define RISING            0u
#define FALLLING          1u
#define RISINGFALLING     2u

void GpioInitALT(GPIO_TypeDef *GPIOx,
                 uint8_t Pin,
                 uint8_t OutType,
                 uint8_t OutSpeed);
void GpioInitOutput(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t OutType,
                    uint8_t OutSpeed);
void GpioInitInput(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t Pullmode);
void GpioWritePin(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t State);
                    
void GpioInitITinput(GPIO_TypeDef *GPIOx,
                    uint8_t Pin,
                    uint8_t Pullmode,
                    uint8_t TrigerMode,
                    uint8_t Priority);
uint8_t GpioReadPin(GPIO_TypeDef *GPIOx,
                    uint8_t Pin);
#endif 











