/* Uncomment this STDPERIPH line in stm32f4xx.h otherwise linking issues */
#define USE_STDPERIPH_DRIVER

#include "stdint.h"
#include "stm32f4xx.h"
#include "my_usart.h"
#include "timer.h"
#include "my_gpio.h"
#include "sched.h"

/* REMEMBER THE RCC FOR INTERRUPTS/GPIOS/ANYTHING!! */

/* TO DO: */
// get I2C working with audio chip on sch
  // setup read, write and ISR
// enable DMA for DAC for audio chip
// get some error checking going on
// figure out how to enable interrupts without NVIC
// put chip into low power modes and take it out
// get EEPROM/SPI-RJ45 for SPI functionality

/******************************** Definitions ********************************/
#define HSE_VALUE ((uint32_t)8000000)

/* Ghetto delay */
/*void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}*/


/********************************* Main Loop *********************************/
int main(void)
{

  led_init();
  TIM3_delay_ms_init();
  TIM4_init();
  USART2_init();

  while (1)
  {
    scheduler();
  }
}



