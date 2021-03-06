#ifndef MEMORYMAP_H_
#define MEMORYMAP_H_

#include <stdint.h>

/***** Memory-mapped I/O macros ******************************************/
#define MMIO8(addr)					(*(volatile uint8_t *)(addr))
#define MMIO16(addr)				(*(volatile uint16_t *)(addr))
#define MMIO32(addr)				(*(volatile uint32_t *)(addr))
#define MMIO64(addr)				(*(volatile uint64_t *)(addr))

/***** NVIC Base *********************************************************/
#define NVIC_BASE 											0xE000E100

/***** Periphal base addresses *******************************************/

/***** Peripheral bus map *********************************/
#define PERIPH_BASE_APB1			0x40000000
#define PERIPH_BASE_APB2			0x40010000
#define PERIPH_BASE_AHB1			0x40020000
#define PERIPH_BASE_AHB2 			0x50000000

/****** APB1 Peripheral Map *******************************/
#define TIM2_BASE 					(PERIPH_BASE_APB1 + 0x0000)
#define TIM3_BASE 					(PERIPH_BASE_APB1 + 0x0400)
#define TIM4_BASE 					(PERIPH_BASE_APB1 + 0x0800)
#define TIM5_BASE 					(PERIPH_BASE_APB1 + 0x0C00)
#define TIM6_BASE 					(PERIPH_BASE_APB1 + 0x1000)
#define TIM7_BASE 					(PERIPH_BASE_APB1 + 0x1400)
#define TIM12_BASE 					(PERIPH_BASE_APB1 + 0x1800)
#define TIM13_BASE 					(PERIPH_BASE_APB1 + 0x1C00)
#define TIM14_BASE 					(PERIPH_BASE_APB1 + 0x2000)

#define RTC_BASE					(PERIPH_BASE_APB1 + 0x2800)
#define WWDG_BASE					(PERIPH_BASE_APB1 + 0x2C00)
#define IWDG_BASE					(PERIPH_BASE_APB1 + 0x3000)

#define SPI2_I2S2_BASE				(PERIPH_BASE_APB1 + 0x3800)
#define SPI3_I2S3_BASE				(PERIPH_BASE_APB1 + 0x3C00)

#define USART2_BASE					(PERIPH_BASE_APB1 + 0x4400)
#define USART3_BASE					(PERIPH_BASE_APB1 + 0x4800)
#define UART4_BASE					(PERIPH_BASE_APB1 + 0x4C00)
#define UART5_BASE					(PERIPH_BASE_APB1 + 0x5000)

#define I2C1_BASE					(PERIPH_BASE_APB1 + 0x5400)
#define I2C2_BASE					(PERIPH_BASE_APB1 + 0x5800)
#define I2C3_BASE					(PERIPH_BASE_APB1 + 0x5C00)

#define CAN1_BASE					(PERIPH_BASE_APB1 + 0x6400)
#define CAN2_BASE					(PERIPH_BASE_APB1 + 0x6800)

#define PWR_CTRL_BASE				(PERIPH_BASE_APB1 + 0x7000)
#define DAC_BASE					(PERIPH_BASE_APB1 + 0x7400)

#define UART7_BASE					(PERIPH_BASE_APB1 + 0x7800)
#define UART8_BASE					(PERIPH_BASE_APB1 + 0x7C00)

/* APB2 Peripheral Map */
#define TIM1_BASE					(PERIPH_BASE_APB2 + 0x0000)
#define TIM8_BASE					(PERIPH_BASE_APB2 + 0x0400)

#define USART1_BASE					(PERIPH_BASE_APB2 + 0x1000)
#define USART6_BASE					(PERIPH_BASE_APB2 + 0x1400)

#define ADC1_BASE					(PERIPH_BASE_APB2 + 0x2000)
#define ADC2_BASE					(PERIPH_BASE_APB2 + 0x2100)
#define ADC3_BASE					(PERIPH_BASE_APB2 + 0x2200)

#define SDIO_BASE					(PERIPH_BASE_APB2 + 0x2C00)

#define SPI1_BASE					(PERIPH_BASE_APB2 + 0x3000)
#define SPI4_BASE					(PERIPH_BASE_APB2 + 0x3400)

#define SYSCFG_BASE					(PERIPH_BASE_APB2 + 0x3800)
#define EXTI_BASE					(PERIPH_BASE_APB2 + 0x3C00)

#define TIM9_BASE					(PERIPH_BASE_APB2 + 0x4000)
#define TIM10_BASE					(PERIPH_BASE_APB2 + 0x4400)
#define TIM11_BASE					(PERIPH_BASE_APB2 + 0x4800)

#define SPI5_BASE					(PERIPH_BASE_APB2 + 0x5000)
#define SPI6_BASE					(PERIPH_BASE_APB2 + 0x5400)


/***** AHB1 Peripheral Map ***********************************/
#define GPIO_PORTA_BASE				(PERIPH_BASE_AHB1 + 0x0000)
#define GPIO_PORTB_BASE				(PERIPH_BASE_AHB1 + 0x0400)
#define GPIO_PORTC_BASE				(PERIPH_BASE_AHB1 + 0x0800)
#define GPIO_PORTD_BASE				(PERIPH_BASE_AHB1 + 0x0C00)
#define GPIO_PORTE_BASE				(PERIPH_BASE_AHB1 + 0x1000)
#define GPIO_PORTF_BASE				(PERIPH_BASE_AHB1 + 0x1400)
#define GPIO_PORTG_BASE				(PERIPH_BASE_AHB1 + 0x1800)
#define GPIO_PORTH_BASE				(PERIPH_BASE_AHB1 + 0x1C00)
#define GPIO_PORTI_BASE				(PERIPH_BASE_AHB1 + 0x2000)

#define CRC_BASE					(PERIPH_BASE_AHB1 + 0x3000)
#define RCC_BASE					(PERIPH_BASE_AHB1 + 0x3800)
#define FLASH_INTERFACE_BASE		(PERIPH_BASE_AHB1 + 0x3C00)
#define BKPSRAM_BASE				(PERIPH_BASE_AHB1 + 0x4000)

#define DMA1_BASE					(PERIPH_BASE_AHB1 + 0x6000)
#define DMA2_BASE					(PERIPH_BASE_AHB1 + 0x6400)
#define ETH_MAC_BASE				(PERIPH_BASE_AHB1 + 0x8000)
#define USB_OTG_HS_BASE				(PERIPH_BASE_AHB1 + 0x20000)


/***** AHB2 Peripheral Map ***********************************/
#define USB_OTG_FS_BASE				(PERIPH_BASE_AHB2 + 0x0000)
#define DCMI_BASE					(PERIPH_BASE_AHB2 + 0x50000)


#endif