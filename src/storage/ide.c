#include "storage/ide.h"
#include "wtvsys.h"
#include "../utils.h"

// SOLO RIO device slots 1 and 2
#define IDE_SOLO_PRI_BASE ((ide_device_registers_t*)(0xbe400000))
// SOLO RIO device slots 5 and 6
#define IDE_SOLO_SEC_BASE ((ide_device_registers_t*)(0xbd400000))

// HAN RIO device slots 1 and 2
#define IDE_HAN_PRI_BASE   ((ide_device_registers_t*)(0x10000100))
// HAN RIO device slots 5 and 6
#define IDE_HAN_SEC_BASE   ((ide_device_registers_t*)(0x10000000)) // this isn't known.

static volatile ide_device_registers_t* ide_base;

void ide_init()
{
	ide_base = IDE_SOLO_PRI_BASE;
}

bool ide_probe(uint32_t timeout)
{
	if(timeout == 0)
	{
		timeout = IDE_DEFAULT_TIMEOUT;
	}

	// The WebTV OS sends "Joe" and B and checks for B.
	
	// We're sending "wtv" and a number.

	uint64_t start = get_ticks_ms();
	uint32_t ide_probe_number = (start & 0xff);

	while((get_ticks_ms() - start) < timeout)
	{
		// Any drive that doesn't support LBA will be ignored.
		if(ide_base->disk_select.using_lba)
		{
			__atomic_store_n(&ide_base->sector_count,  0x77,              __ATOMIC_RELAXED); // w
			__atomic_store_n(&ide_base->sector_number, 0x74,              __ATOMIC_RELAXED); // t
			__atomic_store_n(&ide_base->cylinder_low,  0x76,              __ATOMIC_RELAXED); // v
			__atomic_store(  &ide_base->cylinder_high, &ide_probe_number, __ATOMIC_RELAXED);

			if((__atomic_load_n(&ide_base->cylinder_high, __ATOMIC_RELAXED) & 0xff) == ide_probe_number)
			{
				return true;
			}
		}
	}

	return false;
}

bool ide_wait_for_status(uint8_t status_mask, uint8_t status_match, uint32_t timeout)
{
	if(timeout == 0)
	{
		timeout = IDE_DEFAULT_TIMEOUT;
	}

	uint64_t start = get_ticks_ms();

	bool status_good = false;

	while(!status_good && ((get_ticks_ms() - start) < timeout))
	{
		uint8_t current_status = (__atomic_load_n(&ide_base->status_or_command, __ATOMIC_RELAXED) & 0xff);

		status_good = ((current_status & status_mask) == status_match);
	}

	return status_good;
}

bool ide_check_for_status(uint8_t status_mask, uint8_t status_match)
{
	uint8_t current_status = (__atomic_load_n(&ide_base->status_or_command, __ATOMIC_RELAXED) & 0xff);

	return ((current_status & status_mask) == status_match);
}

bool ide_read_data16(uint16_t data_skip, void* in_data, uint32_t data_length, uint8_t sector_count)
{
	for(uint32_t sector_idx = 0, read_idx = 0, copy_idx = 0; sector_idx < sector_count; sector_idx++)
	{
		for(uint32_t sector_byte_idx = 0, ide_data = 0; sector_byte_idx < IDE_SECTOR_LENGTH; sector_byte_idx++, read_idx++)
		{
			if((sector_byte_idx & 1) == 0)
			{
				ide_data = __atomic_load_n(&ide_base->data, __ATOMIC_RELAXED);
			}

			if(read_idx >= data_skip && read_idx < data_length)
			{
				if((read_idx & 1) == 0)
				{
					*((uint8_t*)(in_data + copy_idx)) = (ide_data >> 8);
				}
				else
				{
					*((uint8_t*)(in_data + copy_idx)) = (ide_data >> 0);
				}

				copy_idx++;
			}
		}

		if(!ide_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT))
		{
			return false;
		}
	}

	return true;
}

bool ide_write_data16(void* out_data, uint32_t data_length, uint8_t sector_count)
{
	for(uint32_t sector_idx = 0, write_idx = 0; sector_idx < sector_count; sector_idx++)
	{
		for(uint32_t sector_byte_idx = 0; sector_byte_idx < IDE_SECTOR_LENGTH; sector_byte_idx += 2)
		{
			if(write_idx < data_length)
			{
				uint32_t ide_data = *((uint16_t*)(out_data + write_idx));

				__atomic_store(&ide_base->data, &ide_data, __ATOMIC_RELAXED);

				write_idx += 2;
			}
			else
			{
				__atomic_store_n(&ide_base->data, 0x00000000, __ATOMIC_RELAXED);
			}
		}

		if(!ide_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT))
		{
			return false;
		}
	}

	return true;
}

uint8_t ide_send_command(ide_command_block_t* command_block)
{
	__atomic_store(&ide_base->error_or_feature, &command_block->feature, __ATOMIC_RELAXED);
	__atomic_store(&ide_base->sector_number, &command_block->sector_number, __ATOMIC_RELAXED);
	__atomic_store(&ide_base->cylinder_low, &command_block->cylinder_low, __ATOMIC_RELAXED);
	__atomic_store(&ide_base->cylinder_high, &command_block->cylinder_high, __ATOMIC_RELAXED);
	__atomic_store(&ide_base->disk_select, &command_block->disk_select, __ATOMIC_RELAXED);
	__atomic_store(&ide_base->altstatus_or_devcontrol, &command_block->device_control, __ATOMIC_RELAXED);
	__atomic_store(&ide_base->sector_count, &command_block->sector_count, __ATOMIC_RELAXED);
	
	uint8_t current_status = (ide_base->status_or_command & 0xff);

	__atomic_store(&ide_base->status_or_command, &command_block->command, __ATOMIC_RELAXED);

	return current_status;
}

void ide_setup_command(ide_command_block_t* command_block, uint8_t selected_drive, uint8_t device_control, uint8_t command, uint8_t feature, uint64_t data_offset, void* data, uint32_t data_length)
{
	command_block->disk_select.always1_a = 1;
	command_block->disk_select.always1_b = 1;
	command_block->disk_select.using_lba = 1;

	command_block->disk_select.selected_drive = selected_drive & 0x01;
	command_block->device_control = (uint32_t)(0x08 | (device_control & 0x06));
	command_block->command = (uint32_t)command;
	command_block->feature = (uint32_t)feature;

	uint32_t lba_address = data_offset >> IDE_SECTOR_OF_BSHIFT;
	uint16_t data_skip = data_offset & IDE_SECTOR_OF_MASK;

	command_block->sector_number = lba_address & 0xff;
	command_block->cylinder_low = (lba_address >> 8) & 0xff;
	command_block->cylinder_high = (lba_address >> 16) & 0xff;
	command_block->disk_select.address = (lba_address >> 24) & 0x0f;

	command_block->data_skip = data_skip;
	command_block->data = data;
	command_block->data_length = data_length + data_skip;

	if(command_block->data_length > 0)
	{
		uint32_t sector_count = MIN(command_block->data_length >> IDE_SECTOR_OF_BSHIFT, 0xff);
		if((command_block->data_length & IDE_SECTOR_OF_MASK) != 0)
		{
			sector_count++;
		}

		command_block->sector_count = sector_count;
	}
	else
	{
		command_block->sector_count = 0;
	}
}

bool ide_handle_simple_command(bool async, ide_command_block_t* command_block)
{
	ide_send_command(command_block);

	return ide_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT);
}

bool ide_handle_pio_in_command(bool async, ide_command_block_t* command_block)
{
	if(command_block->data_length > 0)
	{
		ide_send_command(command_block);

		if(ide_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT))
		{
			if(ide_wait_for_status(IDE_STATUS_DRQ, IDE_STATUS_DRQ, IDE_DEFAULT_TIMEOUT))
			{
				return ide_read_data16(command_block->data_skip, command_block->data, command_block->data_length, command_block->sector_count);
			}
			else
			{
				// Drive request ready not set. 
				return false;
			}

		}
		else
		{
			// Timed out waiting for drive to become not busy.
			return false;
		}
	}
	else
	{
		// Can't capture data.
		return false;
	}
}

bool ide_handle_pio_out_command(bool async, ide_command_block_t* command_block)
{
	if(command_block->data_length > 0)
	{
		ide_send_command(command_block);

		if(ide_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT))
		{
			return ide_write_data16(command_block->data, command_block->data_length, command_block->sector_count);
		}
		else
		{
			// Timed out waiting for drive to become not busy.
			return false;
		}
	}
	else
	{
		// Can't capture data.
		return false;
	}
}

bool ide_handle_command(ide_proto protocol, ide_command_block_t* command_block)
{
	if(ide_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT))
	{
		if(ide_wait_for_status(IDE_STATUS_DRDY, IDE_STATUS_DRDY, IDE_DEFAULT_TIMEOUT))
		{
			switch(protocol)
			{
				case IDE_PROTO_NO_DATA:
					return ide_handle_simple_command(false, command_block);
					
				case IDE_PROTO_SYNC_PIO_IN:
					return ide_handle_pio_in_command(false, command_block);
					
				case IDE_PROTO_PIO_IN:
					return ide_handle_pio_in_command(true, command_block);

				case IDE_PROTO_SYNC_PIO_OUT:
					return ide_handle_pio_out_command(false, command_block);
					
				case IDE_PROTO_PIO_OUT:
					return ide_handle_pio_out_command(true, command_block);

				case IDE_PROTO_SYNC_DMA_IN:
					return false;
					
				case IDE_PROTO_DMA_IN:
					return false;
					
				case IDE_PROTO_SYNC_DMA_OUT:
					return false;
					
				case IDE_PROTO_DMA_OUT:
					return false;

				default:
					// Bad protocol
					return false;

			}
		}
		else
		{
			// Timed out waiting for drive to be ready.
			return false;
		}
	}
	else
	{
		// Timed out waiting for drive to become not busy.
		return false;
	}
}