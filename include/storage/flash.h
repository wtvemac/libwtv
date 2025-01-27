#ifndef __LIBWTV_STORAGE_FLASH_H
#define __LIBWTV_STORAGE_FLASH_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	APPROM_FLASH     = 0x00,
	ALT_APPROM_FLASH = 0x01,
	BOOTROM_FLASH    = 0x02,
} flash_image_type;

// Much of this list is from MAME's intelfsh.cpp
// You can find the official list @ JEDEC's Standard Manufacturer's Identification Code JEP106-K
// This most likey will be reduced or expanded as I go through the list to see whcih one actually can work on the boxes.
typedef enum : uint16_t
{
	FLASH_MFG_AMD         = 0x0001,
	FLASH_MFG_FUJITSU     = 0x0004,
	FLASH_MFG_MITSUBISHI  = 0x001c,
	FLASH_MFG_ATMEL       = 0x001f,
	FLASH_MFG_ST          = 0x0020, // Formerly SGS and Thomson
	FLASH_MFG_CATALYST    = 0x0031,
	FLASH_MFG_PANASONIC   = 0x0032,
	FLASH_MFG_ALLIANCE    = 0x0052,
	FLASH_MFG_SANYO       = 0x0062,
	FLASH_MFG_INTEL       = 0x0089,
	FLASH_MFG_TI          = 0x0097,
	FLASH_MFG_TOSHIBA     = 0x0098,
	FLASH_MFG_HYUNDAI     = 0x00ad,
	FLASH_MFG_SHARP       = 0x00b0,
	FLASH_MFG_MACRONIX    = 0x00c2,
	FLASH_MFG_APPLE       = 0x00c8, // GigaDevice Semiconductor
	FLASH_MFG_WINBOND     = 0x00da,
	FLASH_MFG_NEXCOM      = 0x00ef
} flash_manufacture_id;


// These are official suported ICs from WebTV. There's no reason this can't be expanded in libwtv.
// As long as there's pin-compatability with the boxs (preferably using the footprint on the board).
typedef enum : uint16_t
{
	// AMD (manufacture_id=0x0001)
	AM29F400AB = 0x22ab, // AMD 16-bit 5v 4Mbit boot bottom sector (512kB, 2x for 1MB on the box)
	AM29F400AT = 0x2223, // AMD 16-bit 5v 4Mbit boot top sector (512kB, 2x for 1MB on the box)
	AM29F800BB = 0x2258, // AMD 16-bit 5v 8Mbit boot bottom sector (1MB, 2x for 2MB on the box)
	AM29F800BT = 0x22d6, // AMD 16-bit 5v 8Mbit boot top sector (1MB, 2x for 2MB on the box)
	// Fujitsu (manufacture_id=0x0004)
	MBM29F400B = 0x22ab, // Fijitsu 16-bit 5v 4Mbit boot bottom sector (512kB, 2x for 1MB on the box)
	MBM29F400T = 0x2223, // Fijitsu 16-bit 5v 4Mbit boot top sector (512kB, 2x for 1MB on the box)
	MBM29F800B = 0x2258, // Fijitsu 16-bit 5v 8Mbit boot bottom sector (1MB, 2x for 2MB on the box)
	MBM29F800T = 0x22d6, // Fijitsu 16-bit 5v 8Mbit boot top sector (1MB, 2x for 2MB on the box)
	// Macronix (manufacture_id=0x00c2)
	MX29F1610  = 0x00f1, // Macronix 16-bit 5v 16Mbit (2MB, 2x for 4MB on the box)
} flash_device_id;

typedef struct __attribute__((__packed__))
{
	union
	{
		struct
		{
			uint32_t id_data;
		};
		struct
		{
			flash_manufacture_id manufacture_id;
			flash_device_id device_id;
		};
	};
	bool can_write;
} flash_identity_t;

void flash_init(flash_image_type image_type);
void flash_close();
bool flash_enabled();
flash_identity_t flash_get_identity();
const char* flash_get_manufacture_name();
const char* flash_get_device_name();
bool flash_read_data(uint64_t data_offset, void* data, uint32_t data_length);
bool flash_write_data(uint64_t data_offset, void* data, uint32_t data_length);

#ifdef __cplusplus
}
#endif

#endif
