#include "stm32f4xx.h"
#include "misc.h"

#define APB1_CLK_TIM3 ((uint32_t)84000000)
#define APB1_CLK_TIM4 ((uint32_t)84000000)


uint16_t counter = 0;

void TIM3_IRQHandler()
{
  if (TIM3->SR & TIM_SR_UIF)
    counter++;

  TIM3->CNT = 0;
  TIM3->SR &= ~(TIM_SR_UIF);
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
  NVIC_InitTypeDef NVIC_InitStruct; 

  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;       
  
  TIM3->CR1 &= ~(TIM_CR1_DIR);              /* upcounter */

  arr_for_ms_delay = APB1_CLK_TIM3/(1000*prescale_value);
  auto_reload_value = (arr_for_ms_delay*delay);

  TIM3->PSC = (prescale_value - 1);
  TIM3->ARR = (auto_reload_value - 1);
  TIM3->CNT = 0;                            /* initial counter value */
  TIM3->DIER |= TIM_DIER_UIE;               /* interrupt enable */

  NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStruct);

  TIM3->CR1 |= TIM_CR1_CEN;                 /* counter enable */
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

  RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;       
  
  TIM4->CR1 &= ~(TIM_CR1_DIR);              /* upcounter */

  /* setting PWM1 output mode and enabling preloading */
  TIM4->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1PE);
  TIM4->CCER |= TIM_CCER_CC1E;              /* enabling compare ch 1 */
  TIM4->CCR1 = 0;                           /* starting duty cycle at 0 */

  TIM4->PSC = (prescale_value - 1);
  TIM4->ARR = (auto_reload_value - 1);

  TIM4->CNT = 0;                            /* initial counter value */

  TIM4->CR1 |= TIM_CR1_CEN;                 /* counter enable */
}
