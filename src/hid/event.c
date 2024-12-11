#include "hid/event.h"
#include "libc.h"

#define HID_EVENT_BUFFER_SIZE       16 // needs to be a multiple of 2.
#define HID_EVENT_BUFFER_INDEX_MASK (HID_EVENT_BUFFER_SIZE - 1)

static uint32_t          _enabled_events   = 0x00;

static hid_event         hid_event_buffer[HID_EVENT_BUFFER_SIZE];
static uint8_t           hid_event_buffer_head = 0;
static uint8_t           hid_event_buffer_tail = 0;

static hid_current_state current_state;

void hid_events_enable(uint32_t enable_events)
{
	uint32_t newly_enabled_events = _enabled_events ^ (_enabled_events | enable_events);

	if(newly_enabled_events & EVENT_IR)
	{
		ir_init();
	}

	if(newly_enabled_events & EVENT_PS2)
	{
		ps2_init();
	}

	_enabled_events |= enable_events;
}

void hid_events_enable_all()
{
	hid_events_enable(EVENT_IR | EVENT_PS2);
}

void enqueue_hid_event(uint16_t source, int16_t data, uint32_t time)
{
	if(source != EVENT_NULL)
	{
		if((hid_event_buffer_head + 1) != hid_event_buffer_tail)
		{
			hid_event_buffer_head = (hid_event_buffer_head + 1) & HID_EVENT_BUFFER_INDEX_MASK;

			hid_event_buffer[hid_event_buffer_head].source = source;
			hid_event_buffer[hid_event_buffer_head].data = data;
			hid_event_buffer[hid_event_buffer_head].time = time;
		}

		if(source & EVENT_BUTTON)
		{
			uint16_t keycode = GET_KEYCODE(data);

			if(KY_IS_PRESSED(data))
			{
				if(current_state.key_down_count < EVENT_MAX_KEY_MEMORY)
				{
					bool duplicate_keycode = false;

					for(int i = 0; i < current_state.key_down_count; i++)
					{
						if(current_state.keys_down[i].data == keycode)
						{
							duplicate_keycode = true;
							break;
						}
					}

					if(!duplicate_keycode)
					{
						current_state.keys_down[current_state.key_down_count].source = source;
						current_state.keys_down[current_state.key_down_count].data = data;
						current_state.keys_down[current_state.key_down_count].time = time;

						current_state.key_down_count++;
					}
				}
			} else {
				uint8_t new_key_down_count = 0;

				for(int i = 0; i < current_state.key_down_count; i++)
				{
					if(current_state.keys_down[i].data == keycode)
					{
						continue;
					}

					current_state.keys_down[new_key_down_count].source = current_state.keys_down[i].source;
					current_state.keys_down[new_key_down_count].data = current_state.keys_down[i].data;
					current_state.keys_down[new_key_down_count].time = current_state.keys_down[i].time;

					new_key_down_count++;
				}

				current_state.key_down_count = new_key_down_count;
			}
		}
	}
}

hid_event dequeue_hid_event()
{
	if(hid_event_buffer_tail != hid_event_buffer_head)
	{
		hid_event_buffer_tail = (hid_event_buffer_tail + 1) & HID_EVENT_BUFFER_INDEX_MASK;

		return hid_event_buffer[hid_event_buffer_tail];
	}
	else
	{
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

	if(_enabled_events & EVENT_IR)
	{
		uint32_t ir_data = dequeue_ir_buffer();

		if(ir_data != IR_NOK)
		{
			enqueue_hid_event(EVENT_IR_KEYBOARD, ir_data, 0);
			
			has_processed_buffers = true;
			printf("\tir_data=%08x, current_state.key_down_count=%d\n", ir_data, current_state.key_down_count);
		}
	}

	if(_enabled_events & EVENT_PS2)
	{
		uint32_t ps2_data = dequeue_ps2_buffer();

		if(ps2_data != PS2_NOK)
		{
			enqueue_hid_event(EVENT_PS2_KEYBOARD, ps2_data, 0);

			has_processed_buffers = true;
			printf("\tps2_data=%08x, current_state.key_down_count=%d\n", ps2_data, current_state.key_down_count);
		}
	}

	return has_processed_buffers;
}