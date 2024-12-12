
#ifndef __LIBWTV_HID_EVENT_H
#define __LIBWTV_HID_EVENT_H

#include "hid/event/keyboard.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EVENT_NULL            0x0000

#define EVENT_IR_KEYBOARD     0x0001
#define EVENT_IR_REMOTE       0x0002
#define EVENT_IR              (EVENT_IR_KEYBOARD | EVENT_IR_REMOTE)

#define EVENT_PS2_KEYBOARD    0x0004
#define EVENT_PS2_MOUSE       0x0008
#define EVENT_PS2             (EVENT_PS2_KEYBOARD | EVENT_PS2_MOUSE)

#define EVENT_KEYBOARD        (EVENT_IR_KEYBOARD | EVENT_PS2_KEYBOARD)
#define EVENT_BUTTON          (EVENT_KEYBOARD | EVENT_IR_REMOTE)

#define EVENT_MAX_KEY_MEMORY 8

typedef struct {
	uint16_t source;
	bool is_modifier;
	int16_t data;
	uint32_t time;
} hid_event;

typedef struct {
	uint32_t modifiers;
	uint8_t keys_pressed_count;
	hid_event keys_pressed[EVENT_MAX_KEY_MEMORY];
} hid_state;

void hid_events_enable(uint32_t enable_events);
void hid_events_enable_all();
hid_state hid_current_state();
hid_event dequeue_hid_event();
bool process_hid_buffers();

#ifdef __cplusplus
}
#endif

#endif