#include "storage/nvram.h"
#include "iic.h"
#include "wtvsys.h"

void nvram_secondary_read(uint32_t offset, uint8_t* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		*data = iic_read_byte(SECONDARY_NVRAM_IIC_ADDRESS, offset++);

		data++;
	}
}

void nvram_secondary_write(uint32_t offset, uint8_t* data, uint32_t length)
{
	for(uint32_t i = 0; i < length; i++)
	{
		iic_write_byte(SECONDARY_NVRAM_IIC_ADDRESS, offset++, *data++);
		wait_us(12000);
	}
}

bool get_box_flag(uint16_t flag_mask)
{
	uint8_t nvram_flags_offset = 0x00;
	uint8_t flags = 0x00;

	if(flag_mask & 0xff00)
	{
		nvram_flags_offset = SECONDARY_NVRAM_FLAGS1_OFFSET;
	}
	else
	{
		nvram_flags_offset = SECONDARY_NVRAM_FLAGS0_OFFSET;
	}

	nvram_secondary_read(nvram_flags_offset, &flags, 1);

	return (flags & flag_mask);
}

void set_box_flag(uint16_t flag_mask, bool enable)
{
	uint8_t nvram_flags_offset = 0x00;
	uint8_t current_flags = 0x00;
	uint8_t new_flags = 0x00;

	if(flag_mask & 0xff00)
	{
		nvram_flags_offset = SECONDARY_NVRAM_FLAGS1_OFFSET;
	}
	else if(flag_mask & 0xff)
	{
		nvram_flags_offset = SECONDARY_NVRAM_FLAGS0_OFFSET;
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

