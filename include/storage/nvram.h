#ifndef __LIBWTV_STORAGE_NVRAM_H
#define __LIBWTV_STORAGE_NVRAM_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SECONDARY_NVRAM_IIC_ADDRESS 0xA0

#define SECONDARY_NVRAM_FLAGS0_OFFSET 0x22
#define SECONDARY_NVRAM_FLAGS1_OFFSET 0x2d
#define SECONDARY_NVRAM_ATAPWD_RANDOM_OFFSET 0x36

#define BOX_FLAG_BOOTROM_BOOT 0x0c


void nvram_secondary_read(uint32_t offset, uint8_t* data, uint32_t length);
void nvram_secondary_write(uint32_t offset, uint8_t* data, uint32_t length);

void set_box_flag(uint16_t flag_mask, bool enable);

#ifdef __cplusplus
}
#endif

#endif
