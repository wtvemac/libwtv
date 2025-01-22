#ifndef __LIBWTV_STORAGE_FLASH_H
#define __LIBWTV_STORAGE_FLASH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	USE_APPROM     = 0x00,
	USE_ALT_APPROM = 0x01,
	USE_BOOTROM    = 0x02,
} selected_flash_base;

void flash_init();
void flash_close();
uint32_t flash_get_identity(selected_flash_base base);

#ifdef __cplusplus
}
#endif

#endif
