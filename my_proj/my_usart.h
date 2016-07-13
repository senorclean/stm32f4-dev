#include "stdint.h"

#ifndef USART_H_
#define USART_H_

#define BUF_SIZE ((uint8_t)255)

volatile uint8_t head;
volatile uint8_t tail;
volatile char inputData[BUF_SIZE];

void USART2_IRQHandler();
void USART2_init();

#endif