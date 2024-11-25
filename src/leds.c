#include "leds.h"
#include "regs_internal.h"

void set_leds(int led_state)
{
	*((volatile unsigned long *)DEV_LED) = ~led_state;
}
