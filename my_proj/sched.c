#include "my_usart.h"
#include "cmdline.h"
#include "timer.h"
#include "heartbeat.h"
#include "my_gpio.h"
#include "i2c.h"

#define MAX_16BIT_INT_SIZE ((uint16_t)65535)

typedef struct {
  void    	(*f)(void);
  uint16_t  timeStamp;
  uint16_t  period;
} func_t;

static func_t funcArray[2] = 	{
                    						{heartbeat, 0, 10},
                    						{process_input, 0, 1},
                  						};


void scheduler() {
	int i;

	for (i = 0; i < (sizeof(funcArray) / sizeof(funcArray[0])); i++) {
		if (counter >= funcArray[i].timeStamp) {
			funcArray[i].f();
			funcArray[i].timeStamp = (counter + funcArray[i].period) \
																% MAX_16BIT_INT_SIZE;
		}
	}
}