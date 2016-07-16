#include "stm32f4xx.h"
#include "misc.h"

void I2C1_IRQHandler()
{
	
}


void i2c_init()
{
	/* APB1 clock already enabled from USART2 init*/

	NVIC_InitTypeDef NVIC_InitStruct;

	I2C1->CR2 |= I2C_CR2_FREQ = 42;  				/* (for 42MHz APB1 clock freq) */
	I2C1->CCR = 210;												/* 100KHz clock speed */
	I2C1->TRISE = 43; 											/* Max Trise time = APB1 freq + 1 */



	NVIC_InitStruct.NVIC_IRQChannel = I2C1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStruct);

	I2C1->CR2 |= I2C_CR2_ITEVTEN;						/* Interrupt enable */
	I2C1->CR1 |= I2C_CR1_PE; 								/* I2C enable */
}


void i2c_read()
{
	/* 1. set start bit
	 * 2. INT: read SR1 for SB and write DR register with address
	 * 3. INT: read SR1 for ADDR  and read SR2 for TRA (1 for transmitted
	 *  	and 0 for receive?). If only reading one byte, disable ack as well
	 * 4. INT: RXNE is 1, read DR
	 * 5. INT: same as four, when done, set ACK=0 and stop
	 */

	// I2C_CR1_ACK
	// I2C_DR, (RXNE), not empty
	// I2C_SR1_SMBALERT need to write 0
	// I2C_SR1_BTF (1 transfer is finished)
	// I2C_SR1_SB (start bit)

	I2C1->CR1 |= 
}

void i2c_write()
{
	/* 1. set start bit
	 * 2. INT: read SR1 for SB and write DR register with address
	 * 3. INT: read SR1 for ADDR  and read SR2 for TRA (1 for transmitted
	 *  	and 0 for receive?)
	 * 4. INT: read TXE to make sure empty, write data into DR (register to write)
	 * 5. INT: same as four, write data to be written into register
	 * 6. INT: same as four, write data until done with reg
	 * 7. INT: BTF and TXE should be 1, set stop bit
	 */

	// I2C_CR1_STOP
	// I2C_CR1_START
	// I2C_DR, continuous stream can be started if another bit is put into
	// I2C_SR1_AF, acknowledge failure (reset by software)
	// I2C_SR1_TXE, tx empty
	// I2C_SR1_BTF (1 transfer is finished)
	// I2C_SR1_SB (start bit)
	// this register while TxE bit is set
}