#include "memory.h"
#include "leds.h"
#include "regs_internal.h"
#include "wtvsys.h"
#include "leds.h" // remove after release

void init_data_section()
{
	set_leds(LED_POWER);
	//for(uint32_t i = 0; i < (((build_header *)(__BUILD_BASE))->data_stored_size); i++)
	//{
		set_leds(LED_MESSAGE);
		//*((volatile char *)(__DATA_START + i)) = *((volatile char *)(GET_BF(data_stored_base) + i));
	//}
}

void init_memory()
{
	//set_leds(LED_CONNECTED);for(uint32_t i = 0; i < 0xffffffff; i++){*((volatile uint32_t *)BUS_CHIP_CNTL);}
	init_data_section();
}