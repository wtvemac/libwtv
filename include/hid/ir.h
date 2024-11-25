#ifndef __LIBWTV_HID_IR_H
#define __LIBWTV_HID_IR_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool ir_init();
bool ir_close();
uint32_t dequeue_ir_buffer();
void __solo_ir_input_callback();

#ifdef __cplusplus
}
#endif

#endif
