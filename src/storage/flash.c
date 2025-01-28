#include "storage/flash.h"
#include "../regs_internal.h"
#include "../utils.h"
#include "wtvsys.h"
#include "libc.h"

typedef uint32_t (*inram_function_t)(uint32_t, uint32_t, uint32_t, uint32_t);

#define INRAM_BASE               (inram_function_t)0x80000000
#define INRAM_SIZE               0x180
#define INRAM_FUNCTION_FAILED    0xffffffff
#define INRAM_FUNCTION_SUCCEEDED 0x00000000

#define SECTOR_SIZE_8K            8 * 1024
#define SECTOR_SIZE_16K          16 * 1024
#define SECTOR_SIZE_32K          32 * 1024
#define SECTOR_SIZE_64K          64 * 1024
#define SECTOR_SIZE_128K        128 * 1024
#define DEFAULT_SECTOR_SIZE     SECTOR_SIZE_128K

#define FLASH_SIZE_1MB          1 * 1024 * 1024
#define FLASH_SIZE_2MB          2 * 1024 * 1024
#define FLASH_SIZE_4MB          4 * 1024 * 1024

#define SECTORS_PER_BLOCK       2

typedef struct {
	bool flash_enabled;
	bool writable;
	flash_image_type image_type;
	volatile uint32_t* base_address;
	int8_t bank;
	flash_identity_t identity;
	inram_function_t erase_function;
	inram_function_t program_function;
} flash_context_t;

static flash_context_t flash_context;

static bool watchdog_was_enabled = false;

// flash_inram.S
uint32_t flash_get_device_id(uint32_t flash_base_address, uint32_t flash_sector_address, uint32_t arg2, uint32_t arg3);
uint32_t flash_mx_erase_sector(uint32_t flash_base_address, uint32_t flash_sector_address, uint32_t arg2, uint32_t arg3);
uint32_t flash_mx_program(uint32_t flash_base_address, uint32_t flash_sector_address, uint32_t data_address, uint32_t data_length);

uint32_t __flash_invoke_inram_function(inram_function_t stored_inram_function, uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
	inram_function_t prepared_inram_function = INRAM_BASE;

	for(int i = 0; i < (INRAM_SIZE / sizeof(uint32_t)); i++)
	{
		*((uint32_t*)prepared_inram_function + i) = *((uint32_t*)stored_inram_function + i);
	}

	return prepared_inram_function(arg0, arg1, arg2, arg3);
}

void __flash_resolve_identity(flash_image_type image_type)
{
	flash_context.base_address = flash_base_address_from_type(image_type);
	flash_context.bank = flash_bank_from_address(flash_context.base_address);
	flash_context.identity.id_data = __flash_invoke_inram_function(
		flash_get_device_id,
		(uint32_t)flash_context.base_address,
		0,
		0,
		0
	);

	uint32_t sysconfig = get_sysconfig();

	switch(flash_context.identity.device_id)
	{
		case AM29F400AB: // or MBM29F400B
		case AM29F400AT: // or MBM29F400T
			flash_context.flash_enabled = true;
			flash_context.identity.can_write = true;
			flash_context.identity.page_mode = true;
			flash_context.identity.sector_size = SECTOR_SIZE_128K;
			flash_context.identity.total_size = FLASH_SIZE_1MB;

			flash_context.identity.timings.page_access = 5;
			flash_context.identity.timings.initial_access = 9;
			flash_context.identity.timings.to_next_chip_enable = 0;
			flash_context.identity.timings.chip_enable_to_write_enable = 0;
			flash_context.identity.timings.write_enable = 6;
			flash_context.identity.timings.post_page_programming_wait = TICKS_FROM_US(202);

			flash_context.erase_function = flash_mx_erase_sector;
			flash_context.program_function = flash_mx_program;
			break;

		case AM29F800BB: // or MBM29F800B
		case AM29F800BT: // or MBM29F800T
			flash_context.flash_enabled = true;
			flash_context.identity.can_write = true;
			flash_context.identity.page_mode = true;
			flash_context.identity.sector_size = SECTOR_SIZE_128K;
			flash_context.identity.total_size = FLASH_SIZE_2MB;

			flash_context.identity.timings.page_access = 5;
			flash_context.identity.timings.initial_access = 9;
			flash_context.identity.timings.to_next_chip_enable = 0;
			flash_context.identity.timings.chip_enable_to_write_enable = 0;
			flash_context.identity.timings.write_enable = 6;
			flash_context.identity.timings.post_page_programming_wait = TICKS_FROM_US(202);

			flash_context.erase_function = flash_mx_erase_sector;
			flash_context.program_function = flash_mx_program;
			break;

		case MX29F1610:
			flash_context.flash_enabled = true;
			flash_context.identity.can_write = true;
			flash_context.identity.page_mode = true;
			flash_context.identity.sector_size = SECTOR_SIZE_128K;
			flash_context.identity.total_size = FLASH_SIZE_4MB;

			flash_context.identity.timings.page_access = 5;
			flash_context.identity.timings.initial_access = 9;
			flash_context.identity.timings.to_next_chip_enable = 0;
			flash_context.identity.timings.chip_enable_to_write_enable = 0;
			flash_context.identity.timings.write_enable = 6;
			flash_context.identity.timings.post_page_programming_wait = TICKS_FROM_US(202);

			flash_context.erase_function = flash_mx_erase_sector;
			flash_context.program_function = flash_mx_program;
			break;

		default:
			flash_context.flash_enabled = false;
			flash_context.identity.can_write = false;
			flash_context.identity.page_mode = true;
			flash_context.identity.sector_size = DEFAULT_SECTOR_SIZE;
			flash_context.identity.total_size = 0;

			flash_context.identity.timings.page_access = 0;
			flash_context.identity.timings.initial_access = 0;
			flash_context.identity.timings.to_next_chip_enable = 0;
			flash_context.identity.timings.chip_enable_to_write_enable = 0;
			flash_context.identity.timings.write_enable = 0;
			flash_context.identity.timings.post_page_programming_wait = 0;

			flash_context.erase_function = NULL;
			flash_context.program_function = NULL;
			break;
	}

	flash_set_timing(flash_context.bank, &flash_context.identity.timings);
}

volatile uint32_t* flash_base_address_from_type(flash_image_type image_type)
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


int8_t flash_bank_from_address(volatile uint32_t* flash_base_address)
{
	uint32_t _flash_base_address = (uint32_t)flash_base_address;

	if(_flash_base_address >= ROML_BASE_ADDRESS && _flash_base_address < (ROML_BASE_ADDRESS + ROML_DATA_SIZE))
	{
		return 0;
	}
	else if(_flash_base_address >= ROMU_BASE_ADDRESS && _flash_base_address < (ROMU_BASE_ADDRESS + ROMU_DATA_SIZE))
	{
		return 1;
	}
	else
	{
		return -1;
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

bool flash_can_write()
{
	return flash_context.identity.can_write;
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

volatile uint32_t* flash_get_base_address()
{
	return flash_context.base_address;
}

int8_t flash_get_bank()
{
	return flash_context.bank;
}

uint32_t flash_get_sector_size()
{
	return flash_context.identity.sector_size;
}

uint64_t flash_get_size()
{
	return flash_context.identity.total_size;
}

uint32_t flash_get_post_page_programming_wait()
{
	return flash_context.identity.timings.post_page_programming_wait;
}

void flash_set_timing(int8_t bank, const flash_rom_timings_t* timings)
{
	uint32_t timing_data = (timings->timing_data & ROM_CNTL_TIMING);

	if(bank == 0 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL0, ((REGISTER_READ(RIO_ROM_CNTL0) & ~ROM_CNTL_TIMING) | timing_data));
	}

	if(bank == 1 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL1, ((REGISTER_READ(RIO_ROM_CNTL0) & ~ROM_CNTL_TIMING) | timing_data));
	}
}

flash_rom_timings_t flash_get_timing(int8_t bank)
{
	static flash_rom_timings_t timings;

	if(bank == 0)
	{
		timings.timing_data = REGISTER_READ(RIO_ROM_CNTL0) & ROM_CNTL_TIMING;
	}
	else if(bank == 1)
	{
		timings.timing_data = REGISTER_READ(RIO_ROM_CNTL0) & ROM_CNTL_TIMING;
	}

	return timings;
}

bool flash_page_mode_enabled(int8_t bank)
{
	bool bank0_enabled = true;
	bool bank1_enabled = true;

	if(bank == 0 || bank == -1)
	{
		bank0_enabled = ((REGISTER_READ(RIO_ROM_CNTL0) & ROM_CNTL_PAGE_MODE) != 0);
	}
	
	if(bank == 1 || bank == -1)
	{
		bank1_enabled = ((REGISTER_READ(RIO_ROM_CNTL1) & ROM_CNTL_PAGE_MODE) != 0);
	}

	return bank0_enabled && bank1_enabled;
}

void flash_page_mode_enable(int8_t bank)
{
	if(bank == 0 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL0, (REGISTER_READ(RIO_ROM_CNTL0) | ROM_CNTL_PAGE_MODE));
	}

	if(bank == 1 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL1, (REGISTER_READ(RIO_ROM_CNTL1) | ROM_CNTL_PAGE_MODE));
	}
}

void flash_page_mode_disable(int8_t bank)
{
	if(bank == 0 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL0, (REGISTER_READ(RIO_ROM_CNTL0) ^ ROM_CNTL_PAGE_MODE));
	}

	if(bank == 1 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL1, (REGISTER_READ(RIO_ROM_CNTL1) ^ ROM_CNTL_PAGE_MODE));
	}
}

bool flash_write_enabled(int8_t bank)
{
	bool bank0_enabled = true;
	bool bank1_enabled = true;

	if(bank == 0 || bank == -1)
	{
		bank0_enabled = ((REGISTER_READ(RIO_ROM_CNTL0) & ROM_CNTL_WRITE_PROTECT) != 0);
	}
	
	if(bank == 1 || bank == -1)
	{
		bank1_enabled = ((REGISTER_READ(RIO_ROM_CNTL1) & ROM_CNTL_WRITE_PROTECT) != 0);
	}

	return bank0_enabled && bank1_enabled;
}

void flash_write_enable(int8_t bank)
{
	if(bank == 0 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL0, (REGISTER_READ(RIO_ROM_CNTL0) | ROM_CNTL_WRITE_PROTECT));
	}
	
	if(bank == 1 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL1, (REGISTER_READ(RIO_ROM_CNTL1) | ROM_CNTL_WRITE_PROTECT));
	}
}

void flash_write_disable(int8_t bank)
{
	if(bank == 0 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL0, (REGISTER_READ(RIO_ROM_CNTL0) ^ ROM_CNTL_WRITE_PROTECT));
	}

	if(bank == 1 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL1, (REGISTER_READ(RIO_ROM_CNTL1) ^ ROM_CNTL_WRITE_PROTECT));
	}
}

bool flash_ce_delay_enabled(int8_t bank)
{
	bool bank0_enabled = true;
	bool bank1_enabled = true;

	if(bank == 0 || bank == -1)
	{
		bank0_enabled = ((REGISTER_READ(RIO_ROM_CNTL0) & ROM_CNTL_CE_DELAY) != 0);
	}
	
	if(bank == 1 || bank == -1)
	{
		bank1_enabled = ((REGISTER_READ(RIO_ROM_CNTL1) & ROM_CNTL_CE_DELAY) != 0);
	}

	return bank0_enabled && bank1_enabled;
}

void flash_ce_delay_enable(int8_t bank)
{
	if(bank == 0 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL0, (REGISTER_READ(RIO_ROM_CNTL0) | ROM_CNTL_CE_DELAY));
	}

	if(bank == 1 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL1, (REGISTER_READ(RIO_ROM_CNTL1) | ROM_CNTL_CE_DELAY));
	}
}

void flash_ce_delay_disable(int8_t bank)
{
	if(bank == 0 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL0, (REGISTER_READ(RIO_ROM_CNTL0) ^ ROM_CNTL_CE_DELAY));
	}

	if(bank == 1 || bank == -1)
	{
		REGISTER_WRITE(RIO_ROM_CNTL1, (REGISTER_READ(RIO_ROM_CNTL1) ^ ROM_CNTL_CE_DELAY));
	}
}

bool flash_read_data(uint64_t data_offset, void* data, uint32_t data_length)
{
	uint8_t* flash_data = ((uint8_t*)flash_context.base_address + data_offset);

	return (memcpy(data, flash_data, data_length) != NULL);
}

void __flash_start_write()
{
	flash_set_timing(flash_context.bank, &flash_context.identity.timings);

	flash_write_enable(flash_context.bank);
	flash_ce_delay_enable(flash_context.bank);

	watchdog_was_enabled = watchdog_enabled();
	
	watchdog_disable();
}

void __flash_finish_write()
{
	if(watchdog_was_enabled)
	{
		watchdog_enable();
	}

	flash_ce_delay_disable(flash_context.bank);
	flash_write_disable(flash_context.bank);
}

bool flash_write_data(uint64_t data_offset, void* data, uint32_t data_length)
{
	uint32_t result = false;

	__flash_start_write();

	if(flash_can_write())
	{
		uint32_t data_offset_adjusted = data_offset;
		void* data_adjusted = data;
		uint32_t data_length_adjusted = data_length;

		uint32_t sector_size = flash_get_sector_size();
		uint32_t sector_of_mask = (sector_size - 1);

		uint32_t left_offset = (data_offset & sector_of_mask);
		uint32_t right_offset = 0;
		if((data_length + left_offset) > sector_size)
		{
			right_offset = (data_length + left_offset) & sector_of_mask;
		}
		else
		{
			right_offset = 0;
		}

		if(left_offset > 0 || right_offset > 0)
		{
			uint32_t _left_rel_offset = left_offset;

			data_offset_adjusted = data_offset & ~sector_of_mask;
			// align up to nearest sector
			data_length_adjusted = (data_length + left_offset + right_offset + sector_of_mask) & ~sector_of_mask;
			data_adjusted = malloc(data_length_adjusted);

			if(left_offset > 0)
			{
				flash_read_data(data_offset_adjusted, data_adjusted, sector_size);
			}

			if(right_offset > 0 && (data_length_adjusted > sector_size || left_offset == 0))
			{
				uint32_t right_rel_offset = (data_length_adjusted - sector_size);
				uint32_t right_abs_offset = data_offset_adjusted + (data_length_adjusted - sector_size);

				flash_read_data(right_abs_offset, ((uint8_t*)(data_adjusted + right_rel_offset)), sector_size);
			}

			memcpy(((uint8_t*)(data_adjusted + _left_rel_offset)), data, data_length);
		}

		uint32_t sector_count = MAX((data_length_adjusted / sector_size), 1);
		uint32_t block_count = MAX((sector_count / SECTORS_PER_BLOCK), 1);

		uint8_t* flash_data = ((uint8_t*)flash_context.base_address + data_offset_adjusted);

		for(uint32_t sector_idx = 0, block_idx = 0, programmed_size = 0; block_idx < block_count; block_idx++)
		{
			uint32_t erased_size = 0;
			uint32_t sector_idx_stop = MIN(sector_count, (sector_idx + SECTORS_PER_BLOCK));

			for(; sector_idx < sector_idx_stop; sector_idx++)
			{
				uint32_t result = __flash_invoke_inram_function(
					flash_context.erase_function,
					(uint32_t)flash_context.base_address,
					(uint32_t)((uint8_t*)flash_data + erased_size),
					0,
					0
				);

				if(result != INRAM_FUNCTION_SUCCEEDED)
				{
					result = false;
					goto END_WRITE;
				}

				erased_size += sector_size;
			}

			uint32_t result = __flash_invoke_inram_function(
				flash_context.program_function,
				(uint32_t)flash_context.base_address,
				(uint32_t)flash_data,
				(uint32_t)((uint8_t*)data_adjusted + programmed_size),
				erased_size
			);

			if(result != INRAM_FUNCTION_SUCCEEDED)
			{
				result = false;
				goto END_WRITE;
			}

			programmed_size += erased_size;
			flash_data += erased_size;
		}

		if(data_adjusted != data)
		{
			free(data_adjusted);
		}

		result = true;
	}

END_WRITE:
	__flash_finish_write();

	return result;
}

