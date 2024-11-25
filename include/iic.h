#ifndef __LIBWTV_IIC_H
#define __LIBWTV_IIC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void iic_init();
uint8_t iic_read_byte(uint8_t node_address, uint8_t data_offset);
void iic_write_byte(uint8_t node_address, uint8_t data_offset, uint8_t data);
void iic_close();

#ifdef __cplusplus
}
#endif

#endif
