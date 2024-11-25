#include "ssid.h"
#include "libc.h"
#include "regs_internal.h"

ssid_t get_ssid()
{
	static ssid_t ssid;

	*((uint32_t*)&ssid.hi) = AG(ssid_hi);
	*((uint32_t*)&ssid.lo) = AG(ssid_lo);

	return ssid;
}

uint8_t* get_ssid_bytes()
{
	static uint8_t ssid[8];

	*((uint32_t*)(ssid + 0)) = (uint32_t)AG(ssid_hi);
	*((uint32_t*)(ssid + 4)) = (uint32_t)AG(ssid_lo);

	return ssid;
}

char* get_ssid_string()
{
	static char ssid_string[17];

	sprintf(ssid_string, "%08x%08x", (uint32_t)AG(ssid_hi), (uint32_t)AG(ssid_lo));

	ssid_string[16] = 0;

	return ssid_string;
}
