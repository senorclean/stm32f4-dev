#include "timer.h"

static int ledFlag = 0;
static int hb_counter = 0;


/*  heartbeat()
 *
 *  Arguments: None
 *
 *  Raises and lowers the duty cycle of TIM4 to create a heartbeat effect of
 * 	the green LED (PD12) on the board  
 *
 *  Returns: Nothing  
 */

void heartbeat()
{
	// if the LED is being dimmed
	if (ledFlag)
	{
		// LED is completely off, change flag to begin brightening
	  if (TIM_CCR1(TIM4) == 0)
	    ledFlag = 0;
	  else
	    TIM_CCR1(TIM4) -= 10;
	}
	// the LED is being brightened
	else
	{
		// LED is completely on
	  if (TIM_CCR1(TIM4) == 1000)
	  {
	  	// wait ~20 ms for a better heartbeat effect and then begin dimming
	    hb_counter++;
	    if (hb_counter == 20)
	    {
	      ledFlag = 1;
	      hb_counter = 0;
	    }
	  }
	  else
	    TIM_CCR1(TIM4) += 10;
	}
}