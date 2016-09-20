#include "gpio.h"
#include "rcc.h"


/*  gpio_init()
 *
 *	Arguments: None
 *
 *  Enables RCC clocks for respective GPIO banks and configures pins
 *
 *  Returns: Nothing  
 */

void gpio_init()
{

	RCC_AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  RCC_AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  // for PD12 (AF2 = PWM for TIM4)
  GPIO_MODER(GPIOD) |= GPIO_MODE(12, GPIO_MODE_AF);
  GPIO_AFRH(GPIOD) |= GPIO_AFR(4, GPIO_AF2);
  // LED for USB reset IRQ
  GPIO_MODER(GPIOD) |= GPIO_MODE(13, GPIO_MODE_OUTPUT);
  // LED for USB IRQ
  GPIO_MODER(GPIOD) |= GPIO_MODE(14, GPIO_MODE_OUTPUT);
}