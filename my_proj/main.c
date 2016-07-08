
/* Uncomment this STDPERIPH line in stm32f4xx.h otherwise linking is a hassle */
#define USE_STDPERIPH_DRIVER
#include "stm32f4xx.h"
#include "misc.h"

/* REMEMBER THE RCC FOR INTERRUPTS/GPIOS/ANYTHING!! */

/* TO DO: */
// streamline the display section


#define HSE_VALUE ((uint32_t)8000000)
#define APB1_CLK ((uint32_t)42000000)
#define APB1_CLK_TIM3 ((uint32_t)84000000)
#define BUF_SIZE ((uint32_t)255)

volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile char inputData[BUF_SIZE] = "";
//volatile char* volatile inputDataPtr;
//inputDataPtr = inputData;

int backFlag = 0;


void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}

/* this function name is defined in startup file */
void USART2_IRQHandler()
{
  while (USART2->SR & (USART_SR_RXNE | USART_SR_ORE))
  {
    inputData[head] = USART2->DR;

    if (inputData[head] == 0x3)
    {
      head = 1;
      tail = 0;
      break;
    }

    head = (head + 1) % BUF_SIZE;

    /*
     * This doesn't work with how my loop is structured but there needs
     * to be some kind of handling in case the buffer gets full before it
     * can be read
     */
/*    if (head == tail)
      tail = (tail + 1) % BUF_SIZE;*/
  }
}

void TIM3_IRQHandler()
{
  if (TIM3->SR & TIM_SR_UIF)
    GPIOD->ODR ^= (1 << 15);

  TIM3->SR &= ~(TIM_SR_UIF);
}

void USART2_Init(uint32_t baud);
void LED_Init();
void delayInit(uint16_t delay);
void numberToASCII(uint32_t value, char *numArray);
void reverseArray(char *str);
void printString(char *data);
void printChar(char data);
void clearString(char *cmdStringPtr, uint8_t *cmdStringEndPos);
int processInput(char *cmdStringPtr, uint8_t *cmdStringPos, 
  uint8_t *cmdStringEndPos);

NVIC_InitTypeDef NVIC_InitStruct;

int main(void)
{
  uint8_t cmdStringEndPos = 0;
  uint8_t i = 0;
  int processValue = 0;
  char cmdString[BUF_SIZE] = "";
  char *cmdStringPtr;
  uint8_t cmdStringPos = 0;
  cmdStringPtr = cmdString;
  char numArray[10] = "";
  int x;

  GPIOA->MODER &= ~(3 << 0);            /* set the push button to an input */

  // GPIOD->ODR ^= (1 << 15);           /* toggle LED */

  LED_Init();

 /*
  *  delay in ms (goes up to about 6s)
  *  otherwise I need to adjust PSC value
  */
  delayInit(1000);
  USART2_Init(115200);
  
  // numberToASCII(3750, numArray);
  // reverseArray(numArray);

  while (1)
  {
    while (tail < head)
    {
/*        numberToASCII(head, numArray);
        printString("\r\nHead:");
        printString(numArray);
        numberToASCII(tail, numArray);
        printString("\r\nTail:");
        printString(numArray);
        printString("\r\n");*/
      
      if (processInput(cmdStringPtr, &cmdStringPos, &cmdStringEndPos))
      {
        if (!backFlag)
        {
          if (cmdStringPos < cmdStringEndPos)
          {
            x = cmdStringEndPos;
            while (x >= cmdStringPos)
            {
              cmdString[x + 1] = cmdString[x];
              x--;
            }

            cmdString[cmdStringPos] = inputData[tail];
          }
          else
            cmdString[cmdStringPos] = inputData[tail];
        }

        tail = (tail + 1) % BUF_SIZE;
        printString("\r");
        printString(cmdString);
        i = cmdStringPos;
        while (cmdStringEndPos > i)
        {
          printString("\e[1D");
          i++;
        }


        if (!backFlag)
        {
          cmdStringPos++;
          cmdStringEndPos++;
        }


        backFlag = 0;
      }
    }
  }
}



int processInput(char *cmdStringPtr, uint8_t *cmdStringPos, 
  uint8_t *cmdStringEndPos)
{
  int i = 0;

  switch(inputData[tail])
  {
    case 0x0D:
      printString("\r\n");
      clearString(cmdStringPtr, cmdStringEndPos);
      *cmdStringPos = 0;
      *cmdStringEndPos = 0;

      tail = (tail + 1) % BUF_SIZE;
      return 0;

    case 0x1B:
      tail = (tail + 2) % BUF_SIZE;

      /*
      * This line was necessary because the USART interrupt wasn't able
      * to receive all of the characters in an escape sequence quick
      * enough to have the correct character for the switch below. Also,
      * the tail would be greater than the head and the whole loop would
      * end up crashing. Just another sweet feature of this program.
      */
      while(tail >= head);

      switch(inputData[tail])
      {
        case 'A':
          break;

        case 'B':
          break;

        case 'C':
          if (*cmdStringPos < *cmdStringEndPos)
          {
            printString("\e[1C");
            (*cmdStringPos)++;
          }
          break;

        case 'D':
          printString("\e[1D");
          if (*cmdStringPos != 0)
            (*cmdStringPos)--;
          break;
      }


      tail = (tail + 1) % BUF_SIZE;
      return 0;

    case 0x7F:

      i = *cmdStringPos;

      if (*cmdStringPos != *cmdStringEndPos)
      {
        while (cmdStringPtr[i] != '\0')
        {
          cmdStringPtr[i - 1] = cmdStringPtr[i];
          i++;
        }

        cmdStringPtr[i-1] = '\0';

        if ((*cmdStringPos != 0) && (*cmdStringEndPos != 0))
        {
          (*cmdStringPos)--;
          (*cmdStringEndPos)--;
        }

        for (i = *cmdStringPos; i < *cmdStringEndPos; i++)
          printChar(' ');

        backFlag = 1;
        return 1; 
      }
      else
      {
        cmdStringPtr[i] = cmdStringPtr[i + 1];
        if ((*cmdStringPos != 0) && (*cmdStringEndPos != 0))
        {
          (*cmdStringPos)--;
          (*cmdStringEndPos)--;
        }

        printString("\e[1D \e[1D");
        tail = (tail + 1) % BUF_SIZE;
        return 0;
      }


    default:
      return 1;
  }
}

void clearString(char *cmdStringPtr, uint8_t *cmdStringEndPos)
{
  int i = 0;
  while (cmdStringPtr[i] != '\0')
  {
    cmdStringPtr[i] = '\0';
    i++;
  }

}


/*
 *  Setting up USART2:
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
 */

void USART2_Init(uint32_t baud)
{
  uint32_t div_value = 0;
  uint32_t temp = 0;
  uint32_t frac_value = 0;
  
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN; 

  /*
   *  PA2 = USART2 TX
   *  PA3 = USART2 RX
   */
  GPIOA->MODER |= (1 << 5);
  GPIOA->MODER |= (1 << 7);
  GPIOA->AFR[0] |= (7 << 8);
  GPIOA->AFR[0] |= (7 << 12);

  USART2->CR1 &= ~(USART_CR1_M);          /* Word length = 8 bits */
  USART2->CR2 &= ~(USART_CR2_STOP);       /* 1 stop bit */

  /* 
   *  Same as eq at 30.3.4 of ref man but modified to prevent div_value
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

void LED_Init()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  
  GPIOD->MODER |= (1 << 30);            /* PD15 for the LED as an output */
}

void delayInit(uint16_t delay)
{
  uint16_t prescale_value =  8400;   /* for even division of 84MHz clock */
  uint16_t auto_reload_value = 0;
  uint16_t arr_for_ms_delay = 0; 

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

void printString(char *data)
{
  int i = 0;

  while (data[i] != '\0')
  {
    while(!(USART2->SR & USART_SR_TXE));
    USART2->DR = data[i];
    i++;   
  }  
}

void printChar(char data)
{
  while(!(USART2->SR & USART_SR_TXE));
  USART2->DR = data;
}

/*  
 *  You can't return an array in C so you can either dynamically allocate
 *  memory for an array inside the function and return it, make the array
 *  a static variable, or pass array into the function as pointer
 */
void numberToASCII(uint32_t value, char *numArray)
{
  int i;
  char x = '\0';
  uint32_t tempValue = value;

  for (i = 0; i < 10; i++)
  {
    if ((value == 0) && (i == 0))
    {
      numArray[i] = '0' + x;
      i++;
      break;
    }
    else
      if (value == 0)
        break;

    x = (char)(value % 10);
    value = (value / 10);
    numArray[i] = '0' + x;
  }

  if (tempValue > 9)
    reverseArray(numArray);

  if (numArray[i] != '\0')
    numArray[i] = '\0';

}

void reverseArray(char *str)
{
  int i = 0;
  int j = 0;
  char tempStr[10] = "\0";

  while (str[i] != '\0')
    i++;

  i--;
  while (i >= 0)
  {
    tempStr[j] = str[i];
    i--;
    j++;
  }

  tempStr[j] = '\0';  
  while (j >= 0)
  {
    str[i] = tempStr[i];
    i++;
    j--;  
  }

}
