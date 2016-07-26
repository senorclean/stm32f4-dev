#include "stm32f4xx.h"
#include "misc.h"
#include "util.h"

volatile int i2c_data = 0;

void I2C1_IRQHandler()
{

}


void I2C1_init()
{
	NVIC_InitTypeDef NVIC_InitStruct;



	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;


	GPIOB->MODER |= (1 << 13);               /* PB6 used as alt func */
	GPIOB->MODER |= (1 << 19);               /* PB9 used as alt func */

	GPIOB->OTYPER |= (1 << 6);
	GPIOB->OTYPER |= (1 << 9);

	/*GPIOB->PUPDR |= (1 << 12);
	GPIOB->PUPDR |= (1 << 18);*/

	/*GPIOB->OSPEEDR |= (3 << 12);
	GPIOB->OSPEEDR |= (3 << 18);*/

	GPIOB->AFR[0] |= (4 << 24);              /* PB6 as AF4 (I2C) function */
	GPIOB->AFR[1] |= (4 << 4);               /* PB9 as AF4 (I2C) function */

	// addr: 0x94

	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST);

	I2C1->CR2 = 42;  												/* (for 42MHz APB1 clock freq) */
	I2C1->CCR |= 210;												/* 100KHz clock speed */
	I2C1->TRISE = 43; 											/* Max Trise time = APB1 freq + 1 */



	NVIC_InitStruct.NVIC_IRQChannel = I2C1_EV_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

	//NVIC_Init(&NVIC_InitStruct);

	//I2C1->CR2 |= I2C_CR2_ITEVTEN;						/* Interrupt enable */

	I2C1->CR1 |= I2C_CR1_PE; 								/* I2C enable */

	I2C1->CR1 |= I2C_CR1_ACK;

	// take DAC chip out of reset
	GPIOD->MODER |= (1 << 8);
	GPIOD->ODR |= (1 << 4);

}


void i2c_read(int addr, int reg, int numOfBytes) {
	/* 1. set start bit
	 * 2. INT: read SR1 for SB and write DR register with address
	 * 3. INT: read SR1 for ADDR  and read SR2 for TRA (1 for transmitted
	 *  	and 0 for receive?). If only reading one byte, disable ack as well
	 * 4. INT: RXNE is 1, read DR
	 * 5. INT: same as four, when done, set ACK=0 and stop
	 */ 


	I2C1->CR1 |= I2C_CR1_START;

	while(!(I2C1->SR1 & I2C_SR1_SB));

	I2C1->DR = addr;

	while(!(I2C1->SR1 & I2C_SR1_ADDR));

	if(I2C1->SR2 & I2C_SR2_TRA)
		I2C1->DR = reg;


	while(!(I2C1->SR1 & I2C_SR1_BTF));

	I2C1->CR1 |= I2C_CR1_STOP;

	while(I2C1->SR2 & I2C_SR2_BUSY);


	I2C1->CR1 |= I2C_CR1_START;

	while(!(I2C1->SR1 & I2C_SR1_SB));

	I2C1->DR = (addr | 0x1);

	while(!(I2C1->SR1 & I2C_SR1_ADDR));

	// FOR ONLY READING ONE BYTE

	I2C1->CR1 &= ~(I2C_CR1_ACK);
	if(!(I2C1->SR2 & I2C_SR2_TRA)) {
		I2C1->CR1 |= I2C_CR1_STOP;
		while(!(I2C1->SR1 & I2C_SR1_RXNE));
		i2c_data = I2C1->DR;

		while(I2C1->SR2 & I2C_SR2_BUSY);

		I2C1->CR1 |= I2C_CR1_ACK;

	}


	// I2C_CR1_ACK
	// I2C_DR, (RXNE), not empty
	// I2C_SR1_SMBALERT need to write 0
	// I2C_SR1_BTF (1 transfer is finished)
	// I2C_SR1_SB (start bit)

	 
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