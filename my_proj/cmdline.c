#include "usart.h"
#include "util.h"
#include "i2c.h"
#include <string.h>

#define BUF_SIZE ((uint8_t)255)
#define MAX_HISTORY ((uint8_t)21)
#define DEC ((uint8_t)10)
#define HEX ((uint8_t)16)

static int incrementFlag = 0;
static int escapeFlag = 0;
static char cmdString[MAX_HISTORY][BUF_SIZE] = {{'\0'}};
static uint8_t cmdStringPos = 0;
static uint8_t cmdStringEndPos = 0;
static uint8_t cmdHistPos = 0;
static uint8_t cmdHistEndPos = 0;
static uint8_t i2cBusTotal = 1;

static const char i2c_buses[] =
"0.94 CS43L22 DAC\n\r"
"";

static const char help_cmd[] =
"i2c [r|w]                      - I2C commands\r\n"
"";

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

void display_cmd_string() {
  int i;
  // clear cmd line
  for(i = 0; i < 60; i++)
    print_string("\e[D");

  for(i = 0; i < 60; i++)
    print_char(' ');

  // print cmd string
  print_string("\r> %s", cmdString[0]);  


  // move cursor to current position
  i = cmdStringPos;
  while (cmdStringEndPos > i) {
    print_string("\e[D");
    i++;
  }

  /*  if there's a backspace in the middle of the string, the string isn't
   *  adding a character and therefore doesn't need to be incremented
   */
  if (incrementFlag) {
    cmdStringPos++;
    cmdStringEndPos++;
  }

  incrementFlag = 0;
  tail = (tail + 1) % BUF_SIZE;
}


void process_command() {

  int i;
  int i2cAddr = 0;
  int i2cReg = 0;
  int i2cBus = 0;
  uint32_t i2cData = 0;
  uint32_t i2cMasterBus = 0;
  int numOfBytes = 0;
  char tokenCmd[11][21] = {""};
  char i2cTokenCmd[5][4] = {""};
  char *tokPtr;

  i = 0;
  tokPtr = strtok(cmdString[0], " ");
  memcpy(tokenCmd[i], tokPtr, 20);

  while (*tokPtr != '\0') {
    i++;
    tokPtr = strtok(NULL, " ");
    memcpy(tokenCmd[i], tokPtr, 20);
  }


  if (!(strcmp(tokenCmd[0], "i2c"))) {

    if (!(strcmp(tokenCmd[1], "r"))) {

      i = 0;
      tokPtr = strtok(tokenCmd[2], ".");
      memcpy(i2cTokenCmd[i], tokPtr, 3);
      while (*tokPtr != '\0') {
        i++;
        tokPtr = strtok(NULL, ".");
        memcpy(i2cTokenCmd[i], tokPtr, 4);
      }

      i2cBus = string_to_number(i2cTokenCmd[0], HEX);
      if (i2cBus >= i2cBusTotal) {
        print_string("\r\nError: Not a valid bus number");
        return;
      }

      if (i != 3) {
        print_string("\r\nError: Incorrect formatting of bus.addr.reg");
        print_string("\r\nI2C Usage: i2c r <bus>.<addr>.<reg>" \
          " <number of bytes>");
        return;
      }

      // think of a prettier way of doing this
      if (tokenCmd[3][0] != '\0') {
        // convert to number
        // check to make sure it's less than 101
        numOfBytes = string_to_number(tokenCmd[3], DEC);

        if (numOfBytes > 100) {
          print_string("\r\nError: Max number of bytes is 100");
          return;
        }
      }
      else
        numOfBytes = 1;


      switch(i2cBus) {
        case 0:
          i2cMasterBus = I2C1;
          break;

        case 1:
          i2cMasterBus = I2C2;
          break;

        case 2:
          i2cMasterBus = I2C3;
          break;
      }

      i2cAddr = string_to_number(i2cTokenCmd[1], HEX);
      i2cReg = string_to_number(i2cTokenCmd[2], HEX);

      
      if(i2c_read(i2cMasterBus, i2cAddr, i2cReg, numOfBytes)) {
        print_string("\r\nData: 0x");

        i = 0;
        while (numOfBytes > 0) {
          print_string("%x", i2cBuff[i]);
          numOfBytes--;
          i++;
        }
      }
      
      return;
      
    }

    if (!(strcmp(tokenCmd[1], "w"))) {

      i = 0;
      tokPtr = strtok(tokenCmd[2], ".");
      memcpy(i2cTokenCmd[i], tokPtr, 3);
      while (*tokPtr != '\0') {
        i++;
        tokPtr = strtok(NULL, ".");
        memcpy(i2cTokenCmd[i], tokPtr, 8);
      }

      i2cBus = string_to_number(i2cTokenCmd[0], HEX);
      if (i2cBus >= i2cBusTotal) {
        print_string("\r\nError: Not a valid bus number");
        return;
      }

      if (i != 4) {
        print_string("\r\nError: Incorrect formatting of bus.addr.reg");
        print_string("\r\nI2C Usage: i2c w <bus>.<addr>.<reg>.<data>");
        return;
      }

      switch(i2cBus) {
        case 0:
          i2cMasterBus = I2C1;
          break;

        case 1:
          i2cMasterBus = I2C2;
          break;

        case 2:
          i2cMasterBus = I2C3;
          break;
      }

      i2cAddr = string_to_number(i2cTokenCmd[1], HEX);
      i2cReg = string_to_number(i2cTokenCmd[2], HEX);
      i2cData = string_to_number(i2cTokenCmd[3], HEX);

      if(i2c_write(i2cMasterBus, i2cAddr, i2cReg, i2cData));
        print_string("\r\nWrite successful");
      
      return;
    }

    if (tokenCmd[1][0] == '\0') {
      print_string("\r\nI2C Buses:\r\n%s", i2c_buses);
      return;
    }
  }

  if (!(strcmp(tokenCmd[0], "help"))) {
    print_string("\r\n%s", help_cmd);
    return;
  }

  if (!(strcmp(tokenCmd[0], "--help"))) {
    print_string("\r\n%s", help_cmd);
    return;
  }
  
  if (tokenCmd[0][0] == '\0') {
    // nothing was entered, just ret
  }
  else {
    print_string("\r\nError: Not a valid command");
    print_string("\r\nTry help or --help");
    // not a valid command, try "help" or "--help"
  }
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

void process_input() {
  if (tail < head)
  {
    int i;
    int j;

    if (!escapeFlag) {
      switch(inputData[tail]) {
        case 0x0D:                                    /* enter key */

          if (cmdHistEndPos < MAX_HISTORY)
            cmdHistEndPos++;

          j = cmdHistEndPos;
          while (j > 0) {
            i = 0;
            clear_string(cmdString[j], BUF_SIZE, 0);
            while (cmdString[j - 1][i] != '\0') {
              cmdString[j][i] = cmdString[j - 1][i];
              i++;
            }
            cmdString[j][i] = '\0';
            j--;
          }

          process_command();

          print_string("\r\n> ");
          clear_string(cmdString[0], BUF_SIZE, 0);

          cmdStringPos = 0;
          cmdStringEndPos = 0;
          cmdHistPos = 0;


          tail = (tail + 1) % BUF_SIZE;
          return;

        case 0x1B:                                    /* escape seq */
          escapeFlag++;
          tail = (tail + 1) % BUF_SIZE;
          return;

        case 0x7F:                                    /* backspace */

          if (cmdStringPos > 0) {

            i = cmdStringPos;
            if (cmdStringPos != cmdStringEndPos) {

             /*  backspace is entered but the cursor is somewhere in the middle
              *  of the string
              */

              /* shifting everything in front of the cursor back one */
              while (cmdString[0][i] != '\0') {
                cmdString[0][i - 1] = cmdString[0][i];
                i++;
              }

              cmdString[0][i - 1] = '\0';

              cmdStringPos--;
              cmdStringEndPos--;

              break; 
            }

            else {
              /*  backspace is entered and cursor is at end of string */

              /* delete last character in string */
              cmdString[0][i - 1] = '\0';
              cmdStringPos--;
              cmdStringEndPos--;          

              break;
            }
          }

        default:
          if (cmdStringPos < cmdStringEndPos) {
            /* the cursor is not at the end of the string */

            /* shift every element forward one to accommodate new entry */
            i = cmdStringEndPos;
            while (i >= cmdStringPos) {
              cmdString[0][i + 1] = cmdString[0][i];
              i--;
            }

            /* load the latest element into the cmd string to be displayed */
            cmdString[0][cmdStringPos] = inputData[tail];
          }

          else {
            /* the cursor is at the end of the string so new value is loaded */
            cmdString[0][cmdStringPos] = inputData[tail];
          }
          
          incrementFlag = 1;
          break;
      }
    }

    else {
      if (escapeFlag == 2) {
        switch(inputData[tail]) {
          case 'A':                                 /* up arrow */
            if (cmdHistPos < cmdHistEndPos) {
              cmdHistPos++;

              i = 0;
              while (cmdString[cmdHistPos][i] != '\0') {
                cmdString[0][i] = cmdString[cmdHistPos][i];
                i++;
              }

              cmdStringPos = i;
              cmdStringEndPos = i;

              clear_string(cmdString[0], (BUF_SIZE - cmdStringEndPos), 
                           cmdStringPos); 
            }
            break;

          case 'B':                                 /* down arrow */
            if (cmdHistPos > 0) {
              cmdHistPos--;

              if ((cmdHistPos == 0) && (cmdHistEndPos != 0)) {
                clear_string(cmdString[0], (BUF_SIZE - cmdStringEndPos), 0);

                cmdStringPos = 0;
                cmdStringEndPos = 0;

                break;
              }

              i = 0;
              while (cmdString[cmdHistPos][i] != '\0') {
                cmdString[0][i] = cmdString[cmdHistPos][i];
                i++;
              }

              cmdStringPos = i;
              cmdStringEndPos = i;

              clear_string(cmdString[0], (BUF_SIZE - cmdStringEndPos), 
                           cmdStringPos);
            }
            break;

          case 'C':                                 /* right arrow */
            if (cmdStringPos < cmdStringEndPos) {
              cmdStringPos++;

            }
            break;

          case 'D':                                 /* left arrow */
            if (cmdStringPos > 0)
              cmdStringPos--;
            break;
        }

        escapeFlag = 0;
      }

      else {
        tail = (tail + 1) % BUF_SIZE;
        escapeFlag++;
        return;
      }
    }


    display_cmd_string();

  }
}

