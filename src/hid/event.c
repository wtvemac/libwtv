#include "hid/event.h"
#include "libc.h"

#define HID_EVENT_BUFFER_SIZE       16 // needs to be a multiple of 2.
#define HID_EVENT_BUFFER_INDEX_MASK (HID_EVENT_BUFFER_SIZE - 1)

static uint32_t _enabled_events   = 0x00;

static hid_event  hid_event_buffer[HID_EVENT_BUFFER_SIZE];
static uint8_t  hid_event_buffer_head = 0;
static uint8_t  hid_event_buffer_tail = 0;

void hid_events_enable(uint32_t enable_events)
{
	uint32_t newly_enabled_events = _enabled_events ^ (_enabled_events | enable_events);

	if(newly_enabled_events & EVENT_IR) {
		ir_init();
	}

	if(newly_enabled_events & EVENT_PS2) {
		ps2_init();
	}

	_enabled_events |= enable_events;
}

void hid_events_enable_all()
{
	hid_events_enable(EVENT_IR | EVENT_PS2);
}

void enqueue_hid_event(uint16_t source, uint16_t data, uint32_t time)
{
	if(source != EVENT_NULL) {
		if((hid_event_buffer_head + 1) != hid_event_buffer_tail)
		{
			hid_event_buffer_head = (hid_event_buffer_head + 1) & HID_EVENT_BUFFER_INDEX_MASK;

			hid_event_buffer[hid_event_buffer_head].source = source;
			hid_event_buffer[hid_event_buffer_head].data = data;
			hid_event_buffer[hid_event_buffer_head].time = time;
		}

		if(source & EVENT_BUTTON) {
		}
	}
}

hid_event dequeue_hid_event()
{
	if(hid_event_buffer_tail != hid_event_buffer_head)
	{
		hid_event_buffer_tail = (hid_event_buffer_tail + 1) & HID_EVENT_BUFFER_INDEX_MASK;

		return hid_event_buffer[hid_event_buffer_tail];
	} else {
		hid_event null_event = {
			.source = EVENT_NULL,
			.data = 0x00,
			.time = 0x00
		};

		return null_event;
	}
}

bool process_hid_buffers()
{
	bool has_processed_buffers = false;

	if(_enabled_events & EVENT_IR) {
		uint32_t ir_data = dequeue_ir_buffer();

		if(ir_data != IR_NOK) {
			enqueue_hid_event(EVENT_IR_KEYBOARD, ir_data, 0);
			
			has_processed_buffers = true;
			printf("\tir_data=%08x\n", ir_data);
		}
	}

	if(_enabled_events & EVENT_PS2) {
		uint32_t ps2_data = dequeue_ps2_buffer();

		if(ps2_data != PS2_NOK) {
			enqueue_hid_event(EVENT_PS2_KEYBOARD, ps2_data, 0);

			has_processed_buffers = true;
			printf("\tps2_data=%08x\n", ps2_data);
		}
	}

	return has_processed_buffers;
}