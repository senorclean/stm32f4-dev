#include "rcc.h"
#include "timer.h"
#include "nvic.h"
#include "timer.h"
#include "gpio.h"
#include "util.h"

#define APB1_CLK_TIMER       ((uint32_t)84000000)
#define MILLISECONDS_DELAY   ((uint16_t)1000)
#define MICROSECONDS_DELAY   ((uint32_t)1000000)  


volatile uint16_t tim3Counter = 0;
volatile uint32_t tim5Counter = 0;

/*  TIM3_IRQHandler()
 *
 *  Checks that a timer overflow occurred (~1ms), resets the count, and
 *  clears the interrupt flag
 */

void TIM3_IRQHandler() {
  if (TIM_SR(TIM3) & TIM_SR_UIF)
    tim3Counter++;

  TIM_CNT(TIM3) = 0;
  TIM_SR(TIM3) &= ~(TIM_SR_UIF);
}


/*  TIM5_IRQHandler()
 *
 *  Checks that a timer overflow occurred (~1us), resets the count, and
 *  clears the interrupt flag
 */

void TIM5_IRQHandler() {
  if (TIM_SR(TIM5) & TIM_SR_UIF)
    tim5Counter++;

  TIM_CNT(TIM5) = 0;
  TIM_SR(TIM5) &= ~(TIM_SR_UIF);
}


/*  tim3_init()
 *
 *  Arguments: None
 *
 *  Configures TIM3 to have a 1ms delay between counter overflows. Configured
 *  as a standard upcounter.  
 *
 *  Returns: Nothing  
 */

void tim3_init() {
  // for 10kHz scaled clock
  uint16_t prescale_value =  8400;
  uint16_t auto_reload_value = 0;
  uint16_t arr_for_ms_delay = 0;
  // desired delay in ms
  uint16_t delay = 1;

  // enable TIM3 clock
  RCC_APB1ENR |= RCC_APB1ENR_TIM3EN;
  // configure as upcounter
  TIM_CR1(TIM3) &= ~(TIM_CR1_DIR);

  arr_for_ms_delay = APB1_CLK_TIMER/(MILLISECONDS_DELAY * prescale_value);
  auto_reload_value = (arr_for_ms_delay * delay);

  TIM_PSC(TIM3) = (prescale_value - 1);
  TIM_ARR(TIM3) = (auto_reload_value - 1);
  // initial counter value
  TIM_CNT(TIM3) = 0;
  // enable counter interrupt
  TIM_DIER(TIM3) |= TIM_DIER_UIE;

  // IPR0-20 with 32 bits in each and 8-bits priority for each user interrupt
  // NVIC_IPR((NVIC_TIM3_IRQ / 4) + (NVIC_TIM3_IRQ % 4)) = 0x00;

  // ISER0-2 with 32-bits in each for the 80 user interrupts
  NVIC_ISER(NVIC_TIM3_IRQ / 32) = (1 << (NVIC_TIM3_IRQ % 32));
  // enable the counter
  TIM_CR1(TIM3) |= TIM_CR1_CEN;
}


/*  tim4_init()
 *
 *  Arguments: None
 *
 *  Configures PWM signal output on Channel 1 of TIM4. This is connected
 *  to PD12 on the board (Green LED) and used concurrently with TIM3
 *  to create a "heartbeat" for the LED.
 *
 *  Returns: Nothing  
 */

void tim4_init() {
  uint16_t prescale_value = 840;
  uint16_t auto_reload_value = 1000;

  // enable TIM4 clock
  RCC_APB1ENR |= RCC_APB1ENR_TIM4EN;       
  // configure clock as upcounter
  TIM_CR1(TIM4) &= ~(TIM_CR1_DIR);

  // set PWM1 output mode and enabling preloading
  TIM_CCMR1(TIM4) |= (TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE);
  // enable compare ch 1
  TIM_CCER(TIM4) |= TIM_CCER_CC1E;
  // start duty cycle at 0 (off)
  TIM_CCR1(TIM4) = 0;

  TIM_PSC(TIM4) = (prescale_value - 1);
  TIM_ARR(TIM4) = (auto_reload_value - 1);

  // set initial count as 0
  TIM_CNT(TIM4) = 0;
  // enable counter
  TIM_CR1(TIM4) |= TIM_CR1_CEN;
}


/*  tim5_init()
 *
 *  Arguments: None
 *
 *  Configures TIM5 to have a 1us delay between counter overflows. Configured
 *  as a standard upcounter.  
 *
 *  Returns: Nothing  
 */

void tim5_init() {
  uint16_t prescale_value =  1;
  uint32_t auto_reload_value = 0;
  uint32_t arr_for_us_delay = 0;
  // delay in us
  uint16_t delay = 1;

  // enable TIM5 clock
  RCC_APB1ENR |= RCC_APB1ENR_TIM5EN;       
  // configure as upcounter
  TIM_CR1(TIM5) &= ~(TIM_CR1_DIR);

  arr_for_us_delay = APB1_CLK_TIMER/(MICROSECONDS_DELAY * prescale_value);
  auto_reload_value = (arr_for_us_delay * delay);

  TIM_PSC(TIM5) = (prescale_value - 1);
  TIM_ARR(TIM5) = (auto_reload_value - 1);
  // set initial counter as 0
  TIM_CNT(TIM5) = 0;
  // enable interrupt
  TIM_DIER(TIM5) |= TIM_DIER_UIE;

  // IPR0-20 with 32 bits in each and 8-bits priority for each user interrupt
  // NVIC_IPR((NVIC_TIM5_IRQ / 4) + (NVIC_TIM5_IRQ % 4)) = 0x10;

  // ISER0-2 with 32-bits in each for the 80 user interrupts
  NVIC_ISER(NVIC_TIM5_IRQ / 32) = (1 << (NVIC_TIM5_IRQ % 32));
  // enable counter
  TIM_CR1(TIM5) |= TIM_CR1_CEN;
}
