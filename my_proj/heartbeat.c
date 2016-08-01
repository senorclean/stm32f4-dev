#include "timer.h"

static int ledFlag = 0;
static int hb_counter = 0;

void heartbeat()
{
	if (ledFlag)
	{
	  if (TIM_CCR1(TIM4) == 0)
	    ledFlag = 0;
	  else
	    TIM_CCR1(TIM4) -= 10;
	}
	else
	{
	  if (TIM_CCR1(TIM4) == 1000)
	  {
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