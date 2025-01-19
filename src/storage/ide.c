#include "storage/ide.h"
#include "wtvsys.h"

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
	uint8_t ide_probe_number = (start & 0xff);

	while((get_ticks_ms() - start) < timeout)
	{
		// Any drive that doesn't support LBA will be ignored.
		if(ide_base->disk_select.using_lba)
		{
			ide_base->sector_count  = 0x77; // w
			ide_base->sector_number = 0x74; // t
			ide_base->cylinder_low  = 0x76; // v
			ide_base->cylinder_high = ide_probe_number;

			if((ide_base->cylinder_high & 0xff) == ide_probe_number)
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
		uint8_t current_status = (ide_base->status_or_command & 0xff);

		status_good = ((current_status & status_mask) == status_match);
	}

	return status_good;
}

// Will add more reading and writing routines depending on the disk type.
bool ide_read_data16(void* in_data, uint32_t data_length)
{
	data_length &= ~0x01;

	if(data_length >= 2)
	{
		for(uint32_t i = 0; i < data_length; i += 2)
		{
			uint32_t ide_data = *((uint32_t*)ide_base);

			*((uint8_t*)(in_data + (i + 0))) = ((ide_data >> 0x08) & 0xff);
			*((uint8_t*)(in_data + (i + 1))) = ((ide_data >> 0x00) & 0xff);
		}

		return true;
	}
	else
	{
		// Data length too small.
		return false;
	}
}

uint8_t ide_send_command(ide_command_block_t* command_block)
{
	ide_base->error_or_feature = command_block->feature;
	ide_base->sector_number = command_block->sector_number;
	ide_base->cylinder_low = command_block->cylinder_low;
	ide_base->cylinder_high = command_block->cylinder_high;
	ide_base->disk_select = command_block->disk_select;
	ide_base->altstatus_or_devcontrol = command_block->device_control;
	ide_base->sector_count = ((command_block->data_length >> IDE_SECTOR_BSHIFT) & 0xff);

	uint8_t current_status = (ide_base->status_or_command & 0xff);

	ide_base->status_or_command = command_block->command;

	return current_status;
}

void ide_setup_command(ide_command_block_t* command_block, uint8_t selected_drive, uint8_t device_control, uint8_t command, uint8_t feature, uint32_t lba_address, void* data, uint32_t data_length)
{
	command_block->disk_select.always1_a = 1;
	command_block->disk_select.always1_b = 1;

	command_block->disk_select.selected_drive = selected_drive & 0x01;
	command_block->device_control = (uint32_t)(0x08 | (device_control & 0x06));
	command_block->command = (uint32_t)command;
	command_block->feature = (uint32_t)feature;

	command_block->sector_number = lba_address & 0xff;
	command_block->cylinder_low = (lba_address >> 8) & 0xff;
	command_block->cylinder_high = (lba_address >> 16) & 0xff;
	command_block->disk_select.address = (lba_address >> 24) & 0x0f;

	command_block->data = data;
	command_block->data_length = data_length;
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
			uint8_t current_status = (ide_base->status_or_command & 0xff);

			if(current_status & IDE_STATUS_DRQ)
			{
				return ide_read_data16(command_block->data, (command_block->data_length >> 1));
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
	return false;
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