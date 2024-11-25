#ifndef __LIBWTV_SERIAL_H
#define __LIBWTV_SERIAL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef char serial_context;

//serial_context init_suc_serial(char port, unsigned long baudRate, char parityBits, char stopBits)
void serial_io_init();

void serial_put_string(char const *msg);

void serial_put_char(char chr);

#ifdef __cplusplus
}
#endif

#endif
