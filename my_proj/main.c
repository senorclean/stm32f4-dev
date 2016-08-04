#include "stdint.h"
#include "usart.h"
#include "timer.h"
#include "gpio.h"
#include "sched.h"
#include "i2c.h"
#include "util.h"

/* TO DO: */
// enable DMA for USART for cmd line
// use EEPROM for SPI functionality
// setup enumerator in cmdline:
	// general failure codes:
		// 0 - failure
		// 1 - success  
// get some error checking going on
// recomment my code
// put chip into low power modes and take it out
// configure USB1 for ACM capability (it can talk USART as well)

/***** Definitions ***********************************************************/
#define HSE_VALUE ((uint32_t)8000000)


/***** Main Loop *************************************************************/
int main(void) {

  gpio_init();
  tim3_init();
  tim4_init();
  tim5_init();
  usart2_init();
  i2c1_init();

  while (1) {
    scheduler();
  }
}



