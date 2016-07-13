#include "stm32f4xx.h"


/*  led_init()
 *
 *  Enables the RCC block for the GPIOD block and sets a pin connected
 *  to one of the board's LEDs as an output
 *
 *  Returns: Nothing  
 */

void led_init()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  
  //GPIOD->MODER |= (1 << 30);            /* PD15 for the LED as an output */

  // for PD12
  GPIOD->MODER |= (1 << 25);
  GPIOD->AFR[1] |= (1 << 17);
}