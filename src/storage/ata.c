#include "storage/ata.h"
#include "storage/nvram.h"
#include "ssid.h"
#include "libc.h"
#include "../regs_internal.h"

uint32_t __get_ata_userpwd_pdes_random(uint32_t* input_hi, uint32_t* input_lo)
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
	static uint8_t ata_pwd[32];

	atapwd_random_t atapwd_random = get_atapwd_random();

	uint32_t xor_hi = (atapwd_random.hi ^ AG(ssid_hi));
	uint32_t xor_lo = (atapwd_random.lo ^ AG(ssid_lo));

	for (int i = 0; i < 8; i++)
	{
		uint32_t random = __get_ata_userpwd_pdes_random(&xor_hi, &xor_lo);

		*((uint32_t*)(ata_pwd + (i * 4))) = random;
	}

	return ata_pwd;
}

uint8_t* get_ata_masterpwd()
{
	static uint8_t ata_pwd[32];

	memset(ata_pwd, 0, sizeof(ata_pwd));

	memcpy(ata_pwd, ATA_MASTERPWD, sizeof(ATA_MASTERPWD));

	return ata_pwd;
}

char* get_ata_pwd_string(const uint8_t ata_pwd[32])
{
	static char ata_pwd_string[65];

	for(int i = 0; i < 64; i += 2)
	{
		sprintf((char*)(ata_pwd_string + i), "%02x", *(ata_pwd + (i / 2)));
	}

	ata_pwd_string[64] = 0;

	return ata_pwd_string;
}


atapwd_random_t get_atapwd_random()
{
	static atapwd_random_t atapwd_random;

	nvram_secondary_read(SECONDARY_NVRAM_ATAPWD_RANDOM_OFFSET, (uint8_t*)&atapwd_random, sizeof(atapwd_random));

	return atapwd_random;
}

char* get_atapwd_random_string()
{
	static char atapwd_random_string[17];

	atapwd_random_t atapwd_random = get_atapwd_random();

	sprintf(atapwd_random_string, "%08x%08x", atapwd_random.hi, atapwd_random.lo);

	atapwd_random_string[16] = 0;

	return atapwd_random_string;
}