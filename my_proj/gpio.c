#include "gpio.h"
#include "rcc.h"


/*  led_init()
 *
 *  Enables the RCC block for the GPIOD block and sets a pin connected
 *  to one of the board's LEDs as an output
 *
 *  Returns: Nothing  
 */

void gpio_init()
{

	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  RCC_AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  // for PD12 (PWM for TIM4)
  GPIO_MODER(GPIOD) |= GPIO_MODE(12, GPIO_MODE_AF);
  GPIO_AFRH(GPIOD) |= GPIO_AFR(4, GPIO_AF2);

	// take DAC chip out of reset
	GPIO_MODER(GPIOD) |= GPIO_MODE(4, GPIO_MODE_OUTPUT);
	GPIO_ODR(GPIOD) |= (1 << 4);   
}