#include "rcc.h"
#include "timer.h"
#include "nvic.h"
#include "timer.h"

#define APB1_CLK_TIM3 ((uint32_t)84000000)
#define APB1_CLK_TIM4 ((uint32_t)84000000)


volatile uint16_t counter = 0;

void TIM3_IRQHandler()
{
  if (TIM_SR(TIM3) & TIM_SR_UIF)
    counter++;

  TIM_CNT(TIM3) = 0;
  TIM_SR(TIM3) &= ~(TIM_SR_UIF);
}



/*  TIM3_delay_ms_init()
 *
 *  Takes the desired delay value in milliseconds as input. Based on the
 *  current prescale value (8400), the delay will only work up to about
 *  six seconds otherwise the prescale value needs to be adjusted accorindgly.
 *  This is all done with Timer 3 as it's a 16-bit timer and I didn't need
 *  anything more granular for a simple delay.  
 *
 *  Returns: Nothing  
 */

void TIM3_delay_ms_init()
{
  uint16_t prescale_value =  8400;   /* for even division of 84MHz clock */
  uint16_t auto_reload_value = 0;
  uint16_t arr_for_ms_delay = 0;
  uint16_t delay = 1;                /* 1 ms delay */
  //NVIC_InitTypeDef NVIC_InitStruct; 

  RCC_APB1ENR |= RCC_APB1ENR_TIM3EN;       
  
  TIM_CR1(TIM3) &= ~(TIM_CR1_DIR);              /* upcounter */

  arr_for_ms_delay = APB1_CLK_TIM3/(1000*prescale_value);
  auto_reload_value = (arr_for_ms_delay*delay);

  TIM_PSC(TIM3) = (prescale_value - 1);
  TIM_ARR(TIM3) = (auto_reload_value - 1);
  TIM_CNT(TIM3) = 0;                            /* initial counter value */
  TIM_DIER(TIM3) |= TIM_DIER_UIE;               /* interrupt enable */

  //IPR0-20 with 32 bits in each and 8-bits priority for each user interrupt
  NVIC_IPR((NVIC_TIM3_IRQ / 4) + (NVIC_TIM3_IRQ % 4)) = 0xF0;
  //ISER0-2 with 32-bits in each for the 80 user interrupts
  NVIC_ISER(NVIC_TIM3_IRQ / 32) = (1 << (NVIC_TIM3_IRQ % 32));

  TIM_CR1(TIM3) |= TIM_CR1_CEN;                 /* counter enable */
}


/*  TIM4init()
 *
 *  Currently configured to output a PWM signal on Channel 1 of TIM4. This is
 *  connected to PD12 on the board (Green LED) and used concurrently with TIM3
 *  to create a "heartbeat" for the LED.
 *
 *  Returns: Nothing  
 */

void TIM4_init()
{
  uint16_t prescale_value = 840;
  uint16_t auto_reload_value = 1000;

  RCC_APB1ENR |= RCC_APB1ENR_TIM4EN;       
  
  TIM_CR1(TIM4) &= ~(TIM_CR1_DIR);              /* upcounter */

  /* setting PWM1 output mode and enabling preloading */
  TIM_CCMR1(TIM4) |= (TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE);
  TIM_CCER(TIM4) |= TIM_CCER_CC1E;              /* enabling compare ch 1 */
  TIM_CCR1(TIM4) = 0;                           /* starting duty cycle at 0 */

  TIM_PSC(TIM4) = (prescale_value - 1);
  TIM_ARR(TIM4) = (auto_reload_value - 1);

  TIM_CNT(TIM4) = 0;                            /* initial counter value */

  TIM_CR1(TIM4) |= TIM_CR1_CEN;                 /* counter enable */
}
