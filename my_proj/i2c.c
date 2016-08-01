#include "util.h"
#include "nvic.h"
#include "gpio.h"
#include "i2c.h"
#include "rcc.h"

volatile int i2c_data = 0;

void I2C1_init()
{

	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;


	GPIO_MODER(GPIOB) |= GPIO_MODE(6, GPIO_MODE_AF);               /* PB6 used as alt func */
	GPIO_MODER(GPIOB) |= GPIO_MODE(9, GPIO_MODE_AF);               /* PB9 used as alt func */

	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(6, GPIO_TYPE_OD);
	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(9, GPIO_TYPE_OD);

	GPIO_AFRL(GPIOB) |= GPIO_AFR(6, GPIO_AF4);              /* PB6 as AF4 (I2C) function */
	GPIO_AFRH(GPIOB) |= GPIO_AFR(1, GPIO_AF4);               /* PB9 as AF4 (I2C) function */

	RCC_APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	RCC_APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST);

	I2C_CR2(I2C1) = I2C_CR2_FREQ_42MHZ;  												/* (for 42MHz APB1 clock freq) */
	I2C_CCR(I2C1) |= 210;												/* 100KHz clock speed */
	I2C_TRISE(I2C1) = (I2C_CR2_FREQ_42MHZ + 1); 											/* Max Trise time = APB1 freq + 1 */

	I2C_CR1(I2C1) |= I2C_CR1_PE; 								/* I2C enable */

	I2C_CR1(I2C1) |= I2C_CR1_ACK;

	// take DAC chip out of reset
	GPIO_MODER(GPIOD) |= GPIO_MODE(4, GPIO_MODE_OUTPUT);
	GPIO_ODR(GPIOD) |= (1 << 4);

}


void i2c_read(int addr, int reg, int numOfBytes) {
	/* 1. set start bit
	 * 2. INT: read SR1 for SB and write DR register with address
	 * 3. INT: read SR1 for ADDR  and read SR2 for TRA (1 for transmitted
	 *  	and 0 for receive?). If only reading one byte, disable ack as well
	 * 4. INT: RXNE is 1, read DR
	 * 5. INT: same as four, when done, set ACK=0 and stop
	 */ 

	I2C_CR1(I2C1) |= I2C_CR1_START;

	while(!(I2C_SR1(I2C1) & I2C_SR1_SB));

	I2C_DR(I2C1) = addr;

	while(!(I2C_SR1(I2C1) & I2C_SR1_ADDR));

	if(I2C_SR2(I2C1) & I2C_SR2_TRA)
		I2C_DR(I2C1) = reg;


	while(!(I2C_SR1(I2C1) & I2C_SR1_BTF));

	I2C_CR1(I2C1) |= I2C_CR1_STOP;

	while(I2C_SR2(I2C1) & I2C_SR2_BUSY);

	I2C_CR1(I2C1) |= I2C_CR1_START;

	while(!(I2C_SR1(I2C1) & I2C_SR1_SB));

	I2C_DR(I2C1) = (addr | 0x1);

	while(!(I2C_SR1(I2C1) & I2C_SR1_ADDR));

	// FOR ONLY READING ONE BYTE

	I2C_CR1(I2C1) &= ~(I2C_CR1_ACK);
	if(!(I2C_SR2(I2C1) & I2C_SR2_TRA)) {
		I2C_CR1(I2C1) |= I2C_CR1_STOP;
		while(!(I2C_SR1(I2C1) & I2C_SR1_RXNE));
		i2c_data = I2C_DR(I2C1);

		while(I2C_SR2(I2C1) & I2C_SR2_BUSY);

		I2C_CR1(I2C1) |= I2C_CR1_ACK;

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