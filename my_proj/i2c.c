#include "util.h"
#include "nvic.h"
#include "gpio.h"
#include "i2c.h"
#include "rcc.h"
#include "timer.h"

#define I2C_ACK_TIMEOUT ((uint8_t)20)
#define I2C_100KHZ_DELAY ((uint8_t)10)

volatile uint8_t i2cBuff[101] = {0};


void i2c1_init()
{

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
}

void clock_scl_timeout(uint32_t bus) {
	// don't do anything with bus parameter yet

	I2C_CR1(bus) |= I2C_CR1_STOP;

	/*GPIO_MODER(GPIOB) |= GPIO_MODE(6, GPIO_MODE_OUTPUT);
	GPIO_MODER(GPIOB) |= GPIO_MODE(9, GPIO_MODE_OUTPUT);
	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(6, GPIO_TYPE_PP);
	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(9, GPIO_TYPE_PP);
	GPIO_PUPDR(GPIOB) |= ~GPIO_PUPD(6, GPIO_PUPD_PU);
	GPIO_PUPDR(GPIOB) |= ~GPIO_PUPD(9, GPIO_PUPD_PU);

	GPIO_ODR(GPIOB) |= (1 << 6);
	GPIO_ODR(GPIOB) |= (1 << 9);


	for (i = 0; i < 9; i++) {
		GPIO_ODR(GPIOB) &= ~(1 << 6);
		initialTimerCount = tim5Counter;
		while(tim5Counter < (initialTimerCount + I2C_100KHZ_DELAY));
		GPIO_ODR(GPIOB) |= (1 << 6);
		initialTimerCount = tim5Counter;
		while(tim5Counter < (initialTimerCount + I2C_100KHZ_DELAY));
	}

	GPIO_MODER(GPIOB) |= GPIO_MODE(6, GPIO_MODE_AF); 
	GPIO_MODER(GPIOB) |= GPIO_MODE(9, GPIO_MODE_AF);
	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(6, GPIO_TYPE_OD);
	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(9, GPIO_TYPE_OD);
	GPIO_AFRL(GPIOB) |= GPIO_AFR(6, GPIO_AF4);              
	GPIO_AFRH(GPIOB) |= GPIO_AFR(1, GPIO_AF4);
	*/

/*	RCC_APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	RCC_APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST);*/	
}


void i2c_read(uint32_t bus, int addr, int reg, int numOfBytes) {
	
	int i = 0;
	int initialTimerCount = 0;
	
	I2C_CR1(bus) |= I2C_CR1_START;

	while(!(I2C_SR1(bus) & I2C_SR1_SB));

	I2C_DR(bus) = (addr & 0xFE);

	initialTimerCount = tim3Counter;
	while(!(I2C_SR1(bus) & I2C_SR1_ADDR)) {
		if(tim3Counter >= (initialTimerCount + I2C_ACK_TIMEOUT)) {
			print_string("\r\nError: I2C Timeout - No ACK received from slave");
			clock_scl_timeout(bus);
			i2cBuff[0] = 0;
			return;
		}
	}

	if(I2C_SR2(bus) & I2C_SR2_TRA) {
		if ((addr == 0x94) && (bus == I2C1)) {
			// the cirrus logic chip requires bit 7 be set to 1 for auto-increment
			I2C_DR(bus) = (reg | 0x80);
		}
		else
			I2C_DR(bus) = reg;
	}


	while(!(I2C_SR1(bus) & I2C_SR1_BTF));

	I2C_CR1(bus) |= I2C_CR1_STOP;

	while(I2C_SR2(bus) & I2C_SR2_BUSY);

	I2C_CR1(bus) |= I2C_CR1_START;

	while(!(I2C_SR1(bus) & I2C_SR1_SB));

	I2C_DR(bus) = (addr | 0x01);

	initialTimerCount = tim3Counter;
	while(!(I2C_SR1(bus) & I2C_SR1_ADDR)) {
		if(tim3Counter >= (initialTimerCount + I2C_ACK_TIMEOUT)) {
			print_string("\r\nError: I2C Timeout - No ACK received from slave");
			clock_scl_timeout(bus);
			i2cBuff[0] = 0;
			return;
		}
	}

	// FOR ONLY READING ONE BYTE
	if (numOfBytes == 1) {
		I2C_CR1(bus) &= ~(I2C_CR1_ACK);
		if(!(I2C_SR2(bus) & I2C_SR2_TRA)) {
			I2C_CR1(bus) |= I2C_CR1_STOP;
			while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
			i2cBuff[i] = I2C_DR(bus);

			while(I2C_SR2(bus) & I2C_SR2_BUSY);

			I2C_CR1(bus) |= I2C_CR1_ACK;
		}
	}
	else {
		if(!(I2C_SR2(bus) & I2C_SR2_TRA)) {
			while (numOfBytes > 2) {
				while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
				i2cBuff[i] = I2C_DR(bus);
				I2C_CR1(bus) |= I2C_CR1_ACK;
				i++;
				numOfBytes--;
			}

			while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
			i2cBuff[i] = I2C_DR(bus);
			I2C_CR1(bus) |= I2C_CR1_ACK;
			i++;

			I2C_CR1(bus) &= ~(I2C_CR1_ACK);
			I2C_CR1(bus) |= I2C_CR1_STOP;

			while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
			i2cBuff[i] = I2C_DR(bus);
			I2C_CR1(bus) |= I2C_CR1_ACK;
			i++;

			while(I2C_SR2(bus) & I2C_SR2_BUSY);

			I2C_CR1(bus) |= I2C_CR1_ACK;			
		}
	}


	 
}

void i2c_write(uint32_t bus, int addr, int reg, int data)
{

	int i = 0;
	int numOfBytes = 0;
	int initialTimerCount = 0;

	if (data == 0) {
		i2cBuff[i] = 0;
		i++;
	}
	else {
		while (data > 0) {
			i2cBuff[i] = (data % 256);
			data /= 256;
			i++;
		}
	}

	// array is currently reversed
	numOfBytes = i;
	
	I2C_CR1(bus) |= I2C_CR1_START;

	while(!(I2C_SR1(bus) & I2C_SR1_SB));

	I2C_DR(bus) = (addr & 0xFE);

	initialTimerCount = tim3Counter;
	while(!(I2C_SR1(bus) & I2C_SR1_ADDR)) {
		if(tim3Counter >= (initialTimerCount + I2C_ACK_TIMEOUT)) {
			print_string("\r\nError: I2C Timeout - No ACK received from slave");
			clock_scl_timeout(bus);
			i2cBuff[0] = 0;
			return;
		}
	}

	if(I2C_SR2(bus) & I2C_SR2_TRA) {
		if ((addr == 0x94) && (bus == I2C1)) {
			// the cirrus logic chip requires bit 7 be set to 1 for auto-increment
			while(!(I2C_SR1(bus) & I2C_SR1_TXE));
			I2C_DR(bus) = (reg | 0x80);
		}
		else {
			while(!(I2C_SR1(bus) & I2C_SR1_TXE));
			I2C_DR(bus) = reg;
		}
	}

	while (numOfBytes > 0) {
		while(!(I2C_SR1(bus) & I2C_SR1_TXE));
		I2C_DR(bus) = i2cBuff[numOfBytes - 1];

		numOfBytes--;
	}

	while(!(I2C_SR1(bus) & I2C_SR1_BTF));		

	I2C_CR1(bus) |= I2C_CR1_STOP;

	while(I2C_SR2(bus) & I2C_SR2_BUSY);

	print_string("\r\nWrite successful");

}