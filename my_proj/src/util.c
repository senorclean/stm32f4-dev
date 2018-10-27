#include "usart.h"
#include "dma.h"
#include "nvic.h"
#include <stdarg.h>


enum returnCodes {  
                    FAILURE, 
                    SUCCESS
                 };

/*  clear_string()
 *
 *  Arguments: input string, # of chars to be cleared,
 *             position within string to start erasing
 *
 *  Overwrites elements of string with null characters based on arguments
 *
 *  Returns: Nothing  
 */

void clear_string(char *cmdStringPtr, int iterations, int start) {
  int i;
  for(i = start; i < iterations; i++)
    cmdStringPtr[i] = '\0';
}


/*  print_char()
 *
 *  Arguments: char to be printed
 *
 *  Prints a single character. Will likely be deprecated as print_string() 
 *  can also perform this task and more
 *
 *  Returns: Nothing  
 */

void print_char(char data) {
  while(!(USART_SR(USART2) & USART_SR_TXE));
  USART_DR(USART2) = data;
}


/*  reverse_array()
 *
 *  Arguments: array to be reversed
 *
 *  Takes string as an input and reverses it. Named this one pretty well
 *
 *  Returns: Nothing  
 */

void reverse_array(char *str) {
  int i = 0;
  int j = 0;
  char tempStr[11] = "";
  int stopPoint = 0;


  /* if it's a negative number, make sure negative sign is in first position
   * and then reverse everything after it
   */
  if (str[0] == '-') {
    tempStr[0] = '-';
    j = 1;
    i = 1;
    stopPoint = 1;
  }

  // count number of elements in string
  while (str[i] != '\0')
    i++;

  // start at end of string and reverse string into temp array
  i--;
  while (i >= stopPoint) {
    tempStr[j] = str[i];
    i--;
    j++;
  }
  
  // store reversed string back into original string array
  i = 0;
  while (tempStr[i] != '\0') {
    str[i] = tempStr[i];
    i++; 
  }
}


/*  decimal_to_ascii()
 *
 *  Arguments: value to be converted, array to store converted value
 *
 *  Takes up to a 32-bit decimal value to be converted into ASCII. Each digit 
 *  of the value is converted at one time and as a result ends up backwards in
 *  the array. Reverse_array() is called in the event there is more than one 
 *  digit and resolves this issue.
 *
 *  Returns: Nothing  
 */

void decimal_to_ascii(int value, char *tempArray) {
  int i;
  char x;

  // 32-bit value can't be bigger than 10 digits
  for (i = 0; i < 10; i++) {
    /*  if value is negative, make first element the negative sign and get 
     *  2's complement of the value so it's now positive 
     */
    if (value < 0) {
      tempArray[i] = '-';
      value = ~value + 1;
      continue;
    }

    // if the initial value passed to the function was 0, break from loop
    if ((value == 0) && (i == 0)) {
      tempArray[i] = '0';
      i++;
      break;
    }
    // if the value is now zero after division, loop is exited
    else
      if (value == 0)
        break;

    // store the rightmost value in the number and convert to ASCII equiv.
    x = (value % 10);
    value /= 10;
    tempArray[i] = '0' + x;
  }

  // reverse array if there is more an than one digit in number
  if (i > 1)
    reverse_array(tempArray);

  // seal the end of the string
  if (tempArray[i] != '\0')
    tempArray[i] = '\0';
}


/*  hex_to_ascii()
 *
 *  Arguments: value to be converted, array to store converted value
 *
 *  Takes up to a 32-bit hex value to be converted into ASCII. Each digit 
 *  of the value is converted at one time and as a result ends up backwards in
 *  the array. Reverse_array() is called in the event there is more than one 
 *  digit and resolves this issue.
 *
 *  Returns: Nothing  
 */

void hex_to_ascii(int value, char *tempArray) {
  int i;
  char x;

  for (i = 0; i < 10; i++) {
    // if the initial value passed in was 0, break from loop
    if ((value == 0) && (i == 0)) {
      tempArray[i] = '0';
      i++;
      break;
    }
    // if value is now zero from division, break from loop
    else
      if (value == 0)
        break;

    // divide off the right-most digit of number
    x = (value % 16);
    value /= 16;
    if (x > 9) {
      switch (x) {
        case 10:
          tempArray[i] = 'A';
          break;

        case 11:
          tempArray[i] = 'B';
          break;

        case 12:
          tempArray[i] = 'C';
          break;

        case 13:
          tempArray[i] = 'D';
          break;

        case 14:
          tempArray[i] = 'E';
          break;

        case 15:
          tempArray[i] = 'F';
          break;
      }
    }
    else
      tempArray[i] = '0' + x;
  }

  // reverse array if there is more than one value
  if (i > 1)
    reverse_array(tempArray);
  else {
    // to keep single numbered values to two spaces
    tempArray[1] = tempArray[0];
    tempArray[0] = '0';
    i++;
  }
  // seal end of string
  if (tempArray[i] != '\0')
    tempArray[i] = '\0';
}



/*  exponent()
 *
 *  Arguments: base value, exponent
 *
 *  Pretty self-explanatory
 *
 *  Returns: value^exp  
 */

int exponent(int number, int exp) {
  int newNumber = 1;
  int i;

  for (i = 0; i < exp; i++) {
    newNumber *= number;
  }

  return newNumber; 
}


/*  string_to_number()
 *
 *  Arguments: string to be converted, base of converted number (hex, dec)
 *
 *  Takes a string and converts it to a number with a base determined by the
 *  "base" argument
 *
 *  Returns: newly converted number  
 */

uint32_t string_to_number(char *str, int base) {
  uint32_t tempInt = 0;
  int i = 0;

  while (str[i] != '\0') {
    /*  for every value above position 0, multiply by the base to shift it left
     *  "one position"
     */
    if (i > 0) {
      tempInt *= base; 
    }

    // if the value is less than 10 then just a simple subtraction to the num
    if (str[i] < 58) {
      tempInt += str[i] - 48;
    }
    // if it's not numerical, then add respective value
    else {
      switch (str[i]) {
        case 'a':

        case 'A':
          tempInt += 10;
          break;

        case 'b':

        case 'B':
          tempInt += 11;
          break;

        case 'c':

        case 'C':
          tempInt += 12;
          break;

        case 'd':

        case 'D':
          tempInt += 13;
          break;

        case 'e':

        case 'E':
          tempInt += 14;
          break;

        case 'f':

        case 'F':
          tempInt += 15;
          break;
      }
    }

    i++; 
  }

  return tempInt;
}


/*  print_string()
 *
 *  Arguments: input string, additional arguments depending on what is
 *             used in the input string
 *
 *  Takes string as input and prints the string to the console. Can currently
 *  take %s, %x, and %d arguments in string and display them to console
 *
 *  Returns: Nothing  
 */

void print_string(char *data, ...) {
  int i = 0;
  int j = 0;
  int k = 0;
  char *tempStr;
  int tempInt = 0;
  char tempArray[11] = "";
  va_list args;

  // starts using any arguments input after "data"
  va_start(args, data);

  // wait for the last USART transaction to be completed
  while(!(USART_SR(USART2) & USART_SR_TC));

  // if DMA1 Stream 6 is currently enabled, disable it
  if (DMA_SCR(DMA1, 6) & DMA_SCR_EN)
    DMA_SCR(DMA1, 6) &= ~(DMA_SCR_EN);

  while (data[i] != '\0') {
    // if there is a special argument used
    if (data[i] == '%') {
      i++;
      switch(data[i]) {
        // decimal to be printed
        case 'd':
          // load decimal integer from argument list and convert to ASCII
          tempInt = va_arg(args, int);
          decimal_to_ascii(tempInt, &tempArray[0]);

          j = 0;
          while(tempArray[j] != '\0') {
            dmaOutputBuff[k] = tempArray[j];
            j++;
            k++; 
          }

          break;

        // string to be printed
        case 's':
          // load string from argument list
          tempStr = va_arg(args, char*);

          j = 0;
          while (*(tempStr + j) != '\0') {
            dmaOutputBuff[k] = *(tempStr + j);
            j++;
            k++; 
          }
          break;

        case 'x':
          // fall through

        case 'X':
          // load hex integer from arg list and convert to ASCII
          tempInt = va_arg(args, int);
          hex_to_ascii(tempInt, &tempArray[0]);

          j = 0;
          while(tempArray[j] != '\0') {
            dmaOutputBuff[k] = tempArray[j];
            j++;
            k++; 
          }

          break;

        default:
          break;
      }

      i++;
    }
    // if it's not a special argument, just print the value
    else {
      dmaOutputBuff[k] = data[i];
      i++;
      k++; 
    } 
  }

  // DMA in normal mode needs to be reconfigured every time it's used

  // memory to peripheral direction
  DMA_SCR(DMA1, 6) |= DMA_SCR_DIR_MEM_TO_PER;
  // set the peripheral address at USART2_DR register location
  DMA_SPAR(DMA1, 6) = (USART2 + 0x04);
  // set memory location at the address of the dmaOutput variable
  DMA_SM0AR(DMA1, 6) = (int)&dmaOutputBuff[0];
  // transfers 
  DMA_SNDTR(DMA1, 6) = k;
  // select USART2 TX for DMA1 Stream 6
  DMA_SCR(DMA1, 6) |= DMA_SCR_CHSEL(4);
  // enable memory pointer incrementing
  DMA_SCR(DMA1, 6) |= DMA_SCR_MINC;
  // clear interrupt flag
  DMA_HIFCR(DMA1) |= DMA_HIFCR_CTCIF6;
  // enable transfer complete interrupt, high priority level
  DMA_SCR(DMA1, 6) |= (DMA_SCR_TCIE | DMA_SCR_PL_HIGH);
  // enable DMA1 Stream 6 peripheral
  DMA_SCR(DMA1, 6) |= DMA_SCR_EN;

  va_end(args);  
}
