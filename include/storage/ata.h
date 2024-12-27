#ifndef __LIBWTV_STORAGE_ATA_H
#define __LIBWTV_STORAGE_ATA_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ATA_MASTERPWD "WebTV Networks Inc."

typedef struct __attribute__((packed)) atapwd_random_s
{
	uint32_t hi;
	uint32_t lo;
} atapwd_random_t;

uint8_t* get_ata_userpwd();
uint8_t* get_ata_masterpwd();

char* get_ata_pwd_string(const uint8_t ata_pwd[32]);

atapwd_random_t get_atapwd_random();
char* get_atapwd_random_string();

#ifdef __cplusplus
}
#endif

#endif
