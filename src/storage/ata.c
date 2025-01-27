#include "storage/ata.h"
#include "storage/nvram.h"
#include "storage/ide.h"
#include "ssid.h"
#include "libc.h"
#include "../regs_internal.h"

typedef struct {
	bool ata_enabled;
	uint8_t selected_drive;
	ata_identity_t drive_identity;
} ata_context_t;

static ata_context_t ata_context;

void ata_init()
{
	ide_command_block_t command_block;
	
	ide_init();

	if(ide_probe(IDE_DEFAULT_TIMEOUT))
	{
		ata_context.selected_drive = IDE_DRIVE0;

		if(!ata_request_identity(&ata_context.drive_identity))
		{
			ata_context.selected_drive = IDE_DRIVE1;

			if(!ata_request_identity(&ata_context.drive_identity))
			{
				ata_context.selected_drive = IDE_DRIVE0;
				ata_context.ata_enabled = false;
			}
			else
			{
				ata_context.ata_enabled = true;
			}
		}
		else
		{
			ata_context.ata_enabled = true;
		}
	}
	else
	{
		ata_context.ata_enabled = false;
	}
}

void ata_close()
{
	//
}

bool ata_enabled()
{
	return ata_context.ata_enabled;
}

ata_identity_t ata_get_identity()
{
	return ata_context.drive_identity;
}

bool ata_read_data(uint64_t data_offset, void* data, uint32_t data_length)
{
	ide_command_block_t command_block;

	ide_setup_command(
		&command_block,
		ata_context.selected_drive,
		0x00,
		ATA_CMD_READ_MULTIPLE,
		0,
		data_offset,
		data,
		data_length
	);

	bool result = ide_handle_command(IDE_PROTO_SYNC_PIO_IN, &command_block);

	return result;
}

bool ata_write_data(uint64_t data_offset, void* data, uint32_t data_length)
{
	ide_command_block_t command_block;

	uint32_t data_offset_adjusted = data_offset;
	void* data_adjusted = data;
	uint32_t data_length_adjusted = data_length;

	uint32_t left_underflow = (data_offset & IDE_SECTOR_OF_MASK);
	uint32_t right_underflow = (data_length + left_underflow) & IDE_SECTOR_OF_MASK;

	// If the data isn't aligned to sector boundries then we need to make it align by pulling the missing data.
	if(left_underflow > 0 || right_underflow > 0)
	{
		uint32_t _left_rel_offset = left_underflow;

		data_offset_adjusted = data_offset & ~IDE_SECTOR_OF_MASK;
		// align up to nearest sector
		data_length_adjusted = (data_length + left_underflow + right_underflow + IDE_SECTOR_OF_MASK) & ~IDE_SECTOR_OF_MASK;
		data_adjusted = malloc(data_length_adjusted);

		if(left_underflow > 0)
		{
			ata_read_data(data_offset_adjusted, data_adjusted, IDE_SECTOR_LENGTH);
		}

		if(right_underflow > 0 && (data_length_adjusted > IDE_SECTOR_LENGTH || left_underflow == 0))
		{
			uint32_t right_rel_offset = (data_length_adjusted - IDE_SECTOR_LENGTH);
			uint32_t right_abs_offset = data_offset_adjusted + (data_length_adjusted - IDE_SECTOR_LENGTH);

			ata_read_data(right_abs_offset, ((uint8_t*)(data_adjusted + right_rel_offset)), IDE_SECTOR_LENGTH);
		}

		memcpy(((uint8_t*)(data_adjusted + _left_rel_offset)), data, data_length);
	}

	ide_setup_command(
		&command_block,
		ata_context.selected_drive,
		0x00,
		ATA_CMD_WRITE_MULTIPLE,
		0,
		data_offset_adjusted,
		data_adjusted,
		data_length_adjusted
	);

	bool result = ide_handle_command(IDE_PROTO_SYNC_PIO_OUT, &command_block);

	// If we expanded the input data then we need to free that data before we return.
	if(data_adjusted != data)
	{
		free(data_adjusted);
	}

	return result;
}

bool ata_request_identity(ata_identity_t* identity)
{
	ide_command_block_t command_block;

	ide_setup_command(
		&command_block,
		ata_context.selected_drive,
		0x00,
		ATA_CMD_IDENTIFY_DEVICE,
		0,
		0x00,
		(void*)identity,
		sizeof(ata_identity_t)
	);

	if(ide_handle_command(IDE_PROTO_SYNC_PIO_IN, &command_block))
	{
		return true;
	}

	return false;
}

uint32_t __ata_get_userpwd_pdes(uint32_t* input_hi, uint32_t* input_lo)
{
	uint32_t xor_group1[] = {
		0xbaa96887,
		0x1e17d32c,
		0x03bcdc3c,
		0x0f33d1b2
	};
	uint32_t xor_group2[] = {
		0x4b0f3b58,
		0xe874f0c3,
		0x6955c5a6,
		0x55a7ca46
	};

	for (int t4 = 0; t4 < 4; t4++)
	{
		uint32_t a1 = ((*input_lo ^ xor_group1[t4]) & 0xFFFF);
		uint32_t a2 = (((*input_lo ^ xor_group1[t4]) >> 16) & 0xFFFF);

		uint32_t b = ((a1 * a1) + ~(a2 * a2));

		uint32_t _input_hi = *input_hi;
		*input_hi = *input_lo;
		*input_lo = _input_hi ^ ((((b << 16) | (b >> 16)) ^ xor_group2[t4]) + (a1 * a2));
	}

	return *input_lo;
}

uint8_t* get_ata_userpwd()
{
	static uint8_t ata_pwd[ATA_PWD_LEN];

	ata_pwd_random_t pwd_random = ata_get_userpwd_random();

	uint32_t xor_hi = (pwd_random.hi ^ AG(ssid_hi));
	uint32_t xor_lo = (pwd_random.lo ^ AG(ssid_lo));

	for (int i = 0; i < 8; i++)
	{
		uint32_t random = __ata_get_userpwd_pdes(&xor_hi, &xor_lo);

		*((uint32_t*)(ata_pwd + (i * 4))) = random;
	}

	return ata_pwd;
}

char* ata_get_userpwd_string()
{
	static char ata_pwd_string[(ATA_PWD_LEN * 2) + 1];

	uint8_t* ata_pwd = get_ata_userpwd();

	for(int i = 0; i < (ATA_PWD_LEN * 2); i += 2)
	{
		sprintf((char*)(ata_pwd_string + i), "%02x", *(ata_pwd + (i / 2)));
	}

	ata_pwd_string[(ATA_PWD_LEN * 2)] = 0;

	return ata_pwd_string;
}


ata_pwd_random_t ata_get_userpwd_random()
{
	static ata_pwd_random_t pwd_random;

	nvram_secondary_read(SECONDARY_NVRAM_ATAPWD_RANDOM_OFFSET, (uint8_t*)&pwd_random, sizeof(pwd_random));

	return pwd_random;
}

char* ata_get_userpwd_random_string()
{
	static char pwd_random_string[(ATA_PWD_RAND_LEN * 2) + 1];

	ata_pwd_random_t pwd_random = ata_get_userpwd_random();

	sprintf(pwd_random_string, "%08x%08x", pwd_random.hi, pwd_random.lo);

	pwd_random_string[(ATA_PWD_RAND_LEN * 2)] = 0;

	return pwd_random_string;
}

uint8_t* ata_get_masterpwd()
{
	static uint8_t ata_pwd[(ATA_PWD_LEN * 2) + 1];

	memset(ata_pwd, 0x00, sizeof(ata_pwd));

	memcpy(ata_pwd, ATA_MASTERPWD, sizeof(ATA_MASTERPWD));
	
	ata_pwd[(ATA_PWD_LEN * 2)] = 0;

	return ata_pwd;
}