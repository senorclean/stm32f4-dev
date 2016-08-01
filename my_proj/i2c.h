#ifndef I2C_H_
#define I2C_H_

#include "memorymap.h"

/***** I2C Base Regs ****************************************************/
#define I2C1 						I2C1_BASE
#define I2C2 						I2C2_BASE
#define I2C3 						I2C3_BASE

/***** I2C Regs *********************************************************/
#define I2C_CR1(i2c_base) 			MMIO16(i2c_base + 0x00)
// #define I2C1_CR1 					I2C_CR1(I2C1_BASE)
// #define I2C2_CR1 					I2C_CR1(I2C2_BASE)			
#define I2C_CR2(i2c_base) 			MMIO16(i2c_base + 0x04)
#define I2C_OAR1(i2c_base)			MMIO16(i2c_base + 0x08)
#define I2C_OAR2(i2c_base) 			MMIO16(i2c_base + 0x0C)
#define I2C_DR(i2c_base) 				MMIO16(i2c_base + 0x10)
#define I2C_SR1(i2c_base) 			MMIO16(i2c_base + 0x14)
#define I2C_SR2(i2c_base) 			MMIO16(i2c_base + 0x18)
#define I2C_CCR(i2c_base) 			MMIO16(i2c_base + 0x1C)
#define I2C_TRISE(i2c_base) 		MMIO16(i2c_base + 0x20)
#define I2C_FLTR(i2c_base) 			MMIO16(i2c_base + 0x24)

/***** I2C CR1 Regs *****************************************************/
#define I2C_CR1_SWRST 				(1 << 15)
#define I2C_CR1_ALERT 				(1 << 13)
#define I2C_CR1_PEC 				(1 << 12)
#define I2C_CR1_POS 				(1 << 11)
#define I2C_CR1_ACK 				(1 << 10)
#define I2C_CR1_STOP 				(1 << 9)
#define I2C_CR1_START 				(1 << 8)
#define I2C_CR1_NOSTRETCH 			(1 << 7)
#define I2C_CR1_ENGC 				(1 << 6)
#define I2C_CR1_ENPEC 				(1 << 5)
#define I2C_CR1_ENARP 				(1 << 4)
#define I2C_CR1_SMBTYPE 			(1 << 3)
#define I2C_CR1_SMBUS 				(1 << 1)
#define I2C_CR1_PE 					(1 << 0)

/***** I2C CR2 Regs ******************************************************/
#define I2C_CR2_LAST 				(1 << 12)
#define I2C_CR2_DMAEN 				(1 << 11)
#define I2C_CR2_ITBUFEN				(1 << 10)
#define I2C_CR2_ITEVTEN				(1 << 9)
#define I2C_CR2_ITERREN				(1 << 8)
// This what I'm currently using but can define more based on needs
#define I2C_CR2_FREQ_42MHZ			42

/***** I2C OAR1 Regs *****************************************************/
#define I2C_OAR1_ADDMODE 			(1 << 15)

/***** I2C OAR2 Regs *****************************************************/
#define I2C_OAR2_ENDUAL 			(1 << 0)

/***** I2C SR1 Regs ******************************************************/
#define I2C_SR1_SMBALERT 			(1 << 15)
#define I2C_SR1_TIMEOUT 			(1 << 14)
#define I2C_SR1_PECERR	 			(1 << 12)
#define I2C_SR1_OVR 	 			(1 << 11)
#define I2C_SR1_AF 		 			(1 << 10)
#define I2C_SR1_ARLO 	 			(1 << 9)
#define I2C_SR1_BERR 	 			(1 << 8)
#define I2C_SR1_TXE 	 			(1 << 7)
#define I2C_SR1_RXNE 	 			(1 << 6)
#define I2C_SR1_STOPF 	 			(1 << 4)
#define I2C_SR1_ADD10 	 			(1 << 3)
#define I2C_SR1_BTF 	 			(1 << 2)
#define I2C_SR1_ADDR 	 			(1 << 1)
#define I2C_SR1_SB 		 			(1 << 0)

/***** I2C SR2 Regs ******************************************************/
#define I2C_SR2_DUALF 				(1 << 7)
#define I2C_SR2_SMBHOST				(1 << 6)
#define I2C_SR2_SMBDEFAULT 			(1 << 5)
#define I2C_SR2_GENCALL 			(1 << 4)
#define I2C_SR2_TRA 				(1 << 2)
#define I2C_SR2_BUSY 				(1 << 1)
#define I2C_SR2_MSL 				(1 << 0)

/***** I2C CCR Regs ******************************************************/
#define I2C_CCR_FS 					(1 << 15)
#define I2C_CCR_DUTY 				(1 << 14)



/***** Variables and Function Prototypes *********************************/
volatile int i2c_data;

void I2C1_init();
void i2c_read();
void i2c_write();

#endif