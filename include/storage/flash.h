#ifndef __LIBWTV_STORAGE_FLASH_H
#define __LIBWTV_STORAGE_FLASH_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void flash_init();
void flash_close();
uint32_t flash_get_identity();

#ifdef __cplusplus
}
#endif

#endif
