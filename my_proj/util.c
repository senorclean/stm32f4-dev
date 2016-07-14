#include "stm32f4xx.h"


/*  clear_string()
 *
 *  Takes string as input along with the value of its end position.
 *  Wipes out the inputted string with null characters
 *
 *  Returns: Nothing  
 */

void clear_string(char *cmdStringPtr, int startPoint)
{
  int i = 0;
  while (cmdStringPtr[i + startPoint] != '\0')
  {
    cmdStringPtr[i + startPoint] = '\0';
    i++;
  }

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


/*  print_char()
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
