#include "hid/event.h"
#include "libc.h"
#include "interrupt.h"

#define HID_EVENT_BUFFER_SIZE       16 // needs to be a multiple of 2.
#define HID_EVENT_BUFFER_INDEX_MASK (HID_EVENT_BUFFER_SIZE - 1)

static uint32_t          _enabled_events   = 0x00;

static hid_event         hid_event_buffer[HID_EVENT_BUFFER_SIZE];
static uint8_t           hid_event_buffer_head = 0;
static uint8_t           hid_event_buffer_tail = 0;

static hid_state current_state;

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

hid_state hid_current_state()
{
	return current_state;
}

void enqueue_hid_event(hid_event event_object)
{
	if(event_object.source != EVENT_NULL)
	{
		if(event_object.source & EVENT_BUTTON)
		{
			uint16_t keycode = GET_KEYCODE(event_object.data);
			bool is_pressed = KY_IS_PRESSED(event_object.data);
			bool is_modifier = false;
			uint32_t changed_modifiers = 0x00;

			switch(keycode)
			{
				case KY_METAL:
					is_modifier = true;
					changed_modifiers |= KYMOD_METAL;
					break;
				case KY_METAR:
					is_modifier = true;
					changed_modifiers |= KYMOD_METAR;
					break;
				case KY_CTRLL:
					is_modifier = true;
					changed_modifiers |= KYMOD_CTRLL;
					break;
				case KY_CTRLR:
					is_modifier = true;
					changed_modifiers |= KYMOD_CTRLR;
					break;
				case KY_SHIFTL:
					is_modifier = true;
					changed_modifiers |= KYMOD_SHIFTL;
					break;
				case KY_SHIFTR:
					is_modifier = true;
					changed_modifiers |= KYMOD_SHIFTR;
					break;
				case KY_ALTL:
					is_modifier = true;
					changed_modifiers |= KYMOD_ALTL;
					break;
				case KY_ALTR:
					is_modifier = true;
					changed_modifiers |= KYMOD_ALTR;
					break;
				case KY_ALTGR:
					is_modifier = true;
					changed_modifiers |= KYMOD_ALTGR;
					break;
				case KY_CMD:
					is_modifier = true;
					changed_modifiers |= KYMOD_METAL;
					break;
				case KY_FN:
					is_modifier = true;
					changed_modifiers |= KYMOD_FN;
					break;
				case KY_CAPS_LOCK:
					is_modifier = true;
					if(is_pressed)
					{
						if(current_state.modifiers & KYMOD_CAPS_LOCK)
						{
							current_state.modifiers ^= KYMOD_CAPS_LOCK;
						}
						else
						{
							current_state.modifiers |= KYMOD_CAPS_LOCK;
						}
					}
					break;
				case KY_SCRL_LOCK:
					is_modifier = true;
					if(is_pressed)
					{
						if(current_state.modifiers & KYMOD_SCRL_LOCK)
						{
							current_state.modifiers ^= KYMOD_SCRL_LOCK;
						}
						else
						{
							current_state.modifiers |= KYMOD_SCRL_LOCK;
						}
					}
					break;
				case KY_NUMB_LOCK:
					is_modifier = true;
					if(is_pressed)
					{
						if(current_state.modifiers & KYMOD_NUMB_LOCK)
						{
							current_state.modifiers ^= KYMOD_NUMB_LOCK;
						}
						else
						{
							current_state.modifiers |= KYMOD_NUMB_LOCK;
						}
					}
					break;
			}

			event_object.is_modifier = is_modifier;

			if(is_pressed)
			{
				current_state.modifiers |= changed_modifiers;
				
				if(current_state.keys_pressed_count < EVENT_MAX_KEY_MEMORY)
				{
					bool duplicate_keycode = false;

					for(int i = 0; i < current_state.keys_pressed_count; i++)
					{
						if(current_state.keys_pressed[i].data == keycode)
						{
							duplicate_keycode = true;
							break;
						}
					}

					if(!duplicate_keycode)
					{
						memcpy(&current_state.keys_pressed[current_state.keys_pressed_count], &event_object, sizeof(hid_event));

						current_state.keys_pressed_count++;
					}
				}
			} else {
				current_state.modifiers ^= changed_modifiers;

				uint8_t new_keys_pressed_count = 0;

				for(int i = 0; i < current_state.keys_pressed_count; i++)
				{
					if(current_state.keys_pressed[i].data == keycode)
					{
						continue;
					}

					current_state.keys_pressed[new_keys_pressed_count] = current_state.keys_pressed[i];

					new_keys_pressed_count++;
				}

				current_state.keys_pressed_count = new_keys_pressed_count;
			}
		}

		if((hid_event_buffer_head + 1) != hid_event_buffer_tail)
		{
			hid_event_buffer_head = (hid_event_buffer_head + 1) & HID_EVENT_BUFFER_INDEX_MASK;

			memcpy(&hid_event_buffer[hid_event_buffer_head], &event_object, sizeof(hid_event));
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

    disable_interrupts();

	if(_enabled_events & EVENT_IR)
	{
		uint32_t data = dequeue_ir_buffer();

		if(data != IR_NOK)
		{
			hid_event event_object = {
				.source = EVENT_IR_KEYBOARD,
				.data = data,
				.time = 0
			};

			enqueue_hid_event(event_object);
			
			has_processed_buffers = true;
		}
	}

	if(_enabled_events & EVENT_PS2)
	{
		uint32_t data = dequeue_ps2_buffer();

		if(data != PS2_NOK)
		{
			hid_event event_object = {
				.source = EVENT_PS2_KEYBOARD,
				.data = data,
				.time = 0
			};

			enqueue_hid_event(event_object);

			has_processed_buffers = true;
		}
	}

    enable_interrupts();

	return has_processed_buffers;
}