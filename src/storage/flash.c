#include "storage/flash.h"
#include "interrupt.h"

typedef uint32_t (*inram_function_t)(volatile uint32_t*);

#define INRAM_BASE (inram_function_t)0x80000000
#define INRAM_SIZE 0x180

#define FLASH_APPROM_BASE     (volatile uint32_t*)0xbf000000
#define FLASH_ALT_APPROM_BASE (volatile uint32_t*)0xbfe00000
#define FLASH_BOOTROM_BASE    (volatile uint32_t*)0xbfc00000
#define FLASH_DIAG_BASE       (volatile uint32_t*)0xbf400000

// flash_inram.S
uint32_t flash_get_device_id(volatile uint32_t* flash_base_address);

uint32_t __flash_invoke_inram_function(inram_function_t stored_inram_function, volatile uint32_t* flash_base_address)
{
	inram_function_t prepared_inram_function = INRAM_BASE;

	for(int i = 0; i < (INRAM_SIZE / sizeof(uint32_t)); i++)
	{
		*((uint32_t*)prepared_inram_function + i) = *((uint32_t*)stored_inram_function + i);
	}

	return prepared_inram_function(flash_base_address);
}

volatile uint32_t* __flash_get_base_address(flash_base base)
{
	switch(base)
	{
		case APPROM_BASE:
			return FLASH_APPROM_BASE;
		case ALT_APPROM_BASE:
			return FLASH_ALT_APPROM_BASE;
		case BOOTROM_BASE:
			return FLASH_BOOTROM_BASE;
		case DIAG_BASE:
			return FLASH_DIAG_BASE;
		default:
			return 0;
	}
}

void flash_init()
{
	//
}

void flash_close()
{
	//
}

uint32_t flash_get_identity(flash_base base)
{
	return __flash_invoke_inram_function(flash_get_device_id, __flash_get_base_address(base));
}