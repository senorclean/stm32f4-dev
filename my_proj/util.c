#include "stm32f4xx.h"
#include <stdarg.h>


/*  clear_string()
 *
 *  Takes string as input along with the value of its end position.
 *  Wipes out the inputted string with null characters
 *
 *  Returns: Nothing  
 */

void clear_string(char *cmdStringPtr, int iterations) {
  int i;
  for(i = 0; i < iterations; i++)
    cmdStringPtr[i] = '\0';
}


/*  reverse_array()
 *
 *  Takes string as an input and reverses it. Named this one pretty well.
 *
 *  Returns: Nothing  
 */

void reverse_array(char *str) {
  int i = 0;
  int j = 0;
  char tempStr[11] = "";
  int stopPoint = 0;


  // if it's a negative number
  if (str[0] == '-') {
    tempStr[0] = '-';
    j = 1;
    i = 1;
    stopPoint = 1;
  }

  // if it's a hex number
  if (str[1] == 'x') {
    tempStr[0] = '0';
    tempStr[1] = 'x';
    j = 2;
    i = 2;
    stopPoint = 2;
  }

  while (str[i] != '\0')
    i++;

  i--;
  while (i >= stopPoint) {
    tempStr[j] = str[i];
    i--;
    j++;
  }
  
  i = 0;
  while (tempStr[i] != '\0') {
    str[i] = tempStr[i];
    i++; 
  }
}


/*  decimal_to_ascii()
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

void decimal_to_ascii(int value, char *tempArray) {
  int i;
  char x;

  /* seems arbitrary but a 32-bit value can't be longer than 10 digits */
  for (i = 0; i < 10; i++) {
    if (value < 0) {
      tempArray[i] = '-';
      value = ~value + 1;
      continue;
    }

    if ((value == 0) && (i == 0)) {
      tempArray[i] = '0';
      i++;
      break;
    }
    else
      if (value == 0)
        break;

    x = (value % 10);
    value /= 10;
    tempArray[i] = '0' + x;
  }

  if (i > 1)
    reverse_array(tempArray);

  if (tempArray[i] != '\0')
    tempArray[i] = '\0';
}


void hex_to_ascii(int value, char *tempArray) {
  int i;
  char x;

  tempArray[0] = '0';
  tempArray[1] = 'x';


  /* seems arbitrary but a 32-bit value can't be longer than 10 digits */
  for (i = 2; i < 10; i++) {
    if ((value == 0) && (i == 2)) {
      tempArray[i] = '0';
      i++;
      break;
    }
    else
      if (value == 0)
        break;

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

  if (i > 3)
    reverse_array(tempArray);

  if (tempArray[i] != '\0')
    tempArray[i] = '\0';
}


/*  print_string()
 *
 *  Takes string as input and sends each character out in a loop
 *
 *  Returns: Nothing  
 */

void print_string(char *data, ...) {
  int i = 0;
  int j = 0;
  char *tempStr;
  int tempInt = 0;
  char tempArray[11] = "";
  va_list args;

  va_start(args, data);


  while (data[i] != '\0') {
    if (data[i] == '%') {
      i++;
      switch(data[i]) {
        case 'd':
          tempInt = va_arg(args, int);
          decimal_to_ascii(tempInt, &tempArray[0]);

          j = 0;
          while(tempArray[j] != '\0') {
            while(!(USART2->SR & USART_SR_TXE));
            USART2->DR = tempArray[j];
            j++; 
          }

          //clear_string(tempArray, 10);
          break;

        case 's':
          tempStr = va_arg(args, char*);

          j = 0;
          while (*(tempStr + j) != '\0') {
            while(!(USART2->SR & USART_SR_TXE));
            USART2->DR = *(tempStr + j);
            j++; 
          }
          break;

        case 'x':
          // fall through

        case 'X':
          tempInt = va_arg(args, int);
          hex_to_ascii(tempInt, &tempArray[0]);

          j = 0;
          while(tempArray[j] != '\0') {
            while(!(USART2->SR & USART_SR_TXE));
            USART2->DR = tempArray[j];
            j++; 
          }

          //clear_string(tempArray, 10);
          break;

        default:
          break;
      }

      i++;
    }
    else {
      while(!(USART2->SR & USART_SR_TXE));
      USART2->DR = data[i];
      i++; 
    } 
  }

  va_end(args);  
}


/*  print_char()
 *
 *  Takes a single character as input and prints it out. Will likely be
 *  deprecated as print_string() can also perform this task
 *
 *  Returns: Nothing  
 */

void print_char(char data) {
  while(!(USART2->SR & USART_SR_TXE));
  USART2->DR = data;
}
