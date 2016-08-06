#include "memorymap.h"
#include "rcc.h"
#include "gpio.h"
#include "usart.h"
#include "nvic.h"

#define APB1_CLK_PERIPH ((uint32_t)42000000)
#define BUF_SIZE ((uint8_t)255)

volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile char inputData[BUF_SIZE] = "";


void USART2_IRQHandler()            /* function name defined in startup file */
{
  while (USART_SR(USART2) & USART_SR_RXNE)
  {
    inputData[head] = USART_DR(USART2);

    if (inputData[head] == 0x3)             /* input is Ctrl + C */
    {
      head = 0;
      tail = 0;
      break;
    }

    head = (head + 1) % BUF_SIZE;

    // wrap around when head starts back at origin
    if (head < tail) {
      tail = (tail + 1) % BUF_SIZE;
      inputData[head] = inputData[BUF_SIZE - 1];
      head = (head + 1) % BUF_SIZE;
    }
  }
}


void usart2_init()
{
  uint32_t div_value = 0;
  uint32_t temp = 0;
  uint32_t frac_value = 0;
  
  uint32_t baud = 115200;
  
  RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC_APB1ENR |= RCC_APB1ENR_USART2EN; 

  /*  PA2 = USART2 TX
   *  PA3 = USART2 RX
   */
  GPIO_MODER(GPIOA) |= GPIO_MODE(2, GPIO_MODE_AF);               
  GPIO_MODER(GPIOA) |= GPIO_MODE(3, GPIO_MODE_AF);               
  GPIO_AFRL(GPIOA) |= GPIO_AFR(2, GPIO_AF7);              
  GPIO_AFRL(GPIOA) |= GPIO_AFR(3, GPIO_AF7);             

  USART_CR1(USART2) &= ~(USART_CR1_M);          
  USART_CR2(USART2) &= ~(USART_CR2_STOPBITS_0_5);       
 
  /*  Same as eq at 30.3.4 of ref manual but modified to prevent div_value
   *  from getting too large before dividing
   */
  div_value = ((APB1_CLK_PERIPH*25)/(4*baud));

  temp = (div_value/100);
  frac_value = div_value - (100*temp);
  div_value = (temp << 4);

  div_value |= (((frac_value * 16)/100) & 0x0F);

  USART_BRR(USART2) = div_value;

  USART_CR1(USART2) |= (USART_CR1_RE | USART_CR1_TE | USART_CR1_RXNEIE);

  NVIC_IPR((NVIC_USART2_IRQ / 4) + (NVIC_USART2_IRQ % 4)) = 0x20;
  NVIC_ISER(NVIC_USART2_IRQ / 32) = (1 << (NVIC_USART2_IRQ % 32));

  USART_CR1(USART2) |= USART_CR1_UE;    
}