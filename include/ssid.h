

#ifndef __LIBWTV_SSID_H
#define __LIBWTV_SSID_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct __attribute__((packed)) ssid_s
{
	struct __attribute__((packed)) {
		uint8_t box_type;
		uint8_t box_random[3];
	} hi;

	struct __attribute__((packed)) {
		uint8_t manufacturer_id[3];
		uint8_t crc8;
	} lo;
} ssid_t;


ssid_t get_ssid();
uint8_t* get_ssid_bytes();
char* get_ssid_string();

#ifdef __cplusplus
}
#endif

#endif
