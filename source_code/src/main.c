#include "stdint.h"
#include "usart.h"
#include "timer.h"
#include "gpio.h"
#include "sched.h"
#include "i2c.h"

/* TO DO: */ 
// configure USB1 for ACM capability (it can talk USART as well)
// use Wifi chip for SPI functionality
// put chip into low power modes and take it out

/***** Definitions ***********************************************************/
#define HSE_VALUE ((uint32_t)8000000)



/***** Main Loop *************************************************************/
int main(void) {
  gpio_init();
  tim3_init();
  tim4_init();
  tim5_init();
  usart2_init();
  dma_init();
  i2c1_init();

  while (1) {
    scheduler();
  }
}



