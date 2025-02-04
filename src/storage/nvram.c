#include "storage/nvram.h"
#include "storage/ata.h"
#include "storage/flash.h"
#include "../utils.h"
#include "iic.h"
#include "libc.h"
#include "wtvsys.h"

#define NVRAM_PRIMARY_SIZE       0x4000
#define NVRAM_PRIMARY_ALIGN      0x04
#define NVRAM_PRIMARY_ALIGN_MASK (NVRAM_PRIMARY_ALIGN - 1)

typedef struct __attribute__((packed))
{
	uint32_t length;
	pri_nvram_setting_key key;
	uint8_t value[1];
} nv_pri_setting_t;

typedef struct __attribute__((packed))
{
	uint32_t checksum;
	uint32_t unknown0;
	uint32_t unknown1;
	uint32_t unknown2;
	nv_pri_setting_t first_setting;
} nv_pri_settings_t;

typedef struct
{
	bool enabled;
	nv_pri_settings_t* persistant_settings;

	struct
	{
		bool in_progress;
		nv_pri_setting_t* next_setting;
		nv_pri_settings_t* settings;
	} cur_transaction;
} fnv_pri_context_t;

static fnv_pri_context_t nv_pri_context;

nv_pri_settings_t* __nv_pri_buffer_init()
{
	nv_pri_settings_t* buffer = (nv_pri_settings_t*)malloc(NVRAM_PRIMARY_SIZE);
	memset(buffer, 0x00, NVRAM_PRIMARY_SIZE);

	return buffer;
}

bool __load_persistant_settings()
{
	if(is_diskful_box())
	{
		if(ata_enabled())
		{
			if(!nv_pri_context.enabled)
			{
				nv_pri_context.persistant_settings = __nv_pri_buffer_init();
			}
			return ata_read_data(0x200, nv_pri_context.persistant_settings, NVRAM_PRIMARY_SIZE);
		}
	}
	else if(flash_enabled())
	{
		if(!nv_pri_context.enabled)
		{
			nv_pri_context.persistant_settings = __nv_pri_buffer_init();
		}
		return flash_read_data(0x1fc000, nv_pri_context.persistant_settings, NVRAM_PRIMARY_SIZE);
	}

	return false;
}

void nvram_primary_test_print()
{
	for(nv_pri_setting_t* neat = &nv_pri_context.persistant_settings->first_setting; neat->key != NVPRI_NULL;)
	{
		printf("\t\tkey=0x%08x [%.4s], length=0x%08x: %08x...\n", neat->key, (char*)&neat->key, neat->length, *((uint32_t*)&neat->value)); // (minimum value size is 4 bytes)
		
		neat = (nv_pri_setting_t*)(&neat->value + ((neat->length + NVRAM_PRIMARY_ALIGN_MASK) & ~NVRAM_PRIMARY_ALIGN_MASK));
	}
}

void nvram_primary_init()
{
	nv_pri_context.enabled = __load_persistant_settings();
	if(!nv_pri_context.enabled && nv_pri_context.persistant_settings != NULL)
	{
		nvram_primary_close();
	}
}

void nvram_primary_close()
{
	free(nv_pri_context.persistant_settings);
	nv_pri_context.persistant_settings = NULL;
	nv_pri_context.enabled = false;
}

bool nvram_primary_enabled()
{
	return nv_pri_context.enabled;
}

void nvram_primary_trans_start()
{
	if(nv_pri_context.cur_transaction.in_progress)
	{
		memset(nv_pri_context.cur_transaction.settings, 0x00, NVRAM_PRIMARY_SIZE);
	}
	else
	{
		nv_pri_context.cur_transaction.settings = __nv_pri_buffer_init();
	}

	nv_pri_context.cur_transaction.next_setting = &nv_pri_context.cur_transaction.settings->first_setting;
	nv_pri_context.cur_transaction.in_progress = true;
}

void nvram_primary_trans_restart()
{
	nvram_primary_trans_start();
}

bool nvram_primary_trans_insert(pri_nvram_setting_key setting_key, void* data, uint32_t length)
{
	if(nv_pri_context.cur_transaction.in_progress)
	{
		nv_pri_setting_t* next_setting = nv_pri_context.cur_transaction.next_setting;
		uint8_t* nvram_pri_end = ((uint8_t*)nv_pri_context.cur_transaction.settings + NVRAM_PRIMARY_SIZE);

		if(next_setting != NULL)
		{
			next_setting->key = setting_key;
			next_setting->length = length;

			if(((uint8_t*)&next_setting->value + length) < nvram_pri_end)
			{
				memcpy(&next_setting->value, data, length);
				nv_pri_context.cur_transaction.next_setting = (nv_pri_setting_t*)(&next_setting->value + ((next_setting->length + NVRAM_PRIMARY_ALIGN_MASK) & ~NVRAM_PRIMARY_ALIGN_MASK));

				return true;
			}
			else
			{
				nv_pri_context.cur_transaction.next_setting = NULL;
			}
		}
	}

	return false;
}

bool nvram_primary_trans_commit()
{
	if(nv_pri_context.cur_transaction.in_progress)
	{
		bool result = false;

		uint8_t* data = ((uint8_t*)nv_pri_context.cur_transaction.settings + 4);
		nv_pri_context.cur_transaction.settings->checksum = 0x00000000;
		for(uint32_t i = 0; i < NVRAM_PRIMARY_SIZE; i += 4)
		{
			nv_pri_context.cur_transaction.settings->checksum += *((uint32_t*)(data + i));
		}

		if(is_diskful_box())
		{
			if(ata_enabled())
			{
				result = ata_write_data(0x200, nv_pri_context.cur_transaction.settings, NVRAM_PRIMARY_SIZE);
			}
		}
		else if(flash_enabled())
		{
			result = flash_write_data(0x1fc000, nv_pri_context.cur_transaction.settings, NVRAM_PRIMARY_SIZE);
		}

		free(nv_pri_context.cur_transaction.settings);
		nv_pri_context.cur_transaction.in_progress = false;

		return result && __load_persistant_settings();
	}
	else
	{
		return false;
	}
}

void* nvram_primary_read(pri_nvram_setting_key setting_key, uint32_t* length, bool use_persistant_settings)
{
	nv_pri_settings_t* settings = NULL;
	if(use_persistant_settings)
	{
		settings = nv_pri_context.persistant_settings;
	}
	else
	{
		settings = nv_pri_context.cur_transaction.settings;
	}

	for(nv_pri_setting_t* setting = &settings->first_setting; setting->key != NVPRI_NULL;)
	{
		if(setting->key == setting_key)
		{
			*length = setting->length;
			return (void*)&setting->value;
		}
		
		setting = (nv_pri_setting_t*)(&setting->value + ((setting->length + NVRAM_PRIMARY_ALIGN_MASK) & ~NVRAM_PRIMARY_ALIGN_MASK));
	}

	*length = 0;
	return NULL;
}

bool nvram_primary_write(pri_nvram_setting_key setting_key, void* data, uint32_t length, bool use_persistant_settings)
{
	bool result = false;

	nv_pri_settings_t* settings = NULL;
	if(use_persistant_settings)
	{
		settings = nv_pri_context.persistant_settings;
	}
	else
	{
		settings = nv_pri_context.cur_transaction.settings;
	}

	nvram_primary_trans_start();

	// Making sure the TellyScript or DialScript are inserted first so it matches what the WebTV OS does.
	// The rest of the order isn't matched for no other reason than the TS/DS is the largest and one of the more important settings.
	// The WebTV OS usually wiped all setting and re-commits them after power off in a set order.
	if(setting_key == NVPRI_TLLY || setting_key == NVPRI_DLSC)
	{
		if(!nvram_primary_trans_insert(setting_key, data, length))
		{
			return false;
		}
	}

	// Insert other currenctly active settings.
	for(nv_pri_setting_t* setting = &settings->first_setting; setting->key != NVPRI_NULL;)
	{
		if(setting->key != setting_key)
		{
			if(!nvram_primary_trans_insert(setting->key, (void*)&setting->value, setting->length))
			{
				return false;
			}
		}
		
		setting = (nv_pri_setting_t*)(&setting->value + ((setting->length + NVRAM_PRIMARY_ALIGN_MASK) & ~NVRAM_PRIMARY_ALIGN_MASK));
	}

	// If the setting isn't for the TellyScript or DialScript, then insert it after.
	if(setting_key != NVPRI_TLLY && setting_key != NVPRI_DLSC)
	{
		if(!nvram_primary_trans_insert(setting_key, data, length))
		{
			return false;
		}
	}

	if(use_persistant_settings)
	{
		return nvram_primary_trans_commit();
	}
	else
	{
		return true;
	}
}

void nvram_secondary_read(sec_nvram_offset offset, void* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		*((uint8_t*)data) = iic_read_byte(SECONDARY_NVRAM_IIC_ADDRESS, offset++);

		data++;
	}
}

void nvram_secondary_write(sec_nvram_offset offset, void* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		iic_write_byte(SECONDARY_NVRAM_IIC_ADDRESS, offset++, *((uint8_t*)data++));
		wait_us(12000);
	}
}

bool get_box_flag(nv_box_flag_mask flag_mask)
{
	uint8_t nvram_flags_offset = 0x00;
	uint8_t flags = 0x00;

	if(flag_mask & 0xff00)
	{
		nvram_flags_offset = NVSEC_FLAGS_HI;
	}
	else
	{
		nvram_flags_offset = NVSEC_FLAGS_LO;
	}

	nvram_secondary_read(nvram_flags_offset, &flags, 1);

	return (flags & flag_mask);
}

void set_box_flag(nv_box_flag_mask flag_mask, bool enable)
{
	uint8_t nvram_flags_offset = 0x00;
	uint8_t current_flags = 0x00;
	uint8_t new_flags = 0x00;

	if(flag_mask & 0xff00)
	{
		nvram_flags_offset = NVSEC_FLAGS_HI;
	}
	else if(flag_mask & 0xff)
	{
		nvram_flags_offset = NVSEC_FLAGS_LO;
	}
	
	nvram_secondary_read(nvram_flags_offset, &current_flags, 1);

	if(enable)
	{
		new_flags = current_flags | flag_mask;
	}
	else
	{
		new_flags = current_flags & ~flag_mask;
	}

	nvram_secondary_write(nvram_flags_offset, &new_flags, 1);
}

