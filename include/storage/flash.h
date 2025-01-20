#ifndef __LIBWTV_STORAGE_FLASH_H
#define __LIBWTV_STORAGE_FLASH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	APPROM_BASE     = 0x00,
	ALT_APPROM_BASE = 0x01,
	BOOTROM_BASE    = 0x02,
	DIAG_BASE       = 0x03,
} flash_base;

void flash_init();
void flash_close();
uint32_t flash_get_identity(flash_base base);

#ifdef __cplusplus
}
#endif

#endif
