#ifndef __LIBWTV_STORAGE_IDE_H
#define __LIBWTV_STORAGE_IDE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ide_primary_hd_exists(uint8_t drive_address);

#ifdef __cplusplus
}
#endif

#endif


