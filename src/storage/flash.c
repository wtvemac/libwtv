#include "storage/flash.h"
#include "../regs_internal.h"

typedef uint32_t (*inram_function_t)(uint32_t, uint32_t, uint32_t, uint32_t);

#define INRAM_BASE          (inram_function_t)0x80000000
#define INRAM_SIZE          0x180

#define SECTOR_SIZE_8K        8 * 1024
#define SECTOR_SIZE_16K      16 * 1024
#define SECTOR_SIZE_32K      32 * 1024
#define SECTOR_SIZE_64K      64 * 1024
#define SECTOR_SIZE_128K    128 * 1024
#define DEFAULT_SECTOR_SIZE SECTOR_SIZE_128K

typedef struct {
	bool flash_enabled;
	bool writable;
	flash_image_type image_type;
	volatile uint32_t* base_address;
	uint32_t sector_size;
	flash_identity_t identity;
	inram_function_t erase_function;
	inram_function_t program_function;
} flash_context_t;

static flash_context_t flash_context;

// flash_inram.S
uint32_t flash_get_device_id(uint32_t flash_base_address, uint32_t flash_sector_address, uint32_t arg2, uint32_t arg3);
uint32_t flash_mx_erase_sector(uint32_t flash_base_address, uint32_t flash_sector_address, uint32_t arg2, uint32_t arg3);
uint32_t flash_mx_program(uint32_t flash_base_address, uint32_t flash_sector_address, uint32_t arg2, uint32_t arg3);

uint32_t __flash_invoke_inram_function(inram_function_t stored_inram_function, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
	inram_function_t prepared_inram_function = INRAM_BASE;

	for(int i = 0; i < (INRAM_SIZE / sizeof(uint32_t)); i++)
	{
		*((uint32_t*)prepared_inram_function + i) = *((uint32_t*)stored_inram_function + i);
	}

	return prepared_inram_function(arg0, arg1, arg2, arg3);
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
	flash_context.identity.id_data = __flash_invoke_inram_function(flash_get_device_id, (uint32_t)flash_context.base_address, 0, 0, 0);

	switch(flash_context.identity.device_id)
	{
		case AM29F400AB: // or MBM29F400B
		case AM29F400AT: // or MBM29F400T
		case AM29F800BB: // or MBM29F800B
		case AM29F800BT: // or MBM29F800T
		case MX29F1610:
			flash_context.flash_enabled = true;
			flash_context.identity.can_write = true;
			flash_context.sector_size = SECTOR_SIZE_128K;
			break;
		default:
			flash_context.flash_enabled = false;
			flash_context.identity.can_write = false;
			flash_context.sector_size = DEFAULT_SECTOR_SIZE;
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

#include "libc.h"

uint32_t flash_get_sector_size()
{
	return flash_context.sector_size;
}

uint32_t flash_get_programming_time()
{
	return 22;
}

bool flash_read_data(uint64_t data_offset, void* data, uint32_t data_length)
{
	uint8_t* flash_data = ((uint8_t*)flash_context.base_address + data_offset);

	return (memcpy(data, flash_data, data_length) != NULL);
}

bool flash_write_data(uint64_t data_offset, void* data, uint32_t data_length)
{
	uint32_t data_offset_adjusted = data_offset;
	void* data_adjusted = data;
	uint32_t data_length_adjusted = data_length;

	uint32_t sector_size = flash_get_sector_size();
	uint32_t sector_of_mask = (sector_size - 1);

	uint32_t left_underflow = (data_offset & sector_of_mask);
	uint32_t right_underflow = (data_length + left_underflow) & sector_of_mask;

	if(left_underflow > 0 || right_underflow > 0)
	{
		uint32_t _left_rel_offset = left_underflow;

		data_offset_adjusted = data_offset & ~sector_of_mask;
		// align up to nearest sector
		data_length_adjusted = (data_length + left_underflow + right_underflow + sector_of_mask) & ~sector_of_mask;
		data_adjusted = malloc(data_length_adjusted);

		if(left_underflow > 0)
		{
			flash_read_data(data_offset_adjusted, data_adjusted, sector_size);
		}

		if(right_underflow > 0 && (data_length_adjusted > sector_size || left_underflow == 0))
		{
			uint32_t right_rel_offset = (data_length_adjusted - sector_size);
			uint32_t right_abs_offset = data_offset_adjusted + (data_length_adjusted - sector_size);

			flash_read_data(right_abs_offset, ((uint8_t*)(data_adjusted + right_rel_offset)), sector_size);
		}

		memcpy(((uint8_t*)(data_adjusted + _left_rel_offset)), data, data_length);
	}

	uint32_t sector_count = (data_length_adjusted / sector_size);
	
	uint8_t* flash_data = ((uint8_t*)flash_context.base_address + data_offset_adjusted);
	//for(uint32_t sector_idx = 0; sector_idx < sector_count; sector_idx++)
	//{
		uint8_t* block_data = data_adjusted;
		uint32_t block_size = data_length_adjusted;

		uint32_t result1 = __flash_invoke_inram_function(flash_mx_erase_sector, (uint32_t)flash_context.base_address, (uint32_t)flash_data, 0, 0);
		uint32_t result2 = __flash_invoke_inram_function(flash_mx_program, (uint32_t)flash_context.base_address, (uint32_t)flash_data, (uint32_t)block_data, block_size);

		//flash_data += sector_size;
	//}

	if(data_adjusted != data)
	{
		free(data_adjusted);
	}

	return true;
}

