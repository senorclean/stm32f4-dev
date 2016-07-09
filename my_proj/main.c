
/* Uncomment this STDPERIPH line in stm32f4xx.h otherwise linking issues */
#define USE_STDPERIPH_DRIVER
#include "stm32f4xx.h"
#include "misc.h"

/* REMEMBER THE RCC FOR INTERRUPTS/GPIOS/ANYTHING!! */

/* TO DO: */
// get a heartbeat going for the LED
  // setting up PWM for LED pin

/******************************** Definitions ********************************/
#define HSE_VALUE ((uint32_t)8000000)
#define APB1_CLK ((uint32_t)42000000)
#define APB1_CLK_TIM3 ((uint32_t)84000000)
#define APB1_CLK_TIM4 ((uint32_t)84000000)
#define BUF_SIZE ((uint32_t)255)
#define NUM_BUF_SIZE ((uint8_t)11)     /* max 32-bit number is 10 digits */

/****************************** Global Variables *****************************/
volatile uint8_t head = 0;
volatile uint8_t tail = 0;
volatile char inputData[BUF_SIZE] = "";
NVIC_InitTypeDef NVIC_InitStruct;


/******************************** Global Flags *******************************/
int backFlag = 0;
volatile int ledFlag = 0;
volatile int counter = 0;


/******************************** IRQ Handlers *******************************/
void USART2_IRQHandler()            /* function name defined in startup file */
{
  while (USART2->SR & (USART_SR_RXNE | USART_SR_ORE))
  {
    inputData[head] = USART2->DR;

    if (inputData[head] == 0x3)             /* input is Ctrl + C */
    {
      head = 1;
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

void TIM3_IRQHandler()
{
  if (TIM3->SR & TIM_SR_UIF)
  {
    if (ledFlag)
    {
      if (TIM4->CCR1 == 0)
        ledFlag = 0;
      else
        TIM4->CCR1 -= 50;
    }
    else
    {
      if (TIM4->CCR1 == 1000)
      {
        counter++;
        if (counter == 4)
        {
          ledFlag = 1;
          counter = 0;
        }
      }
      else
        TIM4->CCR1 += 50;
    }
  }

  TIM3->SR &= ~(TIM_SR_UIF);
}


/*************************** Function Prototypes *****************************/
void USART2_init(uint32_t baud);
void led_init();
void TIM3_delay_ms_init(uint16_t delay);
void TIM4_init();
void number_to_ascii(uint32_t value, char *numArray);
void reverse_array(char *str);
void print_string(char *data);
void print_char(char data);
void clear_string(char *cmdStringPtr, uint8_t *cmdStringEndPos);
int process_input(char *cmdStringPtr, uint8_t *cmdStringPos, 
  uint8_t *cmdStringEndPos);
void display_cmd_string(char *cmdStringPtr, uint8_t *cmdStringPos,
  uint8_t *cmdStringEndPos);


/* Ghetto delay */
void ms_delay(int ms)
{
   while (ms-- > 0) {
      volatile int x=5971;
      while (x-- > 0)
         __asm("nop");
   }
}


/********************************* Main Loop *********************************/
int main(void)
{
  uint8_t cmdStringEndPos = 0;
  uint8_t i = 0;
  char cmdString[BUF_SIZE] = "";
  char *cmdStringPtr;
  uint8_t cmdStringPos = 0;
  cmdStringPtr = cmdString;
  char numArray[NUM_BUF_SIZE] = "";

  GPIOA->MODER &= ~(3 << 0);            /* set the push button to an input */

  led_init();
  TIM3_delay_ms_init(45);
  TIM4_init();
  USART2_init(115200);

  while (1)
  {
    while (tail < head)
    {
/*      number_to_ascii(head, numArray);
        print_string("\r\nHead:");
        print_string(numArray);
        number_to_ascii(tail, numArray);
        print_string("\r\nTail:");
        print_string(numArray);
        print_string("\r\n");*/
      
      if (process_input(cmdStringPtr, &cmdStringPos, &cmdStringEndPos))
        display_cmd_string(cmdStringPtr, &cmdStringPos, &cmdStringEndPos);
    }
  }
}



/***************************** Function Definitions **************************/

/*  display_cmd_string()
 *
 *  Takes string as input with two values: the cursor's current position
 *  and the string's current end position. Each time this function is entered
 *  the command string is reprinted on the same line. If the cursor is 
 *  somewhere in the middle of the string and a backspace is entered, the 
 *  string's size and its element order are adjusted accordingly when 
 *  displayed. To keep track of the cursor position after a new line is
 *  printed, the cursor is shifted left based on the value of the current
 *  cursor position and the end of the string.
 *
 *  Returns: Nothing  
 */

void display_cmd_string(char *cmdStringPtr, uint8_t *cmdStringPos,
 uint8_t *cmdStringEndPos)
{
  uint8_t i;

  if (!backFlag)
  { 
    /*  the key was not backspace while the cursor was in the middle of
     *  the string
     */
    if (*cmdStringPos < *cmdStringEndPos)
    {
      /* the cursor is not at the end of the string */

      /* shift every element forward one to accommodate new entry */
      i = *cmdStringEndPos;
      while (i >= *cmdStringPos)
      {
        cmdStringPtr[i + 1] = cmdStringPtr[i];
        i--;
      }

      /* load the latest element into the cmd string to be displayed */
      cmdStringPtr[*cmdStringPos] = inputData[tail];
    }

    else
    {
      /* the cursor is at the end of the string so new value is loaded */
      cmdStringPtr[*cmdStringPos] = inputData[tail];
    }
  }


  tail = (tail + 1) % BUF_SIZE;
  print_string("\r");
  print_string(cmdStringPtr);

  /* move cursor backwards to reflect where the string's current position is */
  i = *cmdStringPos;
  while (*cmdStringEndPos > i)
  {
    print_string("\e[1D");
    i++;
  }

  /*  if there's a backspace in the middle of the string, the string isn't
   *  adding a character and therefore doesn't need to be incremented
   */
  if (!backFlag)
  {
    (*cmdStringPos)++;
    (*cmdStringEndPos)++;
  }


  backFlag = 0;
}


/*  process_input()
 *
 *  Takes string as input with two values: the cursor's current position
 *  and the string's current end position. The value from the input buffer
 *  is analyzed for whether it's a special input that needs to be dealt
 *  with separately or if it should just be printed as entered. In the event
 *  of a backspace, there are two different outcomes based on whether the
 *  cursor is at the end of the string or in the middle of it.
 *
 *  Returns: 0 or 1 depending on whether the displayCmdPrompt function
 *  needs to be called or whether the displaying is handled here.  
 */

int process_input(char *cmdStringPtr, uint8_t *cmdStringPos, 
  uint8_t *cmdStringEndPos)
{
  int i = 0;

  switch(inputData[tail])
  {
    case 0x0D:                                    /* enter key */
      print_string("\r\n");
      clear_string(cmdStringPtr, cmdStringEndPos);
      *cmdStringPos = 0;
      *cmdStringEndPos = 0;

      tail = (tail + 1) % BUF_SIZE;
      return 0;

    case 0x1B:                                    /* escape seq */
      tail = (tail + 2) % BUF_SIZE;
   
     /*  This line was necessary because the USART interrupt wasn't able
      *  to receive all of the characters in an escape sequence quick
      *  enough to have the correct character for the switch below. Also,
      *  the tail would be greater than the head and this loop's criteria
      *  (tail < head) would never be entered again. Just another sweet 
      *  feature of this program.
      */
      while(tail >= head);

      switch(inputData[tail])
      {
        case 'A':                                 /* up arrow */
          break;

        case 'B':                                 /* down arrow */
          break;

        case 'C':                                 /* right arrow */
          if (*cmdStringPos < *cmdStringEndPos)
          {
            print_string("\e[1C");
            (*cmdStringPos)++;
          }
          break;

        case 'D':                                 /* left arrow */
          if (*cmdStringPos != 0)
          {
            print_string("\e[1D");
            (*cmdStringPos)--;
          }
          break;
      }


      tail = (tail + 1) % BUF_SIZE;
      return 0;

    case 0x7F:                                    /* backspace */

      i = *cmdStringPos;


      if (*cmdStringPos != *cmdStringEndPos)      
      {

       /*  backspace is entered but the cursor is something in the middle
        *  of the string
        */

        /* shifting everything in front of the cursor back one */
        while (cmdStringPtr[i] != '\0')
        {
          cmdStringPtr[i - 1] = cmdStringPtr[i];
          i++;
        }

        cmdStringPtr[i - 1] = '\0';

        /* making sure deletion stops at origin of string */
        if ((*cmdStringPos != 0) && (*cmdStringEndPos != 0))
        {
          (*cmdStringPos)--;
          (*cmdStringEndPos)--;
        }

        /* clearing the stale characters to the right of the cursor */
        for (i = *cmdStringPos; i < *cmdStringEndPos; i++)
          print_char(' ');

        backFlag = 1;
        return 1; 
      }

      else
      {
        /*  backspace is entered and cursor is at end of string */

        /* making sure deletoin stops at origin of string */
        cmdStringPtr[i] = cmdStringPtr[i + 1];
        if ((*cmdStringPos != 0) && (*cmdStringEndPos != 0))
        {
          (*cmdStringPos)--;
          (*cmdStringEndPos)--;
        }

        /* clearing stale character in console line */
        print_string("\e[1D \e[1D");

        tail = (tail + 1) % BUF_SIZE;
        return 0;
      }


    default:
      return 1;
  }
}


/*  clear_string()
 *
 *  Takes string as input along with the value of its end position.
 *  Wipes out the inputted string with null characters
 *
 *  Returns: Nothing  
 */

void clear_string(char *cmdStringPtr, uint8_t *cmdStringEndPos)
{
  int i = 0;
  while (cmdStringPtr[i] != '\0')
  {
    cmdStringPtr[i] = '\0';
    i++;
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

void USART2_init(uint32_t baud)
{
  uint32_t div_value = 0;
  uint32_t temp = 0;
  uint32_t frac_value = 0;
  
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


/*  led_init()
 *
 *  Enables the RCC block for the GPIOD block and sets a pin connected
 *  to one of the board's LEDs as an output
 *
 *  Returns: Nothing  
 */

void led_init()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  
  //GPIOD->MODER |= (1 << 30);            /* PD15 for the LED as an output */

  // for PD12
  GPIOD->MODER |= (1 << 25);
  GPIOD->AFR[1] |= (1 << 17);
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

void TIM3_delay_ms_init(uint16_t delay)
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


/*  print_string()
 *
 *  Takes string as input and sends each character out in a loop
 *
 *  Returns: Nothing  
 */

void print_string(char *data)
{
  int i = 0;

  while (data[i] != '\0')
  {
    while(!(USART2->SR & USART_SR_TXE));
    USART2->DR = data[i];
    i++;   
  }  
}


/*  displayCmdString()
 *
 *  Takes a single character as input and prints it out. Will likely be
 *  deprecated as print_string() can also perform this task
 *
 *  Returns: Nothing  
 */

void print_char(char data)
{
  while(!(USART2->SR & USART_SR_TXE));
  USART2->DR = data;
}


/*  number_to_ascii()
 *
 *  Takes a 32-bit integer to be converted into ASCII along with a char
 *  array to store the converted value. The array is passed in instead of
 *  being declared as static or dynamically allocated. Each digit of the
 *  value is converted at one time and as a result ends up backwards in the
 *  array. Reverse_array() is called in the event there is more than one digit
 *  and resolves this issue.
 *
 *  Returns: Nothing  
 */

void number_to_ascii(uint32_t value, char *numArray)
{
  int i;
  char x = '\0';

  /* seems arbitrary but a 32-bit value can't be longer than 10 digits */
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

  if (i > 1)
    reverse_array(numArray);

  if (numArray[i] != '\0')
    numArray[i] = '\0';
}


/*  reverse_array()
 *
 *  Takes string as an input and reverses it. Named this one pretty well.
 *
 *  Returns: Nothing  
 */

void reverse_array(char *str)
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
