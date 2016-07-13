#include "stm32f4xx.h"

static int ledFlag = 0;
static int counter = 0;

void heartbeat()
{
	if (ledFlag)
	{
	  if (TIM4->CCR1 == 0)
	    ledFlag = 0;
	  else
	    TIM4->CCR1 -= 10;
	}
	else
	{
	  if (TIM4->CCR1 == 1000)
	  {
	    counter++;
	    if (counter == 20)
	    {
	      ledFlag = 1;
	      counter = 0;
	    }
	  }
	  else
	    TIM4->CCR1 += 10;
	}
}