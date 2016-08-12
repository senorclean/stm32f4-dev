#include "memorymap.h"
#include "rcc.h"
#include "gpio.h"
#include "usart.h"
#include "nvic.h"
#include "dma.h"
#include "util.h"

#define APB1_CLK_PERIPH ((uint32_t)42000000)
#define BUF_SIZE ((uint8_t)255)

volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile char inputData[BUF_SIZE] = "";
static char dmaInput;

// load from the peripheral or memory location through DMA_SxPAR
// or DMA_SxM0AR
// storage in the destination through the same registers
// post-decrement of DMA_SxNDTR for transactions remaining

// Stream 5: USART2_RX, Stream 6: USART2_TX

// Read enable bit from DMA_SxCR reg to ensure it's disabled
// set the DMA_SxPAR register address
// set the DMA_SxM0AR register address
// configure DMA_SxNDTR for number of data items
// DMA_SxCR reg bits 29:27 are 0x4 for USART
// Configure stream priority in DMA_SxCR reg
// Configure the FIFO
// Configure for peripheral to mem direction, single trans,
// auto-increment, data widths, circ mode, interrupts
// enable the stream
// check for TCIF in interrupt
// setup ISR for the DMA

// enable DMAR in CR3

void DMA1_Stream5_IRQHandler()            /* function name defined in startup file */
{
  if (DMA_HISR(DMA1) & DMA_HISR_TCIF5)
  {
    inputData[head] = dmaInput;

    if (inputData[head] == 0x3)             /* input is Ctrl + C */
    {
      head = 0;
      tail = 0;
      return;
    }

    head = (head + 1) % BUF_SIZE;

    // wrap around when head starts back at origin
    if (head < tail) {
      tail = (tail + 1) % BUF_SIZE;
      inputData[head] = inputData[BUF_SIZE - 1];
      head = (head + 1) % BUF_SIZE;
    }


    DMA_HIFCR(DMA1) |= DMA_HIFCR_CTCIF5;
  }

}


void dma_init() {

  RCC_AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  if (DMA_SCR(DMA1, 5) & DMA_SCR_EN)
    DMA_SCR(DMA1, 5) &= ~(DMA_SCR_EN);

  DMA_SPAR(DMA1, 5) = (USART2 + 0x04);
  DMA_SM0AR(DMA1, 5) = (int)&dmaInput;
  DMA_SNDTR(DMA1, 5) = 1;

  DMA_SCR(DMA1, 5) |= DMA_SCR_CHSEL(4);

  NVIC_IPR((NVIC_DMA1_STREAM5_IRQ / 4) + (NVIC_DMA1_STREAM5_IRQ % 4)) = 0x20;
  NVIC_ISER(NVIC_DMA1_STREAM5_IRQ / 32) = (1 << (NVIC_DMA1_STREAM5_IRQ % 32));
  

  DMA_HIFCR(DMA1) |= DMA_HIFCR_CTCIF5;
  DMA_SCR(DMA1, 5) |= (DMA_SCR_TCIE | DMA_SCR_PL_HIGH | DMA_SCR_CIRC);
  DMA_SCR(DMA1, 5) |= DMA_SCR_EN;
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

  USART_CR1(USART2) |= (USART_CR1_RE | USART_CR1_TE); 

  USART_CR3(USART2) |= USART_CR3_DMAR;
  USART_CR1(USART2) |= USART_CR1_UE;    
}