#ifndef GPIO_H_
#define GPIO_H_

#include "memorymap.h"

/***** GPIO Base Regs ****************************************************/
#define GPIOA 						GPIO_PORTA_BASE
#define GPIOB 						GPIO_PORTB_BASE
#define GPIOC 						GPIO_PORTC_BASE
#define GPIOD 						GPIO_PORTD_BASE
#define GPIOE 						GPIO_PORTE_BASE
#define GPIOF 						GPIO_PORTF_BASE
#define GPIOG 						GPIO_PORTG_BASE
#define GPIOH 						GPIO_PORTH_BASE
#define GPIOI 						GPIO_PORTI_BASE

/***** GPIO Regs *********************************************************/
#define GPIO_MODER(gpio_base) 		MMIO32(gpio_base + 0x00)
#define GPIO_OTYPER(gpio_base) 		MMIO32(gpio_base + 0x04)
#define GPIO_OSPEEDR(gpio_base)		MMIO32(gpio_base + 0x08)
#define GPIO_PUPDR(gpio_base) 		MMIO32(gpio_base + 0x0C)
#define GPIO_IDR(gpio_base) 			MMIO32(gpio_base + 0x10)
#define GPIO_ODR(gpio_base) 			MMIO32(gpio_base + 0x14)
#define GPIO_BSRR(gpio_base) 			MMIO32(gpio_base + 0x18)
#define GPIO_LCKR(gpio_base) 			MMIO32(gpio_base + 0x1C)
#define GPIO_AFRL(gpio_base) 			MMIO32(gpio_base + 0x20)
#define GPIO_AFRH(gpio_base) 			MMIO32(gpio_base + 0x24)

/***** GPIO MODER Regs ***************************************************/
#define GPIO_MODE(x, mode) 			(mode << (2 * x))
#define GPIO_MODE_INPUT 				0
#define GPIO_MODE_OUTPUT 				1
#define GPIO_MODE_AF 						2
#define GPIO_MODE_ANALOG 				3

/***** GPIO OTYPER Regs ***************************************************/
#define GPIO_OTYPE(x, type) 		(type << x)
#define GPIO_TYPE_PP 						0
#define GPIO_TYPE_OD 						1

/***** GPIO OSPEEDR Regs **************************************************/
#define GPIO_OSPEED(x, speed) 	(speed << (x * 2))
#define GPIO_OSPEED_2MHZ 				0
#define GPIO_OSPEED_25MHZ 			1
#define GPIO_OSPEED_50MHZ 			2
#define GPIO_OSPEED_100MHZ 			3

/***** GPIO PUPDR Regs ****************************************************/
#define GPIO_PUPD(x, pupd) 			(pupd << (x * 2))
#define GPIO_PUPD_NONE  				0
#define GPIO_PUPD_PU 	 					1
#define GPIO_PUPD_PD 	 					2

/***** GPIO LCKR Regs ******************************************************/
#define GPIO_LCK_LCKK 					(1 << 16)

/***** GPIO AFR Regs ******************************************************/
#define GPIO_AFR(x, af) 				(af << (x * 4))
#define GPIO_AF0 								0
#define GPIO_AF1 								1
#define GPIO_AF2 								2
#define GPIO_AF3 								3
#define GPIO_AF4 								4
#define GPIO_AF5 								5
#define GPIO_AF6 								6
#define GPIO_AF7 								7
#define GPIO_AF8 								8
#define GPIO_AF9 								9
#define GPIO_AF10 							10
#define GPIO_AF11 							11
#define GPIO_AF12 							12
#define GPIO_AF13 							13
#define GPIO_AF14 							14
#define GPIO_AF15 							15




/***** Variables and Function Prototypes ***********************************/

void led_init();

#endif