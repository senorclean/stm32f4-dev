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


/*  DMA1_Stream5_IRQHandler()
 *
 *  Checks for transfer complete interrupt flag to load whatever has been
 *  loaded at the DMA input location into the input buffer. Circular buffer
 *  functionality has also been implemented.
 */

void DMA1_Stream5_IRQHandler() {
  if (DMA_HISR(DMA1) & DMA_HISR_TCIF5) {
    inputData[head] = dmaInput;
    // if input is Ctrl + C, to reset buffer
    if (inputData[head] == 0x3) {
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


/*  dma_init()
 *
 *  Arguments: None
 *
 *  Enables DMA1 Stream 5 (for USART2 RX) and DMA1 Stream 6 (for USART2 TX).
 *
 *  Returns: Nothing
 */

void dma_init() {

  // enable DMA1 clock
  RCC_AHB1ENR |= RCC_AHB1ENR_DMA1EN;

  // if DMA1 is currently enabled, disable it
  if (DMA_SCR(DMA1, 5) & DMA_SCR_EN)
    DMA_SCR(DMA1, 5) &= ~(DMA_SCR_EN);

  // set the peripheral address at USART2_DR register location
  DMA_SPAR(DMA1, 5) = (USART2 + 0x04);
  // set memory location at the address of the dmaInput variable
  DMA_SM0AR(DMA1, 5) = (int)&dmaInput;
  // only transfer one byte per transaction
  DMA_SNDTR(DMA1, 5) = 1;
  // select USART2 RX for DMA1 Stream 5
  DMA_SCR(DMA1, 5) |= DMA_SCR_CHSEL(4);

  // set interrupt priority as 2 (of 15 possible priorities)
  NVIC_IPR((NVIC_DMA1_STREAM5_IRQ / 4) + (NVIC_DMA1_STREAM5_IRQ % 4)) = 0x20;
  // enable DMA1 Stream 5 interrupt
  NVIC_ISER(NVIC_DMA1_STREAM5_IRQ / 32) = (1 << (NVIC_DMA1_STREAM5_IRQ % 32));
  
  // clear interrupt flag
  DMA_HIFCR(DMA1) |= DMA_HIFCR_CTCIF5;
  // enable transfer complete interrupt, high priority level, circular mode 
  DMA_SCR(DMA1, 5) |= (DMA_SCR_TCIE | DMA_SCR_PL_HIGH | DMA_SCR_CIRC);
  // enable DMA1 Stream 5 peripheral
  DMA_SCR(DMA1, 5) |= DMA_SCR_EN;

  // ENABLE DMA1 STREAM 6 IN THE SAME WAY AS ABOVE
}


/*  usart2_init()
 *
 *  Arguments: None
 *
 *  Enables the USART2 peripheral with PA2 as TX and PA3 as RX
 *
 *  Returns: Nothing
 */

void usart2_init()
{
  uint32_t div_value = 0;
  uint32_t temp = 0;
  uint32_t frac_value = 0;
  
  // selected baudrate
  uint32_t baud = 115200;
  
  // enable GPIOA and USART2 clocks
  RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC_APB1ENR |= RCC_APB1ENR_USART2EN; 

  // configure USART pins for alt functions
  GPIO_MODER(GPIOA) |= GPIO_MODE(2, GPIO_MODE_AF);               
  GPIO_MODER(GPIOA) |= GPIO_MODE(3, GPIO_MODE_AF);
  // configure USART pins for USART functionality               
  GPIO_AFRL(GPIOA) |= GPIO_AFR(2, GPIO_AF7);              
  GPIO_AFRL(GPIOA) |= GPIO_AFR(3, GPIO_AF7);             
  // length = one start bit, 8 data bits, and one stop bit
  USART_CR1(USART2) &= ~(USART_CR1_M);       
  // one stop bit   
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
  // enable RX and TX for USART2
  USART_CR1(USART2) |= (USART_CR1_RE | USART_CR1_TE); 

  // These are only necessary if using USART_IRQ directly not DMA
  // USART_CR1(USART2) |= USART_CR1_RXNEIE;
  // NVIC_IPR((NVIC_USART2_IRQ / 4) + (NVIC_USART2_IRQ % 4)) = 0x20;
  // NVIC_ISER(NVIC_USART2_IRQ / 32) = (1 << (NVIC_USART2_IRQ % 32));

  // enable DMA mode with USART2 RX
  USART_CR3(USART2) |= USART_CR3_DMAR;
  // enable USART2 peripheral
  USART_CR1(USART2) |= USART_CR1_UE;    
}