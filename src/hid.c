#include "hid.h"
#include "libc.h"

static hid_state previous_state;

void hid_init()
{
	hid_events_enable_all();
}

void hid_scan()
{
	previous_state = hid_current_state();

	process_hid_buffers();
}

hid_state hid_keys_pressed()
{
	return hid_current_state();
}

hid_state hid_keys_held()
{
	hid_state current_state = hid_current_state();
	hid_state ret;

	ret.modifiers = previous_state.modifiers & current_state.modifiers;

	for(int i = 0; i < current_state.keys_pressed_count; i++)
	{
		bool key_found = false;

		for(int j = 0; j < previous_state.keys_pressed_count; j++)
		{
			if(current_state.keys_pressed[i].data == current_state.keys_pressed[j].data)
			{
				key_found = true;
				break;
			}
		}

		if(key_found)
		{
			ret.keys_pressed[ret.keys_pressed_count] = current_state.keys_pressed[i];
			ret.keys_pressed_count++;
		}
	}

	return ret;
}

hid_state hid_keys_down()
{
	hid_state current_state = hid_current_state();
	hid_state ret;

	ret.modifiers = (current_state.modifiers) & ~(previous_state.modifiers);

	for(int i = 0; i < current_state.keys_pressed_count; i++)
	{
		bool key_found = false;

		for(int j = 0; j < previous_state.keys_pressed_count; j++)
		{
			if(current_state.keys_pressed[i].data == current_state.keys_pressed[j].data)
			{
				key_found = true;
				break;
			}
		}

		if(!key_found)
		{
			ret.keys_pressed[ret.keys_pressed_count] = current_state.keys_pressed[i];
			ret.keys_pressed_count++;
		}
	}

	return ret;
}

hid_state hid_keys_up()
{
	hid_state current_state = hid_current_state();
	hid_state ret;

	ret.modifiers = ~(current_state.modifiers) & (previous_state.modifiers);

	for(int i = 0; i < previous_state.keys_pressed_count; i++)
	{
		bool key_found = false;

		for(int j = 0; j < current_state.keys_pressed_count; j++)
		{
			if(current_state.keys_pressed[i].data == current_state.keys_pressed[j].data)
			{
				key_found = true;
				break;
			}
		}

		if(!key_found)
		{
			ret.keys_pressed[ret.keys_pressed_count] = current_state.keys_pressed[i];
			ret.keys_pressed_count++;
		}
	}

	return ret;
}

void hid_close()
{
	hid_events_disable_all();
}

struct controller_data __hid_state_to_controller_data(hid_state state)
{
	struct controller_data ret;

	memset(&ret, 0, sizeof(struct controller_data));

	ret.c[0].err = 0x00; // ERROR_NONE

	// This could possibly be filled in with a PS2 mouse or support for other analog inputs
	// I may put in basic key to analog translation (like press key X and go a % in one direction)
	ret.c[0].x = 0;
	ret.c[0].y = 0;

	for(int i = 0; i < state.keys_pressed_count; i++)
	{
		switch(state.keys_pressed[i].data)
		{
			case KY_ENTER:
				ret.c[0].start = 1;
				break;

			case KY_J:
				ret.c[0].A = 1;
				break;
			case KY_N:
				ret.c[0].B = 1;
				break;
			case KY_SPACE:
				ret.c[0].Z = 1;
				break;

			case KY_H:
				ret.c[0].L = 1;
				break;
			case KY_K:
				ret.c[0].R = 1;
				break;

			case KY_UP:
				ret.c[0].up = 1;
				break;
			case KY_DOWN:
				ret.c[0].down = 1;
				break;
			case KY_LEFT:
				ret.c[0].left = 1;
				break;
			case KY_RIGHT:
				ret.c[0].right = 1;
				break;

			case KY_7:
				ret.c[0].C_up = 1;
				break;
			case KY_U:
				ret.c[0].C_down = 1;
				break;
			case KY_Y:
				ret.c[0].C_left = 1;
				break;
			case KY_I:
				ret.c[0].C_right = 1;
				break;
		}
	}

	return ret;
}

void controller_read(struct controller_data* output)
{
	struct controller_data ret = __hid_state_to_controller_data(hid_current_state());

	memcpy(output, &ret, sizeof(struct controller_data));
}

struct controller_data get_keys_pressed()
{
	return __hid_state_to_controller_data(hid_current_state());
}

struct controller_data get_keys_held()
{
	struct controller_data current = __hid_state_to_controller_data(hid_current_state());
	struct controller_data prev = __hid_state_to_controller_data(previous_state);
	struct controller_data ret = current;

	/* Figure out which wasn't pressed last time and is now */
	ret.c[0].data = (current.c[0].data) & (prev.c[0].data);

	return ret;
}

struct controller_data get_keys_down()
{
	struct controller_data current = __hid_state_to_controller_data(hid_current_state());
	struct controller_data prev = __hid_state_to_controller_data(previous_state);
	struct controller_data ret = current;

	/* Figure out which wasn't pressed last time and is now */
	ret.c[0].data = (current.c[0].data) & ~(prev.c[0].data);

	return ret;
}

struct controller_data get_keys_up()
{
	struct controller_data current = __hid_state_to_controller_data(hid_current_state());
	struct controller_data prev = __hid_state_to_controller_data(previous_state);
	struct controller_data ret = current;

	/* Figure out which wasn't pressed last time and is now */
	ret.c[0].data = ~(current.c[0].data) & (prev.c[0].data);

	return ret;
}

int get_dpad_direction(int controller)
{
	struct controller_data current = __hid_state_to_controller_data(hid_current_state());

	/* Diagonals first because it could only be right angles otherwise */
	if(current.c[controller & 0x3].up && current.c[controller & 0x3].left)
	{
		return 3;
	}

	if(current.c[controller & 0x3].up && current.c[controller & 0x3].right)
	{
		return 1;
	}

	if(current.c[controller & 0x3].down && current.c[controller & 0x3].left)
	{
		return 5;
	}

	if(current.c[controller & 0x3].down && current.c[controller & 0x3].right)
	{
		return 7;
	}

	if(current.c[controller & 0x3].right)
	{
		return 0;
	}

	if(current.c[controller & 0x3].up)
	{
		return 2;
	}

	if(current.c[controller & 0x3].left)
	{
		return 4;
	}

	if(current.c[controller & 0x3].down)
	{
		return 6;
	}

	return -1;
}


int get_accessories_present(struct controller_data * data)
{
	return 0;
}

int get_controllers_present()
{
	return 1;
}

void rumble_start(int controller)
{
	// Do nothing. No rumble pak on the WebTV (yet?).
}

void rumble_stop(int controller)
{
	// Do nothing. No rumble pak on the WebTV (yet?).
}