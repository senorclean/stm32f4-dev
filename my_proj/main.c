#include "stdint.h"
#include "usart.h"
#include "timer.h"
#include "gpio.h"
#include "sched.h"
#include "i2c.h"
#include "util.h"

/* TO DO: */
// get I2C working with audio chip on sch
	// test out commands (mostly done, need to try writes)
  // figure out how to change I2Cx based on bus number (done)
  // setup write / multiple bytes
	// setup read to do multiple bytes (done)
// setup board.h file with a board_init() for disc board specific stuff
// enable DMA for USART for cmd line
// get some error checking going on
// recomment my code
// put chip into low power modes and take it out
// configure USB1 for ACM capability (it can talk USART as well)
// get EEPROM/SPI-RJ45 for SPI functionality

/******************************** Definitions ********************************/
#define HSE_VALUE ((uint32_t)8000000)

/* Ghetto delay */
/*void ms_delay(int ms) {
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}*/


/********************************* Main Loop *********************************/
int main(void) {

  led_init();
  TIM3_init();
  TIM4_init();
  TIM5_init();
  USART2_init();
  I2C1_init();

  while (1) {
    scheduler();
  }
}



