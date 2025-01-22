#include "storage/flash.h"
#include "../regs_internal.h"

typedef uint32_t (*inram_function_t)(volatile uint32_t*);

#define INRAM_BASE (inram_function_t)0x80000000
#define INRAM_SIZE 0x180

typedef struct {
	bool flash_enabled;
	bool writable;
	flash_image_type image_type;
	volatile uint32_t* base_address;
	flash_identity_t identity;
	inram_function_t erase_function;
	inram_function_t program_function;
} flash_context_t;

static flash_context_t flash_context;

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

volatile uint32_t* __flash_get_base_address(flash_image_type image_type)
{
	switch(image_type)
	{
		case APPROM_FLASH:
			return (volatile uint32_t*)APPROM_BASE_ADDRESS;
		case ALT_APPROM_FLASH:
			return (volatile uint32_t*)ALTROM_BASE_ADDRESS;
		case BOOTROM_FLASH:
			return (volatile uint32_t*)BOOTROM_BASE_ADDRESS;
		default:
			return 0;
	}
}

void __flash_resolve_identity(flash_image_type image_type)
{
	flash_context.base_address = __flash_get_base_address(image_type);
	flash_context.identity.id_data = __flash_invoke_inram_function(flash_get_device_id, flash_context.base_address);

	switch(flash_context.identity.device_id)
	{
		case AM29F400AB: // or MBM29F400B
		case AM29F400AT: // or MBM29F400T
		case AM29F800BB: // or MBM29F800B
		case AM29F800BT: // or MBM29F800T
		case MX29F1610:
			flash_context.flash_enabled = true;
			flash_context.identity.can_write = true;
			break;
		default:
			flash_context.flash_enabled = false;
			flash_context.identity.can_write = false;
			break;
	}
}

void flash_init(flash_image_type image_type)
{
	__flash_resolve_identity(image_type);

	if(flash_enabled())
	{
		flash_context.writable = flash_context.identity.can_write;
		flash_context.image_type = image_type;
	}
}

void flash_close()
{
	//
}

bool flash_enabled()
{
	return flash_context.flash_enabled;
}

flash_identity_t flash_get_identity()
{
	return flash_context.identity;
}

const char* flash_get_manufacture_name()
{
	switch(flash_context.identity.manufacture_id)
	{
		case FLASH_MFG_AMD:
			return "AMD";
		case FLASH_MFG_FUJITSU:
			return "Fujitsu";
		case FLASH_MFG_MITSUBISHI:
			return "Mitsubishi";
		case FLASH_MFG_ATMEL:
			return "Atmel";
		case FLASH_MFG_ST:
			return "STMicroelectronics";
		case FLASH_MFG_CATALYST:
			return "Catalyst";
		case FLASH_MFG_PANASONIC:
			return "Panasonic";
		case FLASH_MFG_ALLIANCE:
			return "Alliance Semiconductor";
		case FLASH_MFG_SANYO:
			return "SANYO";
		case FLASH_MFG_INTEL:
			return "Intel";
		case FLASH_MFG_TI:
			return "Texas Instruments";
		case FLASH_MFG_TOSHIBA:
			return "Toshiba";
		case FLASH_MFG_HYUNDAI:
			return "Hyundai Electronics";
		case FLASH_MFG_SHARP:
			return "Sharp";
		case FLASH_MFG_MACRONIX:
			return "Macronix";
		case FLASH_MFG_APPLE:
			return "Apple / GigaDevice Semiconductor";
		case FLASH_MFG_WINBOND:
			return "Winbond Electronic";
		case FLASH_MFG_NEXCOM:
			return "NEXCOM";
		default:
			return "Unknown";
	}

}

const char* flash_get_device_name()
{
	switch(flash_context.identity.device_id)
	{
		case AM29F400AB: // or MBM29F400B
			return "AM29F400AB or compatble 16-bit 5v 4Mbit boot bottom sector (512kB per IC, 2x for 1MB per box)";
		case AM29F400AT: // or MBM29F400T
			return "AM29F400AT or compatble 16-bit 5v 4Mbit boot top sector (512kB per IC, 2x for 1MB per box";
		case AM29F800BB: // or MBM29F800B
			return "AM29F800BB or compatble 16-bit 5v 8Mbit boot bottom sector (1MB per IC, 2x for 2MB per box)";
		case AM29F800BT: // or MBM29F800T
			return "AM29F800BT or compatble 16-bit 5v 8Mbit boot top sector (1MB per IC, 2x for 2MB per box)";
		case MX29F1610:
			return "MX29F1610 16-bit 5v 16Mbit (2MB per IC, 2x for 4MB per box)";
		default:
			return "Unknown";
	}
}