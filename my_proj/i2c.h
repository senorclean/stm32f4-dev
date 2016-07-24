#ifndef I2C_H_
#define I2C_H_

volatile int i2c_data;
volatile int i2c_data_flag;

void I2C1_IRQHandler();
void I2C1_init();
void i2c_read();
void i2c_write();

#endif