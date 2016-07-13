#include "misc.h"
#include "stm32f4xx.h"

#define APB1_CLK ((uint32_t)42000000)
#define BUF_SIZE ((uint8_t)255)

volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile char inputData[BUF_SIZE] = "";

void USART2_IRQHandler()            /* function name defined in startup file */
{
  while (USART2->SR & (USART_SR_RXNE | USART_SR_ORE))
  {
    inputData[head] = USART2->DR;

    if (inputData[head] == 0x3)             /* input is Ctrl + C */
    {
      head = 0;
      tail = 0;
      break;
    }

    head = (head + 1) % BUF_SIZE;

    /* This doesn't work with how my loop is structured but there needs
     * to be some kind of handling in case the buffer gets full before it
     * can be read. I'll worry about it later.
     */
    /*if (head == tail)
      tail = (tail + 1) % BUF_SIZE;*/
  }
}


/*  USART2_init()
 *
 *  Holy mother of fuck was this frustrating.
 *
 *  1. Enable the peripheral clock for the GPIO bank you're using
 *    (USART2 uses GPIOA bank)
 *  2. Enable the USART clock based on the specific USART you're using
 *  3. Setup the GPIO modes so that they're "alternate function"
 *  4. Edit GPIO alternate function register to make them USART specifically
 *  5. Set word length to 8 bits
 *  6. Set stop bit to 1
 *  7. Set baud rate (side note: fuck the STM datasheet)
 *  8. Enable the TX and RX for USART2
 *  9. Optionally enable the TX/RX interrupts
 *  10. Enable USART2 itself
 *
 *
 *  Baud rate notes:
 *  - fclk is peripheral clk which is the system clock prescaled by 4 because
 *  we're not operating in low power mode. this is 42MHz
 *  - make sure that you have a clear #define 8MHz line for the HSE_CLK because
 *  the default discovery board files have it set as 25MHz because they're 
 *  morons
 *
 *  Returns: Nothing
 */

void USART2_init()
{
  uint32_t div_value = 0;
  uint32_t temp = 0;
  uint32_t frac_value = 0;
  NVIC_InitTypeDef NVIC_InitStruct;
  
  uint32_t baud = 115200;
  
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN; 

  /*  PA2 = USART2 TX
   *  PA3 = USART2 RX
   */
  GPIOA->MODER |= (1 << 5);               /* PA2 used as alt func */
  GPIOA->MODER |= (1 << 7);               /* PA3 used as alt func */
  GPIOA->AFR[0] |= (7 << 8);              /* PA2 as USART function */
  GPIOA->AFR[0] |= (7 << 12);             /* PA3 as USART function */

  USART2->CR1 &= ~(USART_CR1_M);          /* Word length = 8 bits */
  USART2->CR2 &= ~(USART_CR2_STOP);       /* 1 stop bit */
 
  /*  Same as eq at 30.3.4 of ref manual but modified to prevent div_value
   *  from getting too large before dividing
   */
  div_value = ((APB1_CLK*25)/(4*baud));

  temp = (div_value/100);
  frac_value = div_value - (100*temp);
  div_value = (temp << 4);

  div_value |= (((frac_value * 16)/100) & 0x0F);

  USART2->BRR = div_value;

  USART2->CR1 |= USART_CR1_RE;
  USART2->CR1 |= USART_CR1_TE;
  USART2->CR1 |= USART_CR1_RXNEIE;

  NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStruct);

  USART2->CR1 |= USART_CR1_UE;    
}