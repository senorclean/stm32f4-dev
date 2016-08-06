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

	// need to put all devices connected to I2C in reset

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

	// take I2C devices out of reset

	// take DAC chip out of reset
	GPIO_MODER(GPIOD) |= GPIO_MODE(4, GPIO_MODE_OUTPUT);
	GPIO_ODR(GPIOD) |= (1 << 4); 
}

void fix_i2c_timeout(uint32_t bus) {

	I2C_CR1(bus) |= I2C_CR1_STOP;

	// potentially add manually SCL toggling (x9) if SDA is still stuck
	
}


int check_sr1_timeout(uint32_t bus, uint16_t statusBit) {

	int initialTimerCount = 0;

	initialTimerCount = tim3Counter;
	while(!(I2C_SR1(bus) & statusBit)) {
		if(tim3Counter >= (initialTimerCount + I2C_ACK_TIMEOUT)) {
			return(FAILURE);
		}
	}

	return(SUCCESS);	
}


int check_sr2_timeout(uint32_t bus, uint16_t statusBit) {

	int initialTimerCount = 0;

	initialTimerCount = tim3Counter;
	while(!(I2C_SR2(bus) & statusBit)) {
		if(tim3Counter >= (initialTimerCount + I2C_ACK_TIMEOUT)) {
			return(FAILURE);
		}
	}

	return(SUCCESS);	
}


int i2c_send_start(uint32_t bus) {

	I2C_CR1(bus) |= I2C_CR1_START;

	if(!(check_sr1_timeout(bus, I2C_SR1_SB))) {
		print_string("\r\nError: I2C Timeout - Initial start bit not set");
		fix_i2c_timeout(bus);
		return(FAILURE);
	}

	return(SUCCESS);
}


void i2c_send_stop(uint32_t bus) {

	I2C_CR1(bus) |= I2C_CR1_STOP;

	while(I2C_SR2(bus) & I2C_SR2_BUSY);
}


int i2c_send_addr(uint32_t bus, int addr) {

	I2C_DR(bus) = addr;

	if(!(check_sr1_timeout(bus, I2C_SR1_ADDR))) {
		print_string("\r\nError: I2C Timeout - No ACK from write addr");
		fix_i2c_timeout(bus);
		return(FAILURE);
	}

	return(SUCCESS);
}


int i2c_send_reg(uint32_t bus, int addr, int reg) {

	if(I2C_SR2(bus) & I2C_SR2_TRA) {
		if ((addr == 0x94) && (bus == I2C1)) {
			// the cirrus logic chip requires bit 7 be set to 1 for auto-increment
			I2C_DR(bus) = (reg | 0x80);
		}
		else
			I2C_DR(bus) = reg;


		while(!(I2C_SR1(bus) & I2C_SR1_BTF));

		return(SUCCESS);
	}

	return(FAILURE);
}


void i2c_read_byte(uint32_t bus) {

	I2C_CR1(bus) &= ~(I2C_CR1_ACK);
	if(!(I2C_SR2(bus) & I2C_SR2_TRA)) {
		i2c_send_stop(bus);

		while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
		i2cBuff[0] = I2C_DR(bus);
		I2C_CR1(bus) |= I2C_CR1_ACK;
	}
}


void i2c_read_bytes(uint32_t bus, int numOfBytes) {

	int i = 0;

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


void i2c_write_bytes(uint32_t bus, int numOfBytes) {
	while (numOfBytes > 0) {
		while(!(I2C_SR1(bus) & I2C_SR1_TXE));
		I2C_DR(bus) = i2cBuff[numOfBytes - 1];
		numOfBytes--;
	}

	while(!(I2C_SR1(bus) & I2C_SR1_BTF));
}


int count_bytes(int data) {
	int numOfBytes = 0;

	if (data == 0) {
		i2cBuff[numOfBytes] = 0;
		numOfBytes++;
	}
	else {
		while (data > 0) {
			i2cBuff[numOfBytes] = (data % 256);
			data /= 256;
			numOfBytes++;
		}
	}

	return numOfBytes;
}


int i2c_read(uint32_t bus, int addr, int reg, int numOfBytes) {

	if (!(i2c_send_start(bus)))
		return(FAILURE);

	if (!(i2c_send_addr(bus, (addr & 0xFE))))
		return(FAILURE);

	if (!(i2c_send_reg(bus, (addr & 0xFE), reg)))
		return(FAILURE);

	i2c_send_stop(bus);

	if (!(i2c_send_start(bus)))
		return(FAILURE);

	if (!(i2c_send_addr(bus, (addr | 0x01))))
		return(FAILURE);	


	// FOR ONLY READING ONE BYTE
	if (numOfBytes == 1)
		i2c_read_byte(bus);
	else
		i2c_read_bytes(bus, numOfBytes);

	return(SUCCESS); 
}


int i2c_write(uint32_t bus, int addr, int reg, int data)
{

	int numOfBytes;

	numOfBytes = count_bytes(data);

	// i2c array is currently reversed
	
	if (!(i2c_send_start(bus)))
		return(FAILURE);

	if (!(i2c_send_addr(bus, (addr & 0xFE))))
		return(FAILURE);

	if (!(i2c_send_reg(bus, (addr & 0xFE), reg)))
		return(FAILURE);

	i2c_write_bytes(bus, numOfBytes);		

	i2c_send_stop(bus);

	return(SUCCESS);
}