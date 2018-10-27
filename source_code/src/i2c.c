#include "util.h"
#include "nvic.h"
#include "gpio.h"
#include "i2c.h"
#include "rcc.h"
#include "timer.h"

#define I2C_ACK_TIMEOUT ((uint8_t)20)
#define I2C_100KHZ_DELAY ((uint8_t)10)

volatile uint8_t i2cBuff[101] = {0};


/*  i2c1_init()
 *
 *  Arguments: None
 *
 *  Initializes I2C1 to be used on pins PB6 (SCL) and PB9 (SDA). Clock speed
 *	is 100kHz
 *
 *  Returns: Nothing  
 */

void i2c1_init()
{

	// need to put all devices connected to this I2C bus in reset if not already

	// enable I2C1 clock
	RCC_APB1ENR |= RCC_APB1ENR_I2C1EN;

	// switch I2C pins to alt functions
	GPIO_MODER(GPIOB) |= GPIO_MODE(6, GPIO_MODE_AF);               
	GPIO_MODER(GPIOB) |= GPIO_MODE(9, GPIO_MODE_AF);
	// configure I2C pins as open drain
	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(6, GPIO_TYPE_OD);
	GPIO_OTYPER(GPIOB) |= GPIO_OTYPE(9, GPIO_TYPE_OD);
	// configure alt functions for I2C
	GPIO_AFRL(GPIOB) |= GPIO_AFR(6, GPIO_AF4);
	GPIO_AFRH(GPIOB) |= GPIO_AFR(1, GPIO_AF4);

	// to unstick the I2C bus, reset the I2C peripheral (documented STM bug)
	RCC_APB1RSTR |= RCC_APB1RSTR_I2C1RST;
	RCC_APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST);

	// configure to use 42MHz clock peripheral
	I2C_CR2(I2C1) = I2C_CR2_FREQ_42MHZ;
	// I2C clock speed is 100kHz
	I2C_CCR(I2C1) |= 210;
	// max Trise time = APB1 freq + 1 from datasheet
	I2C_TRISE(I2C1) = (I2C_CR2_FREQ_42MHZ + 1);
	// enable I2C peripheral
	I2C_CR1(I2C1) |= I2C_CR1_PE;

	// take devices out of reset connected to this I2C bus

	// take DAC chip out of reset
	GPIO_MODER(GPIOD) |= GPIO_MODE(4, GPIO_MODE_OUTPUT);
	GPIO_ODR(GPIOD) |= (1 << 4); 
}


/*  fix_i2c_timeout()
 *
 *  Arguments: I2C bus to unstick
 *
 *  Sends a stop condition so slave releases I2C lines
 *
 *  Returns: Nothing  
 */

void fix_i2c_timeout(uint32_t bus) {

	I2C_CR1(bus) |= I2C_CR1_STOP;

	// potentially add manually SCL toggling (x9) if SDA is still stuck
	
}


/*  check_sr1_timeout()
 *
 *  Arguments: selected I2C bus, selected bit to check from SR1 reg
 *
 *  Checks to make sure that there isn't an I2C timeout if the passed bit
 *	is never set by the I2C peripheral and the bus is stuck
 *
 *  Returns: Success or failure code
 */

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


/*  check_sr2_timeout()
 *
 *  Arguments: selected I2C bus, selected bit to check from SR2 reg
 *
 *  Checks to make sure that there isn't an I2C timeout if the passed bit
 *	is never set by the I2C peripheral and the bus is stuck
 *
 *  Returns: Success or failure code
 */

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


/*  i2c_send_start()
 *
 *  Arguments: selected I2C bus
 *
 *  Sends a start condition on selected I2C bus. Checks for an I2C timeout
 *	and unsticks the bus if necessary.
 *
 *  Returns: Success or failure code
 */

int i2c_send_start(uint32_t bus) {

	I2C_CR1(bus) |= I2C_CR1_START;

	if(!(check_sr1_timeout(bus, I2C_SR1_SB))) {
		print_string("\r\nError: I2C Timeout - Initial start bit not set");
		fix_i2c_timeout(bus);
		return(FAILURE);
	}

	return(SUCCESS);
}


/*  i2c_send_stop()
 *
 *  Arguments: selected I2C bus
 *
 *  Sends a stop condition on selected I2C bus and waits for bus to become
 * 	idle before returning.
 *
 *  Returns: Nothing
 */

void i2c_send_stop(uint32_t bus) {

	I2C_CR1(bus) |= I2C_CR1_STOP;

	while(I2C_SR2(bus) & I2C_SR2_BUSY);
}


/*  i2c_send_addr()
 *
 *  Arguments: selected I2C bus, I2C device's address
 *
 *  Loads the address of the I2C device to be communicated with and checks
 * 	that an acknowledgment is received
 *
 *  Returns: Success or failure code
 */

int i2c_send_addr(uint32_t bus, int addr) {

	I2C_DR(bus) = addr;

	if(!(check_sr1_timeout(bus, I2C_SR1_ADDR))) {
		print_string("\r\nError: I2C Timeout - No ACK from write addr");
		fix_i2c_timeout(bus);
		return(FAILURE);
	}

	return(SUCCESS);
}


/*  i2c_send_reg()
 *
 *  Arguments: selected I2C bus, I2C device's address, I2C device's register
 *
 *  Loads the I2C register to be accessed and ensures the byte is transferred
 * 	completely
 *
 *  Returns: Success or failure code
 */

int i2c_send_reg(uint32_t bus, int addr, int reg) {

	if(I2C_SR2(bus) & I2C_SR2_TRA) {
		if ((addr == 0x94) && (bus == I2C1)) {
			// the cirrus logic chip requires bit 7 be set to 1 for auto-increment
			I2C_DR(bus) = (reg | 0x80);
		}
		else
			I2C_DR(bus) = reg;

		// wait for transfer to be complete
		while(!(I2C_SR1(bus) & I2C_SR1_BTF));

		return(SUCCESS);
	}

	return(FAILURE);
}


/*  i2c_read_byte()
 *
 *  Arguments: selected I2C bus
 *
 *  Reads a single byte from the I2C slave. Single byte reading requires that
 * 	the acknowledge bit is cleared before reading the SR2 reg and a stop bit
 *	is sent before reading the I2C data register per STM datasheet.
 *
 *  Returns: Nothing
 */

void i2c_read_byte(uint32_t bus) {

	// clear acknowledge bit to slave can send data
	I2C_CR1(bus) &= ~(I2C_CR1_ACK);
	if(!(I2C_SR2(bus) & I2C_SR2_TRA)) {
		i2c_send_stop(bus);

		while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
		i2cBuff[0] = I2C_DR(bus);
		I2C_CR1(bus) |= I2C_CR1_ACK;
	}
}


/*  i2c_read_bytes()
 *
 *  Arguments: selected I2C bus, number of bytes to be read
 *
 *  Reads bytes from I2C data register until there is one byte left. Then
 *	the acknowledge bit is cleared and stop bit is set before receiving the
 *	last bit. 
 *
 *  Returns: Nothing
 */

void i2c_read_bytes(uint32_t bus, int numOfBytes) {

	int i = 0;

	if(!(I2C_SR2(bus) & I2C_SR2_TRA)) {
		while (numOfBytes > 1) {
			while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
			i2cBuff[i] = I2C_DR(bus);
			I2C_CR1(bus) |= I2C_CR1_ACK;
			i++;
			numOfBytes--;
		}

		I2C_CR1(bus) &= ~(I2C_CR1_ACK);
		I2C_CR1(bus) |= I2C_CR1_STOP;

		while(!(I2C_SR1(bus) & I2C_SR1_RXNE));
		i2cBuff[i] = I2C_DR(bus);
		I2C_CR1(bus) |= I2C_CR1_ACK;

		while(I2C_SR2(bus) & I2C_SR2_BUSY);
		I2C_CR1(bus) |= I2C_CR1_ACK;			
	}
}


/*  i2c_write_bytes()
 *
 *  Arguments: selected I2C bus, number of bytes to write
 *
 *  Writes data that is currently in I2C buffer to slave device. Starts from
 * 	the end of the I2C buffer because the input should be formatted such that
 * 	the MSB is at position 0 
 *
 *  Returns: Nothing
 */

void i2c_write_bytes(uint32_t bus, int numOfBytes) {
	while (numOfBytes > 0) {
		while(!(I2C_SR1(bus) & I2C_SR1_TXE));
		I2C_DR(bus) = i2cBuff[numOfBytes - 1];
		numOfBytes--;
	}

	while(!(I2C_SR1(bus) & I2C_SR1_BTF));
}


/*  count_bytes()
 *
 *  Arguments: data to be counted
 *
 *  Stores the right-most byte of the data and then right shifts the data over
 *	a byte for the next loop until all bytes have been stored
 *
 *  Returns: Number of bytes in passed data variable
 */

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


/*  i2c_read()
 *
 *  Arguments: selected I2C bus, I2C device's address, I2C device's reg,
 * 						 number of bytes to be read
 *
 *  Performs a read on the selected I2C device for the specified number
 *	of bytes. 
 *
 *  Returns: Success or failure code
 */

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

	if (numOfBytes == 1)
		i2c_read_byte(bus);
	else
		i2c_read_bytes(bus, numOfBytes);

	return(SUCCESS); 
}


/*  i2c_write()
 *
 *  Arguments: selected I2C bus, I2C device's address, I2C device's reg,
 * 						 data to be written
 *
 *  Performs a write on the selected I2C device for the data provided
 *
 *  Returns: Success or failure code
 */

int i2c_write(uint32_t bus, int addr, int reg, int data) {

	int numOfBytes;

	numOfBytes = count_bytes(data);
	
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