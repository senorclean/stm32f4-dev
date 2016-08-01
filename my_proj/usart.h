#ifndef USART_H_
#define USART_H_

#include "stdint.h"
#include "memorymap.h"

/***** USART Base Regs **************************************************/
#define USART1 						USART1_BASE
#define USART2 						USART2_BASE
#define USART3 						USART3_BASE
#define UART4 						UART4_BASE
#define UART5 						UART5_BASE
#define USART6 						USART6_BASE

/***** USART Regs *******************************************************/
#define USART_SR(usart_base) 			MMIO32(usart_base + 0x00)
#define USART_DR(usart_base) 			MMIO32(usart_base + 0x04)
#define USART_BRR(usart_base) 		MMIO32(usart_base + 0x08)
#define USART_CR1(usart_base) 		MMIO32(usart_base + 0x0C)
#define USART_CR2(usart_base) 		MMIO32(usart_base + 0x10)
#define USART_CR3(usart_base) 		MMIO32(usart_base + 0x14)
#define USART_GTPR(usart_base) 		MMIO32(usart_base + 0x18)

/***** USART SR Regs ****************************************************/
#define USART_SR_CTS 				(1 << 9)
#define USART_SR_LBD 				(1 << 8)
#define USART_SR_TXE 				(1 << 7)
#define USART_SR_TC 				(1 << 6)
#define USART_SR_RXNE 				(1 << 5)
#define USART_SR_IDLE 				(1 << 4)
#define USART_SR_ORE 				(1 << 3)
#define USART_SR_NF 				(1 << 2)
#define USART_SR_FE 				(1 << 1)
#define USART_SR_PE 				(1 << 0)

/***** USART CR1 Regs ****************************************************/
#define USART_CR1_OVER8 			(1 << 15)
#define USART_CR1_UE 				(1 << 13)
#define USART_CR1_M 	 			(1 << 12)
#define USART_CR1_WAKE 	 			(1 << 11)
#define USART_CR1_PCE 	 			(1 << 10)
#define USART_CR1_PS 	 			(1 << 9)
#define USART_CR1_PEIE 	 			(1 << 8)
#define USART_CR1_TXEIE 			(1 << 7)
#define USART_CR1_TCIE 	 			(1 << 6)
#define USART_CR1_RXNEIE 			(1 << 5)
#define USART_CR1_IDLEIE 			(1 << 4)
#define USART_CR1_TE 	 			(1 << 3)
#define USART_CR1_RE 	 			(1 << 2)
#define USART_CR1_RWU 	 			(1 << 1)
#define USART_CR1_SBK 	 			(1 << 0)

/***** USART CR2 Regs ****************************************************/
#define USART_CR2_LINEN 			(1 << 14)

#define USART_CR2_STOPBITS_1 		(0 << 12)
#define USART_CR2_STOPBITS_0_5		(1 << 12)
#define USART_CR2_STOPBITS_2		(2 << 12)
#define USART_CR2_STOPBITS_1_5 		(3 << 12)

#define USART_CR2_CLKEN 			(1 << 11)
#define USART_CR2_CPOL 	 			(1 << 10)
#define USART_CR2_CPHA  			(1 << 9)
#define USART_CR2_LBCL  			(1 << 8)
#define USART_CR2_LBDIE 			(1 << 6)
#define USART_CR2_LBDL  			(1 << 5)

/***** USART CR3 Regs ****************************************************/
#define USART_CR3_ONEBIT 			(1 << 11)
#define USART_CR3_CTSIE 			(1 << 10)
#define USART_CR3_CTSE  			(1 << 9)
#define USART_CR3_RTSE  			(1 << 8)
#define USART_CR3_DMAT  			(1 << 7)
#define USART_CR3_DMAR  			(1 << 6)
#define USART_CR3_SCEN  			(1 << 5)
#define USART_CR3_NACK  			(1 << 4)
#define USART_CR3_HDSEL 			(1 << 3)
#define USART_CR3_IRLP  			(1 << 2)
#define USART_CR3_IREN  			(1 << 1)
#define USART_CR3_EIE 	 			(1 << 0)


/***** Variables and Function Prototypes *********************************/
#define BUF_SIZE ((uint8_t)255)

volatile uint8_t head;
volatile uint8_t tail;
volatile char inputData[BUF_SIZE];

void USART2_IRQHandler();
void USART2_init();

#endif