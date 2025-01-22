#include "storage/flash.h"
#include "../regs_internal.h"

typedef uint32_t (*inram_function_t)(volatile uint32_t*);

#define INRAM_BASE (inram_function_t)0x80000000
#define INRAM_SIZE 0x180

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

volatile uint32_t* __flash_get_base_address(selected_flash_base base)
{
	switch(base)
	{
		case USE_APPROM:
			return (volatile uint32_t*)APPROM_BASE_ADDRESS;
		case USE_ALT_APPROM:
			return (volatile uint32_t*)ALTROM_BASE_ADDRESS;
		case USE_BOOTROM:
			return (volatile uint32_t*)BOOTROM_BASE_ADDRESS;
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

flash_identity_t flash_get_identity(selected_flash_base base)
{
	static flash_identity_t identity;

	identity.id_data = __flash_invoke_inram_function(flash_get_device_id, __flash_get_base_address(base));

	return identity;
}