#include "my_usart.h"
#include "util.h"

#define BUF_SIZE ((uint8_t)255)
#define MAX_HISTORY ((uint8_t)21)

static int displayFlag = 0;
static int backFlag = 0;
static int escapeFlag = 0;
static char cmdString[MAX_HISTORY][BUF_SIZE] = {""};
static uint8_t cmdStringPos = 0;
static uint8_t cmdStringEndPos = 0;
static uint8_t cmdHistPos = 0;
static uint8_t cmdHistEndPos = 0;


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

void display_cmd_string()
{
  uint8_t i;

  if (!backFlag)
  { 
    /*  the key was not backspace while the cursor was in the middle of
     *  the string
     */
    if (cmdStringPos < cmdStringEndPos)
    {
      /* the cursor is not at the end of the string */

      /* shift every element forward one to accommodate new entry */
      i = cmdStringEndPos;
      while (i >= cmdStringPos)
      {
        cmdString[0][i + 1] = cmdString[0][i];
        i--;
      }

      /* load the latest element into the cmd string to be displayed */
      cmdString[0][cmdStringPos] = inputData[tail];
    }

    else
    {
      /* the cursor is at the end of the string so new value is loaded */
      cmdString[0][cmdStringPos] = inputData[tail];
    }
  }


  tail = (tail + 1) % BUF_SIZE;
  print_string("\r");
  print_string(cmdString[0]);

  /* move cursor backwards to reflect where the string's current position is */
  i = cmdStringPos;
  while (cmdStringEndPos > i)
  {
    print_string("\e[1D");
    i++;
  }

  /*  if there's a backspace in the middle of the string, the string isn't
   *  adding a character and therefore doesn't need to be incremented
   */
  if (!backFlag)
  {
    cmdStringPos++;
    cmdStringEndPos++;
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

void process_input()
{
  char numArray[10] = "";
  if (tail < head)
  {
    int i;
    int j;

    if (!escapeFlag)
    {
      switch(inputData[tail])
      {
        case 0x0D:                                    /* enter key */
          j = 0;

          if (cmdHistEndPos < MAX_HISTORY)
            cmdHistEndPos++;

          j = cmdHistEndPos;
          while (j > 0)
          {
            i = 0;
            clear_string(cmdString[j], 0);
            while (cmdString[j - 1][i] != '\0')
            {
              cmdString[j][i] = cmdString[j - 1][i];
              i++;
            }
            cmdString[j][i] = '\0';
            j--;
          }


          print_string("\r\n");
          clear_string(cmdString[0], 0);
          cmdStringPos = 0;
          cmdStringEndPos = 0;
          cmdHistPos = 0;


          tail = (tail + 1) % BUF_SIZE;
          displayFlag = 0;
          break;

        case 0x1B:                                    /* escape seq */
          escapeFlag++;
          tail = (tail + 1) % BUF_SIZE;
          return;

        case 0x7F:                                    /* backspace */

          i = cmdStringPos;

          if (cmdStringPos != cmdStringEndPos)      
          {

           /*  backspace is entered but the cursor is somewhere in the middle
            *  of the string
            */

            /* shifting everything in front of the cursor back one */
            while (cmdString[0][i] != '\0')
            {
              cmdString[0][i - 1] = cmdString[0][i];
              i++;
            }

            cmdString[0][i - 1] = '\0';

            /* making sure deletion stops at origin of string */
            if ((cmdStringPos != 0) && (cmdStringEndPos != 0))
            {
              cmdStringPos--;
              cmdStringEndPos--;
            }

            /* clearing the stale characters to the right of the cursor */
            for (i = cmdStringPos; i < cmdStringEndPos; i++)
              print_char(' ');

            backFlag = 1;
            displayFlag = 1;
            break; 
          }

          else
          {
            /*  backspace is entered and cursor is at end of string */

            /* making sure deletion stops at origin of string */
            cmdString[0][i - 1] = '\0';
            if (cmdStringPos != 0)
            {
              cmdStringPos--;
              cmdStringEndPos--;
            }

            /* clearing stale character in console line */
            print_string("\e[1D \e[1D");          

            tail = (tail + 1) % BUF_SIZE;
            displayFlag = 0;
            break;
          }


        default:
          displayFlag = 1;
          break;
      }
    }

    else
    {
      if (escapeFlag == 2)
      {
        switch(inputData[tail])
        {
          case 'A':                                 /* up arrow */
            if (cmdHistPos < cmdHistEndPos)
            {
              cmdHistPos++;

              i = cmdStringPos;
              while(i > 0)
              {
                print_char('\b');
                i--;
              }

              i = 0;
              while(cmdString[cmdHistPos][i] != '\0')
                i++;

              cmdStringPos = i;
              cmdStringEndPos = i;

              i = 0;
              while (cmdString[cmdHistPos][i] != '\0')
              {
                cmdString[0][i] = cmdString[cmdHistPos][i];
                i++;
              }

              cmdString[0][i] = '\0';
              clear_string(cmdString[0], (i + 1));

              /*number_to_ascii(cmdStringPos, &numArray[0]);
              print_string("\r\n");
              print_string(numArray);
              print_string("\r\n");*/


              for(i = 0; i < 60; i++)
                print_char(' ');

              print_string("\r");
              print_string(cmdString[0]);
              
              displayFlag = 0;
            }
            break;

          case 'B':                                 /* down arrow */
            if (cmdHistPos > 0)
            {
              i = 0;
              cmdHistPos--;

              if ((cmdHistPos == 0) && (cmdHistEndPos != 0))
              {
                clear_string(cmdString[0], 0);

                i = cmdStringPos;
                while(i > 0)
                {
                  print_char('\b');
                  i--;
                }

                for(i = 0; i < 60; i++)
                  print_char(' ');

                print_string("\r");
                cmdStringPos = 0;
                cmdStringEndPos = 0;


                displayFlag = 0;
                break;
              }

              i = cmdStringPos;
              while(i > 0)
              {
                print_char('\b');
                i--;
              }

              i = 0;
              while(cmdString[cmdHistPos][i] != '\0')
                i++;

              cmdStringPos = i;
              cmdStringEndPos = i;

              i = 0;
              while (cmdString[cmdHistPos][i] != '\0')
              {
                cmdString[0][i] = cmdString[cmdHistPos][i];
                i++;
              }


              cmdString[0][i] = '\0';

              for(i = 0; i < 60; i++)
                print_char(' ');

              print_string("\r");
              print_string(cmdString[0]);
              
              displayFlag = 0;
            }
            break;

          case 'C':                                 /* right arrow */
            if (cmdStringPos < cmdStringEndPos)
            {
              print_string("\e[1C");
              cmdStringPos++;
              displayFlag = 0;
            }
            break;

          case 'D':                                 /* left arrow */
            if (cmdStringPos != 0)
            {
              print_string("\e[1D");
              cmdStringPos--;
              displayFlag = 0;
            }
            break;
        }


        tail = (tail + 1) % BUF_SIZE;
        escapeFlag = 0;
      }

      else
      {
        tail = (tail + 1) % BUF_SIZE;
        escapeFlag++;
        return;
      }
    }

    if (displayFlag)
      display_cmd_string();

  }
}
