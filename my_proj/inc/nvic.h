#ifndef NVIC_H_
#define NVIC_H_

#include "memorymap.h"

/***** NVIC Regs **********************************************************/
#define NVIC_ISER(iser_id) 				MMIO32(NVIC_BASE + 0x000 + (iser_id * 4))
#define NVIC_ICER(icer_id) 				MMIO32(NVIC_BASE + 0x080 + (icer_id * 4))
#define NVIC_ISPR(ispr_id) 				MMIO32(NVIC_BASE + 0x100 + (ispr_id * 4))
#define NVIC_ICPR(icpr_id) 				MMIO32(NVIC_BASE + 0x180 + (icpr_id * 4))
#define NVIC_IABR(iabr_id) 			  MMIO32(NVIC_BASE + 0x200 + (iabr_id * 4))
#define NVIC_IPR(ipr_id) 					MMIO8(NVIC_BASE + 0x300 + ipr_id)

/***** User Interrupts ****************************************************/
#define NVIC_NVIC_WWDG_IRQ 							0
#define NVIC_PVD_IRQ 			 							1
#define NVIC_TAMP_STAMP_IRQ							2
#define NVIC_RTC_WKUP_IRQ 							3
#define NVIC_FLASH_IRQ 									4
#define NVIC_RCC_IRQ			 							5
#define NVIC_EXTI0_IRQ		 							6
#define NVIC_EXTI1_IRQ 									7
#define NVIC_EXTI2_IRQ 									8
#define NVIC_EXTI3_IRQ 									9
#define NVIC_EXTI4_IRQ 									10
#define NVIC_DMA1_STREAM0_IRQ 					11
#define NVIC_DMA1_STREAM1_IRQ 					12
#define NVIC_DMA1_STREAM2_IRQ 					13
#define NVIC_DMA1_STREAM3_IRQ 					14
#define NVIC_DMA1_STREAM4_IRQ 					15
#define NVIC_DMA1_STREAM5_IRQ 					16
#define NVIC_DMA1_STREAM6_IRQ 					17
#define NVIC_ADC_IRQ 										18
#define NVIC_CAN1_TX_IRQ 								19
#define NVIC_CAN1_RX0_IRQ 							20
#define NVIC_CAN1_RX1_IRQ 							21
#define NVIC_CAN1_SCE_IRQ 							22
#define NVIC_EXTI9_5_IRQ 								23
#define NVIC_TIM1_BRK_TIM9_IRQ 					24
#define NVIC_TIM1_UP_TIM10_IRQ 					25
#define NVIC_TIM1_TRG_COM_TIM11_IRQ 		26
#define NVIC_TIM1_CC_IRQ 								27
#define NVIC_TIM2_IRQ 									28
#define NVIC_TIM3_IRQ 									29
#define NVIC_TIM4_IRQ 									30
#define NVIC_I2C1_EV_IRQ 								31
#define NVIC_I2C1_ER_IRQ 								32
#define NVIC_I2C2_EV_IRQ 								33
#define NVIC_I2C2_ER_IRQ 								34
#define NVIC_SPI1_IRQ 									35
#define NVIC_SPI2_IRQ 									36
#define NVIC_USART1_IRQ 								37
#define NVIC_USART2_IRQ 								38
#define NVIC_USART3_IRQ 								39
#define NVIC_EXT15_10_IRQ 							40
#define NVIC_RTC_ALARM_IRQ 							41
#define NVIC_USB_FS_WKUP_IRQ 						42
#define NVIC_TIM8_BRK_TIM12_IRQ 				43
#define NVIC_TIM8_UP_TIM13_IRQ 					44
#define NVIC_TIM8_TRG_COM_TIM14_IRQ 		45
#define NVIC_TIM8_CC_IRQ 								46
#define NVIC_DMA1_STREAM7_IRQ 					47
#define NVIC_FSMC_IRQ 									48
#define NVIC_SDIO_IRQ 									49
#define NVIC_TIM5_IRQ 									50
#define NVIC_SPI3_IRQ 									51
#define NVIC_UART4_IRQ 									52
#define NVIC_UART5_IRQ 									53
#define NVIC_TIM6_DAC_IRQ 							54
#define NVIC_TIM7_IRQ 									55
#define NVIC_DMA2_STREAM0_IRQ 					56
#define NVIC_DMA2_STREAM1_IRQ 					57
#define NVIC_DMA2_STREAM2_IRQ 					58
#define NVIC_DMA2_STREAM3_IRQ 					59
#define NVIC_DMA2_STREAM4_IRQ 					60
#define NVIC_ETH_IRQ 										61
#define NVIC_ETH_WKUP_IRQ 							62
#define NVIC_CAN2_TX_IRQ 								63
#define NVIC_CAN2_RX0_IRQ 							64
#define NVIC_CAN2_RX1_IRQ 							65
#define NVIC_CAN2_SCE_IRQ 							66
#define NVIC_OTG_FS_IRQ 								67
#define NVIC_DMA2_STREAM5_IRQ 					68
#define NVIC_DMA2_STREAM6_IRQ 					69
#define NVIC_DMA2_STREAM7_IRQ 					70
#define NVIC_USART6_IRQ 								71
#define NVIC_I2C3_EV_IRQ 								72
#define NVIC_I2C3_ER_IRQ 								73
#define NVIC_OTG_HS_EP1_OUT_IRQ 				74
#define NVIC_OTG_HS_EP1_IN_IRQ 					75
#define NVIC_OTG_HS_WKUP_IRQ 						76
#define NVIC_OTG_HS_IRQ 								77
#define NVIC_DMCI_IRQ 									78
#define NVIC_CRYP_IRQ 									79
#define NVIC_HASH_RNG_IRQ 							80



#endif