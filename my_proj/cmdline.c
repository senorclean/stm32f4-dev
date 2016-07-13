#include "my_usart.h"
#include "util.h"

#define BUF_SIZE ((uint8_t)255)

static int displayFlag = 0;
static int backFlag = 0;
static char cmdString[BUF_SIZE] = "";
static uint8_t cmdStringPos = 0;
static uint8_t cmdStringEndPos = 0;


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
        cmdString[i + 1] = cmdString[i];
        i--;
      }

      /* load the latest element into the cmd string to be displayed */
      cmdString[cmdStringPos] = inputData[tail];
    }

    else
    {
      /* the cursor is at the end of the string so new value is loaded */
      cmdString[cmdStringPos] = inputData[tail];
    }
  }


  tail = (tail + 1) % BUF_SIZE;
  print_string("\r");
  print_string(cmdString);

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
  if (tail < head)
    {
      int i = 0;

      switch(inputData[tail])
      {
        case 0x0D:                                    /* enter key */
          print_string("\r\n");
          clear_string(cmdString);
          cmdStringPos = 0;
          cmdStringEndPos = 0;

          tail = (tail + 1) % BUF_SIZE;
          displayFlag = 0;
          break;

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
              if (cmdStringPos < cmdStringEndPos)
              {
                print_string("\e[1C");
                cmdStringPos++;
              }
              break;

            case 'D':                                 /* left arrow */
              if (cmdStringPos != 0)
              {
                print_string("\e[1D");
                cmdStringPos--;
              }
              break;
          }


          tail = (tail + 1) % BUF_SIZE;
          displayFlag = 0;
          break;

        case 0x7F:                                    /* backspace */

          i = cmdStringPos;


          if (cmdStringPos != cmdStringEndPos)      
          {

           /*  backspace is entered but the cursor is something in the middle
            *  of the string
            */

            /* shifting everything in front of the cursor back one */
            while (cmdString[i] != '\0')
            {
              cmdString[i - 1] = cmdString[i];
              i++;
            }

            cmdString[i - 1] = '\0';

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
            cmdString[i] = cmdString[i + 1];
            if ((cmdStringPos != 0) && (cmdStringEndPos != 0))
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

      if (displayFlag)
        display_cmd_string();

    }
}
