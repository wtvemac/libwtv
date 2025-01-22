#ifndef __LIBWTV_STORAGE_FLASH_H
#define __LIBWTV_STORAGE_FLASH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	USE_APPROM_FLASH     = 0x00,
	USE_ALT_APPROM_FLASH = 0x01,
	USE_BOOTROM_FLASH    = 0x02,
} selected_flash_base;

typedef enum : uint16_t
{
	FLASH_MFG_AMD         = 0x0001,
	FLASH_MFG_SPANSION    = 0x0001,
	FLASH_MFG_TI_OLD      = 0x0001,
	FLASH_MFG_FUJITSU     = 0x0004,
	FLASH_MFG_EON         = 0x001c,
	FLASH_MFG_ATMEL       = 0x001f,
	FLASH_MFG_ST          = 0x0020,
	FLASH_MFG_CATALYST    = 0x0031,
	FLASH_MFG_PANASONIC   = 0x0032,
	FLASH_MFG_AMIC        = 0x0037,
	FLASH_MFG_SYNCMOS     = 0x0040,
	FLASH_MFG_ALLIANCE    = 0x0052,
	FLASH_MFG_SANYO       = 0x0062,
	FLASH_MFG_INTEL       = 0x0089,
	FLASH_MFG_TI          = 0x0097,
	FLASH_MFG_TOSHIBA     = 0x0098,
	FLASH_MFG_PMC         = 0x009d,
	FLASH_MFG_BRIGHT      = 0x00ad,
	FLASH_MFG_HYUNDAI     = 0x00ad,
	FLASH_MFG_SHARP       = 0x00b0,
	FLASH_MFG_SST         = 0x00bf,
	FLASH_MFG_MACRONIX    = 0x00c2,
	FLASH_MFG_GIGADEVICE  = 0x00c8,
	FLASH_MFG_ISSI        = 0x00d5,
	FLASH_MFG_WINBOND     = 0x00da,
	FLASH_MFG_WINBOND_NEX = 0x00ef
} flash_manufacture_id;

typedef enum : uint16_t
{
	// AMD (manufacture_id=0x0001)
	AM29F400AB = 0x22ab, // AMD 16-bit 5v 4Mbit bottom boot sector (512kB, 2x for 1MB on the box)
	AM29F400AT = 0x2223, // AMD 16-bit 5v 4Mbit top boot sector (512kB, 2x for 1MB on the box)
	AM29F800BB = 0x2258, // AMD 16-bit 5v 8Mbit bottom boot sector (1MB, 2x for 2MB on the box)
	AM29F800BT = 0x22d6, // AMD 16-bit 5v 8Mbit top boot sector (1MB, 2x for 2MB on the box)
	// Fujitsu (manufacture_id=0x0004)
	MBM29F400B = 0x22ab, // Fijitsu 16-bit 5v 4Mbit bottom boot sector (512kB, 2x for 1MB on the box)
	MBM29F400T = 0x2223, // Fijitsu 16-bit 5v 4Mbit top boot sector (512kB, 2x for 1MB on the box)
	MBM29F800B = 0x2258, // Fijitsu 16-bit 5v 8Mbit bottom boot sector (1MB, 2x for 2MB on the box)
	MBM29F800T = 0x22d6, // Fijitsu 16-bit 5v 8Mbit top boot sector (1MB, 2x for 2MB on the box)
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
} flash_identity_t;

void flash_init();
void flash_close();
flash_identity_t flash_get_identity(selected_flash_base base);

#ifdef __cplusplus
}
#endif

#endif
