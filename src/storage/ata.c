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

bool ata_request_identity(ata_identity_t* identity)
{
	ide_command_block_t command_block;

	ide_setup_command(
		&command_block,             // IDE parameters to set
		ata_context.selected_drive, // Selected drive on the IDE bus to work on.
		0x00,                       // IDE device control byte
		ATA_CMD_IDENTIFY_DEVICE,    // IDE command byte
		0,                          // IDE feature byte
		0x00,                       // LBA address
		(void*)identity,            // Data to store or send
		sizeof(ata_identity_t)      // Length of the data to store or send
	);

	if(ide_handle_command(IDE_PROTO_PIO_IN, &command_block))
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