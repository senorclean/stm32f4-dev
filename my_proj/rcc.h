#ifndef RCC_H_
#define RCC_H_

#include "memorymap.h"

#define RCC_APB1RSTR 								MMIO32(RCC_BASE + 0x20)
#define RCC_AHB1ENR									MMIO32(RCC_BASE + 0x30)
#define RCC_APB1ENR 								MMIO32(RCC_BASE + 0x40)

#define RCC_APB1RSTR_I2C1RST 				(1 << 21)


#define RCC_AHB1ENR_GPIODEN 				(1 << 3)
#define RCC_AHB1ENR_GPIOBEN 				(1 << 1)
#define RCC_AHB1ENR_GPIOAEN 				(1 << 0)

#define RCC_APB1ENR_I2C1EN 					(1 << 21)
#define RCC_APB1ENR_USART2EN 				(1 << 17)
#define RCC_APB1ENR_TIM4EN 					(1 << 2)
#define RCC_APB1ENR_TIM3EN 					(1 << 1)


#endif