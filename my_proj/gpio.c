#include "gpio.h"
#include "rcc.h"


/*  led_init()
 *
 *  Enables the RCC block for the GPIOD block and sets a pin connected
 *  to one of the board's LEDs as an output
 *
 *  Returns: Nothing  
 */

void led_init()
{
  RCC_AHB1ENR |= RCC_AHB1ENR_GPIODEN;  
  //GPIOD->MODER |= (1 << 30);            /* PD15 for the LED as an output */

  // for PD12 (PWM for TIM4)
  GPIO_MODER(GPIOD) |= GPIO_MODE(12, GPIO_MODE_AF);
  GPIO_AFRH(GPIOD) |= GPIO_AFR(4, GPIO_AF2);


  GPIO_MODER(GPIOD) |= GPIO_MODE(13, GPIO_MODE_OUTPUT);
  GPIO_MODER(GPIOD) |= GPIO_MODE(14, GPIO_MODE_OUTPUT);
  GPIO_MODER(GPIOD) |= GPIO_MODE(6, GPIO_MODE_OUTPUT);
   
}