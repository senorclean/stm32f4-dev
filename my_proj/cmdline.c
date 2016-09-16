#include "usart.h"
#include "util.h"
#include "i2c.h"
#include <string.h>


#define ENTER_KEY ((uint8_t)0x0D)
#define ESCAPE_SEQ ((uint8_t)0x1B)
#define BACKSPACE_KEY ((uint8_t)0x7F)
#define CMD_BUF_SIZE ((uint8_t)255)
#define MAX_HISTORY ((uint8_t)21)
#define DEC ((uint8_t)10)
#define HEX ((uint8_t)16)

static int incrementFlag = 0;
static int escapeFlag = 0;
static char cmdString[MAX_HISTORY][CMD_BUF_SIZE] = {{'\0'}};
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
 *  Arguments: None
 *
 *  Prints the current command line string while taking into account the
 *  user's cursor position
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

  /*  if the character entered was a valid character to be displayed, the
   *  command string positions are incremented
   */
  if (incrementFlag) {
    cmdStringPos++;
    cmdStringEndPos++;
  }

  incrementFlag = 0;
  tail = (tail + 1) % CMD_BUF_SIZE;
}


/*  process_command()
 *
 *  Arguments: None
 *
 *  After the user has entered a command, process_command takes that string
 *  and tokenizes it. From there it determines whether the user has input
 *  a valid command and if so executes that command.
 *
 *  Returns: Nothing  
 */

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

  // separates the first token of cmdString[0] with space delimiter
  tokPtr = strtok(cmdString[0], " ");
  // copies that first token into an array. 20 bytes was chosen arbitrarily
  memcpy(tokenCmd[i], tokPtr, 20);

  // process above is repeated until cmdString[0] is empty
  while (*tokPtr != '\0') {
    i++;
    /* NULL is used here because it tells strtok to continue from the last 
     * memory location it stopped at (the delimiter) 
     */
    tokPtr = strtok(NULL, " ");
    memcpy(tokenCmd[i], tokPtr, 20);
  }

  // I2C command was entered
  if (!(strcmp(tokenCmd[0], "i2c"))) {

    // I2C read
    if (!(strcmp(tokenCmd[1], "r"))) {

      i = 0;

      // parses the bus.addr.reg field
      tokPtr = strtok(tokenCmd[2], ".");
      memcpy(i2cTokenCmd[i], tokPtr, 3);
      while (*tokPtr != '\0') {
        i++;
        tokPtr = strtok(NULL, ".");
        memcpy(i2cTokenCmd[i], tokPtr, 4);
      }

      // determine if the I2C bus number entered is valid
      i2cBus = string_to_number(i2cTokenCmd[0], HEX);
      if (i2cBus >= i2cBusTotal) {
        print_string("\r\nError: Not a valid bus number");
        return;
      }

      // the user entered an incorrect format for the bus.addr.reg field
      if (i != 3) {
        print_string("\r\nError: Incorrect formatting of bus.addr.reg");
        print_string("\r\nI2C Usage: i2c r <bus>.<addr>.<reg>" \
          " <number of bytes>");
        return;
      }

      // if a number of bytes was specified by the user
      if (tokenCmd[3][0] != '\0') {
        // check that the number of bytes is less than 101 (arbitrary)
        numOfBytes = string_to_number(tokenCmd[3], DEC);
        if (numOfBytes > 100) {
          print_string("\r\nError: Max number of bytes is 100");
          return;
        }
      }
      else {
        // default to one byte if nothing is specified
        numOfBytes = 1;
      }

      // determine I2C bus to use based on user's input
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

      // perform I2C read and print data if successful       
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

    // I2C write
    if (!(strcmp(tokenCmd[1], "w"))) {

      // follows the same process as I2C read
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

      // Incorrect format for this I2C field
      if (i != 4) {
        print_string("\r\nError: Incorrect formatting of bus.addr.reg");
        print_string("\r\nI2C Usage: i2c w <bus>.<addr>.<reg>.<data> " \
          " (MSB first)");
        return;
      }

      // determines I2C bus based on user's input
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

      // performs I2C write
      if(i2c_write(i2cMasterBus, i2cAddr, i2cReg, i2cData));
        print_string("\r\nWrite successful");
      
      return;
    }

    /*  If the user only entered "i2c". This will print a list of I2C devices
     *  that the user can talk to
     */
    if (tokenCmd[1][0] == '\0') {
      print_string("\r\nI2C Devices:\r\n%s", i2c_buses);
      return;
    }
  }

  // print list of possible commands and functionalities
  if (!(strcmp(tokenCmd[0], "help"))) {
    print_string("\r\n%s", help_cmd);
    return;
  }

  // print list of possible commands and functionalities
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
  }
}


/*  process_input()
 *
 *  Arguments: None
 *
 *  Takes the latest character input and performs appropriate actions based
 *  on whether it is a special character or a displayable character
 *
 *  Returns: Nothing  
 */

void process_input() {
  // if there is data to be read from the input buffer
  if (tail < head)
  {
    int i;
    int j;

    // If an escape character wasn't previously entered
    if (!escapeFlag) {
      switch(inputData[tail]) {
        case ENTER_KEY:

          /* if the capacity of the command buffer's history hasn't been
           * reached, then increase the buffer's current history size
           */
          if (cmdHistEndPos < MAX_HISTORY)
            cmdHistEndPos++;

          /* shift all commands in the command buffer's history back one spot.
           * position zero is now free to be used again
           */ 
          j = cmdHistEndPos;
          while (j > 0) {
            i = 0;
            clear_string(cmdString[j], CMD_BUF_SIZE, 0);
            while (cmdString[j - 1][i] != '\0') {
              cmdString[j][i] = cmdString[j - 1][i];
              i++;
            }
            cmdString[j][i] = '\0';
            j--;
          }

          // take user's input and execute it
          process_command();

          // clear the position zero string
          print_string("\r\n> ");
          clear_string(cmdString[0], CMD_BUF_SIZE, 0);
          // reset command string variables
          cmdStringPos = 0;
          cmdStringEndPos = 0;
          cmdHistPos = 0;


          tail = (tail + 1) % CMD_BUF_SIZE;
          return;

        case ESCAPE_SEQ:
          /* with the escape flag set, the next two characters are known to be
           * garbage and thrown out
           */
          escapeFlag = 1;
          tail = (tail + 1) % CMD_BUF_SIZE;
          return;

        case BACKSPACE_KEY:

          /* if the cursor is anywhere but the origin. otherwise the backspace
           * is ignored
           */
          if (cmdStringPos > 0) {

            i = cmdStringPos;
            if (cmdStringPos != cmdStringEndPos) {

             /*  backspace is entered but the cursor is somewhere in the middle
              *  of the string
              */

              // shift everything in front of the cursor back one
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
              // backspace is entered and cursor is at end of string

              // delete last character in string
              cmdString[0][i - 1] = '\0';
              cmdStringPos--;
              cmdStringEndPos--;          

              break;
            }
          }

        // displayable character input is entered
        default:
          if (cmdStringPos < cmdStringEndPos) {
            // the cursor is not at the end of the string

            // shift every element forward one to accommodate new entry
            i = cmdStringEndPos;
            while (i >= cmdStringPos) {
              cmdString[0][i + 1] = cmdString[0][i];
              i--;
            }

            // load the latest element into the cmd string to be displayed
            cmdString[0][cmdStringPos] = inputData[tail];
          }

          else {
            // the cursor is at the end of the string so new value is loaded
            cmdString[0][cmdStringPos] = inputData[tail];
          }
          
          incrementFlag = 1;
          break;
      }
    }

    /*  an escape character was previoulsy entered and the subsequent
     *  characters haven't been dealt with yet
     */ 
    else {
      // character isn't '[' which comes before the important value
      if (escapeFlag == 2) {
        switch(inputData[tail]) {
          // up arrow
          case 'A':
            // if the current command isn't the oldest command in storage
            if (cmdHistPos < cmdHistEndPos) {
              cmdHistPos++;

              // copy stored command into position 0
              i = 0;
              while (cmdString[cmdHistPos][i] != '\0') {
                cmdString[0][i] = cmdString[cmdHistPos][i];
                i++;
              }

              cmdStringPos = i;
              cmdStringEndPos = i;

              // clear everything after the end of the new position 0 string
              clear_string(cmdString[0], (CMD_BUF_SIZE - cmdStringEndPos), 
                           cmdStringPos); 
            }
            break;

          // down arrow
          case 'B':
            // check to see if history position isn't already at origin
            if (cmdHistPos > 0) {
              cmdHistPos--;

              /*  if the history is now 0 and this isn't the first command,
               *  then erase contents of position 0 of cmd string so the
               *  terminal is now clear. I wanted this feature
               */
              if ((cmdHistPos == 0) && (cmdHistEndPos != 0)) {
                clear_string(cmdString[0], (CMD_BUF_SIZE - cmdStringEndPos), 0);

                cmdStringPos = 0;
                cmdStringEndPos = 0;

                break;
              }

              // load the contents of the new command into position 0
              i = 0;
              while (cmdString[cmdHistPos][i] != '\0') {
                cmdString[0][i] = cmdString[cmdHistPos][i];
                i++;
              }

              cmdStringPos = i;
              cmdStringEndPos = i;

              // clear everything after the end of the new position 0 string
              clear_string(cmdString[0], (CMD_BUF_SIZE - cmdStringEndPos), 
                           cmdStringPos);
            }
            break;

          // right arrow 
          case 'C':
            // if cursor isn't at the end of the string, move it to the right
            if (cmdStringPos < cmdStringEndPos)
              cmdStringPos++;
            break;

          // left arrow
          case 'D':
            // if cursor isn't at origin, move it left
            if (cmdStringPos > 0)
              cmdStringPos--;
            break;
        }

        escapeFlag = 0;
      }

      // '[' has been received and isn't used
      else {
        tail = (tail + 1) % CMD_BUF_SIZE;
        escapeFlag++;
        return;
      }
    }

    display_cmd_string();
  }
}

