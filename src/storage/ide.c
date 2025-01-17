#include "storage/ide.h"
#include "./ide_regs_internal.h"
#include "wtvsys.h"

static uint8_t ide_probe_number = 0x01;

bool ide_primary_hd_exists(uint8_t selected_drive, uint32_t timeout)
{
	if(timeout == 0)
	{
		timeout = IDE_DEFAULT_TIMEOUT;
	}

	// The WebTV OS sends "Joe" and B and checks for B.
	
	// We're sending "wtv" and a incremented number, then check for that number.
	// The number is so we can call this multiple times and see the change.

	ide_probe_number++;

	// There can only be two devices on an IDE bus, addressed by 0 or 1.
	selected_drive &= 0x01;

	uint64_t start = get_ticks_ms();

	while((get_ticks_ms() - start) < timeout)
	{
		ide_disk_select disk_select = IDE_PRI(disk_select);

		disk_select.selected_drive = selected_drive;

		IDE_PRI_SET(disk_select, disk_select);

		// Any drive that doesn't support LBA will be ignored.
		if(disk_select.using_lba)
		{
			IDE_PRI_SET(sector_count,  0x77); // w
			IDE_PRI_SET(sector_number, 0x74); // t
			IDE_PRI_SET(cylinder_low,  0x76); // v
			IDE_PRI_SET(cylinder_high, ide_probe_number);

			if((IDE_PRI(cylinder_high) & 0xff) == ide_probe_number)
			{
				return true;
			}
		}
	}

	return false;
}

bool ide_primary_wait_for_status(uint8_t status_mask, uint8_t status_match, uint32_t timeout)
{
	if(timeout == 0)
	{
		timeout = IDE_DEFAULT_TIMEOUT;
	}

	uint64_t start = get_ticks_ms();

	bool status_good = false;

	while(!status_good && ((get_ticks_ms() - start) < timeout))
	{
		uint8_t current_status = (IDE_PRI(status_or_command) & 0xff);

		status_good = ((current_status & status_mask) == status_match);
	}

	return status_good;
}

// Will add more reading and writing routines depending on the disk type.
bool ide_primary_read_data16(uint8_t* in_data, uint32_t data_length)
{
	data_length &= ~0x01;

	if(data_length >= 2)
	{
		for(uint32_t i = 0; i < data_length; i += 2)
		{
			uint32_t ide_data = IDE_PRI(data);

			*((uint16_t*)(in_data + i)) = (ide_data & 0xffff);
		}

		return true;
	}
	else
	{
		// Data length too small.
		return false;
	}
}

void ide_setup_command(ide_command_block* command_block, uint8_t selected_drive, uint8_t device_control, uint8_t command, uint8_t feature, uint32_t lba_address, void* data, uint32_t data_length)
{
	command_block->disk_select.always1_a = 1;
	command_block->disk_select.always1_b = 1;

	command_block->disk_select.selected_drive = selected_drive;
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

uint8_t ide_primary_send_command(ide_command_block command_block)
{
	IDE_PRI_SET(error_or_feature, command_block.feature);
	IDE_PRI_SET(sector_number, command_block.sector_number);
	IDE_PRI_SET(cylinder_low, command_block.cylinder_low);
	IDE_PRI_SET(cylinder_high, command_block.cylinder_high);
	IDE_PRI_SET(disk_select, command_block.disk_select);
	IDE_PRI_SET(altstatus_or_devcontrol, command_block.device_control);

	IDE_PRI_SET(sector_count, ((command_block.data_length >> IDE_SECTOR_BSHIFT) & 0xff));

	uint8_t current_status = (IDE_PRI(status_or_command) & 0xff);

	IDE_PRI_SET(status_or_command, command_block.command);

	return current_status;
}

bool ide_primary_handle_nodata_command(bool async, ide_command_block command_block)
{
	ide_primary_send_command(command_block);

	return ide_primary_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT);
}

bool ide_primary_handle_pioin_command(bool async, ide_command_block command_block)
{
	if(command_block.data_length > 0)
	{
		ide_primary_send_command(command_block);

		if(ide_primary_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT))
		{
			uint8_t current_status = (IDE_PRI(status_or_command) & 0xff);

			if(current_status & IDE_STATUS_DRQ)
			{
				return ide_primary_read_data16(command_block.data, command_block.data_length);
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

bool ide_primary_handle_pioout_command(bool async, ide_command_block command_block)
{
	return false;
}

bool ide_primary_handle_command(ide_proto protocol, ide_command_block command_block)
{
	if(ide_primary_wait_for_status(IDE_STATUS_BSY, IDE_STATUS_NONE, IDE_DEFAULT_TIMEOUT))
	{
		if(ide_primary_wait_for_status(IDE_STATUS_DRDY, IDE_STATUS_DRDY, IDE_DEFAULT_TIMEOUT))
		{
			switch(protocol)
			{
				case IDE_PROTO_NO_DATA:
					return ide_primary_handle_nodata_command(false, command_block);
					
				case IDE_PROTO_PIO_IN:
					return ide_primary_handle_pioin_command(false, command_block);
					
				case IDE_PROTO_ASYNC_PIO_IN:
					return ide_primary_handle_pioin_command(true, command_block);

				case IDE_PROTO_PIO_OUT:
					return ide_primary_handle_pioout_command(false, command_block);
					
				case IDE_PROTO_ASYNC_PIO_OUT:
					return ide_primary_handle_pioout_command(true, command_block);

				case IDE_PROTO_DMA_IN:
					return false;
					
				case IDE_PROTO_ASYNC_DMA_IN:
					return false;
					
				case IDE_PROTO_DMA_OUT:
					return false;
					
				case IDE_PROTO_ASYNC_DMA_OUT:
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