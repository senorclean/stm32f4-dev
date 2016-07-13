#include "stdint.h"

#ifndef TIMER_H_
#define TIMER_H_

extern uint16_t counter;

void TIM3_delay_ms_init();
void TIM4_init();
void TIM3_IRQHandler();

#endif