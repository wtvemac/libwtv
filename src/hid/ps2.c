#include "hid/ps2.h"
#include "../regs_internal.h"
#include "ps2_scancode_tables.h"
#include "wtvsys.h"
#include "libc.h"
#include "interrupt.h"

#define PS2_BUFFER_RETRY_MAX                     312000
#define PS2_FLUSH_RETRY_MAX                      16
#define PS2_COMMAND_RETRY_MAX                    16
#define PS2_LOCAL_BUFFER_SIZE                    16 // needs to be a multiple of 2.
#define PS2_LOCAL_BUFFER_INDEX_MASK              (PS2_LOCAL_BUFFER_SIZE - 1)

#define PS2_NOK                                  0xff
#define PS2_ACK                                  0xfa
#define PS2_PASSED                               0xaa
#define PS2_FAILED1                              0xfc
#define PS2_FAILED2                              0xfd
#define PS2_RESEND                               0xfe
#define PS2_ECHO                                 0xee

#define PS2_CNFG_SPECIAL2                        0x80
#define PS2_CNFG_TRANSLATE1                      0x40
#define PS2_CNFG_CLK2_EN                         0x20
#define PS2_CNFG_CLK1_EN                         0x10
#define PS2_CNFG_SPECIAL1                        0x08
#define PS2_CNFG_SYSTEM_FLAG                     0x04
#define PS2_CNFG_INT1_EN                         0x02
#define PS2_CNFG_INT2_EN                         0x01

#define PS2_STAT_PARITY_ERROR                    0x80
#define PS2_STAT_TIMEOUT_ERROR                   0x40
#define PS2_STAT_SPECIAL2                        0x20 // Chipset specific. Commonly recieve timeout or second port output buffer full
#define PS2_STAT_SPECIAL1                        0x10 // Chipset specific. Usually "keyboard lock" set.
#define PS2_STAT_GOT_COMMAND                     0x08
#define PS2_STAT_SYSTEM_FLAG                     0x04
#define PS2_STAT_IN_FULL                         0x02
#define PS2_STAT_OUT_FULL                        0x01

#define PS2_CNTLR_PORT1_DATA                     0x80
#define PS2_CNTLR_PORT1_CLK                      0x40
#define PS2_CNTLR_PORT2_OUT_FULL                 0x20
#define PS2_CNTLR_PORT1_OUT_FULL                 0x10
#define PS2_CNTLR_PORT2_DATA                     0x08
#define PS2_CNTLR_PORT2_CLK                      0x04
#define PS2_CNTLR_A20                            0x02
#define PS2_CNTLR_RESET                          0x01

#define PS2_CNTLR_CNFG_INDEX                     0x00

#define PS2_COMMAND_CNTLR_READ_BYTE(byte_index)  (0x20 | (byte_index & 0x1f))
#define PS2_COMMAND_CNTLR_WRITE_BYTE(byte_index) (0x60 | (byte_index & 0x1f))
#define PS2_COMMAND_CNTLR_TEST                   0xaa
#define PS2_COMMAND_CNTLR_IN_READ                0xc0
#define PS2_COMMAND_CNTLR_IN_COPY_0347           0xc1
#define PS2_COMMAND_CNTLR_IN_COPY_4747           0xc2
#define PS2_COMMAND_CNTLR_OUT_READ               0xd0
#define PS2_COMMAND_CNTLR_OUT_WRITE              0xd1
#define PS2_COMMAND_CNTLR_OUT_PULSE(bit_index)   (0xf0 | (bit_index & 0xf))

#define PS2_COMMAND_PORT1_DISABLE                0xad
#define PS2_COMMAND_PORT1_ENABLE                 0xae
#define PS2_COMMAND_PORT1_TEST                   0xab
#define PS2_COMMAND_PORT1_OUT_WRITE              0xd2

#define PS2_COMMAND_PORT2_DISABLE                0xa7
#define PS2_COMMAND_PORT2_ENABLE                 0xa8
#define PS2_COMMAND_PORT2_TEST                   0xa9
#define PS2_COMMAND_PORT2_OUT_WRITE              0xd3
#define PS2_COMMAND_PORT2_IN_WRITE               0xd4

#define PS2_COMMAND_A20_ASSERT                   0xdf
#define PS2_COMMAND_A20_CLEAR                    0xdd

#define PS2_COMMAND_SET_LEDS                     0xed
#define PS2_COMMAND_ECHO                         0xee
#define PS2_COMMAND_SCANCODE                     0xf0
#define PS2_COMMAND_IDENTIFY                     0xf2
#define PS2_COMMAND_SET_AUTOREPEAT_PARAMS        0xf3
#define PS2_COMMAND_ENABLE_SCANNING              0xf4
#define PS2_COMMAND_DISABLE_SCANNING             0xf5
#define PS2_COMMAND_SET_DEFAULTS                 0xf6
#define PS2_COMMAND_ALL_KEY_REPEAT               0xf7
#define PS2_COMMAND_ALL_KEY_DOWNUP               0xf8
#define PS2_COMMAND_ALL_KEY_DOWN                 0xf9
#define PS2_COMMAND_ALL_KEY_FULL                 0xfa
#define PS2_COMMAND_SINGLE_KEY_REPEAT            0xfb
#define PS2_COMMAND_SINGLE_KEY_DOWNUP            0xfc
#define PS2_COMMAND_SINGLE_KEY_DOWN              0xfd
#define PS2_COMMAND_RESEND                       0xfe
#define PS2_COMMAND_RESET                        0xff

#define PS2_CNTRL_TEST_PASSED                    0x55
#define PS2_CNTRL_TEST_FAILED                    0xfc

#define PS2_PORT_TEST_PASSED                     0x00
#define PS2_PORT_TEST_FAILED                     0x01

#define PS2_SET_SCANCODE_SET3                    0x08
#define PS2_SET_SCANCODE_SET2                    0x04
#define PS2_SET_SCANCODE_SET1                    0x02
#define PS2_GET_SCANCODE_SET                     0x01

#define PS2_SCANCODE_SET1                        0x43
#define PS2_SCANCODE_SET2                        0x41
#define PS2_SCANCODE_SET3                        0x3f

#define PS2_DATA                                 KBD0
#define PS2_CNTL                                 KBD1

static bool     ps2_initilized       = false;
static uint8_t  ps2_led_state        = 0x00;
static uint8_t  ps2_identity         = PS2_IDENTITY_IS_UNKNOWN;
static uint8_t  ps2_scancode_set     = PS2_SCANCODE_SET2;
static uint8_t  ps2_autorepeat_rate  = PS2_AUTOREPEAT_RATE_10P9HZ;
static uint8_t  ps2_autorepeat_delay = PS2_AUTOREPEAT_DELAY_500MS;
static uint8_t  ps2_buffer[PS2_LOCAL_BUFFER_SIZE];
static uint8_t  ps2_buffer_head = 0;
static uint8_t  ps2_buffer_tail = 0;

bool __ps2_write(uint8_t register_index, uint8_t data)
{
	for(int retry_countdown = PS2_BUFFER_RETRY_MAX; retry_countdown > 0; retry_countdown--)
	{
		if(!(REGISTER_READ(DEV_KBD_BASE + PS2_CNTL) & PS2_STAT_IN_FULL))
		{
			REGISTER_WRITE(DEV_KBD_BASE + register_index, (uint32_t)data);

			return true;
		}
	}

	return false;
}

uint8_t __ps2_read(uint8_t register_index)
{
	for(int retry_countdown = PS2_BUFFER_RETRY_MAX; retry_countdown > 0; retry_countdown--)
	{
		if((REGISTER_READ(DEV_KBD_BASE + PS2_CNTL) & PS2_STAT_OUT_FULL))
		{
			return (uint8_t)REGISTER_READ(DEV_KBD_BASE + register_index);
		}
	}

	return PS2_NOK;
}

uint8_t __ps2_read_buffer(bool raw)
{
	uint8_t buffer_byte = PS2_NOK;

	disable_interrupts();

	if(raw)
	{
		buffer_byte = __ps2_read(PS2_DATA);
	}
	else if(ps2_buffer_tail != ps2_buffer_head)
	{
		ps2_buffer_tail = (ps2_buffer_tail + 1) & PS2_LOCAL_BUFFER_INDEX_MASK;

		buffer_byte = ps2_buffer[ps2_buffer_tail];
	}

	enable_interrupts();

	return buffer_byte;
}

uint8_t __ps2_device_command_data(uint8_t *data, uint8_t length)
{
	for(int i = 0; i < length; i++)
	{
		if(__ps2_write(PS2_DATA, *(data + i)))
		{
			uint8_t response = __ps2_read(PS2_DATA);
			
			if(response != PS2_ACK)
			{
				return response;
			}
		}
		else
		{
			return PS2_NOK;
		}
	}

	return PS2_ACK;
}

bool __ps2_device_command(uint8_t command, uint8_t *data, uint8_t length)
{
	for(int retry_countdown = PS2_COMMAND_RETRY_MAX; retry_countdown > 0; retry_countdown--)
	{
		if(__ps2_write(PS2_DATA, command))
		{
			uint8_t response = __ps2_read(PS2_DATA);

			if(response == PS2_RESEND)
			{
				continue;
			}
			else if(response == PS2_ACK)
			{
				uint8_t response = __ps2_device_command_data(data, length);

				if(response == PS2_RESEND)
				{
					continue;
				}
				else
				{
					return (response == PS2_ACK);
				}
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

uint8_t __ps2_device_command_return(uint8_t command, uint8_t *data, uint8_t length)
{
	if(__ps2_device_command(command, data, length))
	{
		return __ps2_read(PS2_DATA);
	}
	else
	{
		return PS2_NOK;
	}
}

bool __ps2_flush()
{
	for(int retry_countdown = PS2_FLUSH_RETRY_MAX; retry_countdown > 0; retry_countdown--)
	{
		if(__ps2_read(PS2_DATA) == PS2_NOK)
		{
			return true;
		}
	}

	return false;
}

bool __ps2_device_reset()
{
	disable_interrupts();

	bool ok = (
		__ps2_flush()
		&& (__ps2_device_command_return(PS2_COMMAND_RESET, 0, 0) == PS2_PASSED)
	);

	enable_interrupts();

	return ok;
}

bool __ps2_port_enable()
{
	disable_interrupts();

	bool ok = __ps2_write(PS2_CNTL, PS2_COMMAND_PORT1_ENABLE);

	enable_interrupts();

	return ok;
}

bool __ps2_port_disable()
{
	disable_interrupts();

	bool ok = __ps2_write(PS2_CNTL, PS2_COMMAND_PORT1_DISABLE);

	enable_interrupts();

	return ok;
}

bool __ps2_set_controller_config(uint8_t config)
{
	disable_interrupts();

	bool ok = (
		__ps2_write(PS2_CNTL, PS2_COMMAND_CNTLR_WRITE_BYTE(PS2_CNTLR_CNFG_INDEX))
		&& __ps2_write(PS2_DATA, config)
	);

	enable_interrupts();

	return ok;
}

bool __ps2_controller_self_test()
{
	disable_interrupts();

	bool ok = (
		__ps2_write(PS2_CNTL, PS2_COMMAND_CNTLR_TEST)
		&& (__ps2_read(PS2_DATA) == PS2_CNTRL_TEST_PASSED)
	);

	enable_interrupts();

	return ok;
}

uint16_t __ps2_parse_scancode(const scancode_t scancode_table[], bool raw)
{
	uint8_t data = __ps2_read_buffer(raw);

	if(data == PS2_NOK) { return PS2_NOK; }

	for(int i = 0; !IS_SCANCODE_TABLE_END(scancode_table, i) ; i++)
	{
		if(scancode_table[i].input_code == data)
		{
			if(scancode_table[i].result_code == PS2_CODE_CONTINUE)
			{
				return __ps2_parse_scancode(scancode_table[i].next_code, raw);
			}
			else
			{
				return scancode_table[i].result_code;
			}
		}
	}

	return PS2_NOK;
}

void __ps2_input_callback()
{
	while(true)
	{
		uint8_t data = __ps2_read(PS2_DATA);

		if((ps2_buffer_head + 1) != ps2_buffer_tail)
		{
			ps2_buffer_head = (ps2_buffer_head + 1) & PS2_LOCAL_BUFFER_INDEX_MASK;

			ps2_buffer[ps2_buffer_head] = data;
		}

		//printf("data=%02x\n", data);

		if(__ps2_read(PS2_CNTL) & PS2_STAT_OUT_FULL)
		{
			break;
		}
	}
}

void __ps2_select_scancode_table(uint8_t scancode_set)
{
	ps2_scancode_set = scancode_set;
}

uint8_t __ps2_get_scancode_set()
{
	if(!ps2_initilized) { return PS2_SCANCODE_SET2; }

	disable_interrupts();

	uint8_t scancode_set = __ps2_device_command_return(PS2_COMMAND_SCANCODE, (uint8_t*)(PS2_GET_SCANCODE_SET), 1);

	enable_interrupts();

	return scancode_set;
}

bool __ps2_set_scancode_set(uint8_t scancode_set)
{
	if(!ps2_initilized) { return false; }

	disable_interrupts();

	uint8_t _scancode_set = PS2_SET_SCANCODE_SET1;
	switch(scancode_set)
	{
		case PS2_SCANCODE_SET1:
			_scancode_set = PS2_SET_SCANCODE_SET1;
			break;
		case PS2_SCANCODE_SET2:
			_scancode_set = PS2_SET_SCANCODE_SET2;
			break;
		case PS2_SCANCODE_SET3:
			_scancode_set = PS2_SET_SCANCODE_SET3;
			break;
		default:
			scancode_set = PS2_SCANCODE_SET2;
			_scancode_set = PS2_SET_SCANCODE_SET2;
			break;
	}

	bool ok = (
		__ps2_device_command(PS2_COMMAND_SCANCODE, &_scancode_set, 1)
	);

	if(ok)
	{
		__ps2_select_scancode_table(scancode_set);
	}

	enable_interrupts();

	return ok;
}

uint8_t __ps2_identify()
{
	if(!ps2_initilized) { return PS2_IDENTITY_IS_UNKNOWN; }

	disable_interrupts();

	uint8_t identity = PS2_IDENTITY_IS_UNKNOWN;

	if(__ps2_device_command(PS2_COMMAND_IDENTIFY, 0, 0))
	{
		identity = __ps2_parse_scancode(keyboard_identiy, true);
	}

	enable_interrupts();

	return identity;
}

bool ps2_set_autorepeat_params(uint8_t repeat_rate, uint8_t repeat_delay)
{
	if(!ps2_initilized) { return false; }
	
	disable_interrupts();
	
	uint8_t autorepeat_params = ((repeat_delay & 0x3) << 5) | (repeat_rate & 0x1f);

	bool ok = (
		__ps2_device_command(PS2_COMMAND_SET_AUTOREPEAT_PARAMS, &autorepeat_params, 1)
	);

	if(ok)
	{
		ps2_autorepeat_rate = repeat_rate;
		ps2_autorepeat_delay = repeat_delay;
	}

	enable_interrupts();
	
	return ok;
}

uint8_t ps2_get_leds()
{
	if(!ps2_initilized) { return false; }

	return ps2_led_state;
}

uint8_t ps2_set_leds(uint8_t led_state)
{
	if(!ps2_initilized) { return false; }
	
	disable_interrupts();

	bool ok = (
		__ps2_device_command(PS2_COMMAND_SET_LEDS, &led_state, 1)
	);

	if(ok)
	{
		ps2_led_state = led_state;
	}

	enable_interrupts();
	
	return ps2_led_state;
}

uint16_t dequeue_ps2_buffer()
{
	switch(ps2_scancode_set)
	{
		case PS2_NOK:
		case PS2_SCANCODE_SET1:
			return __ps2_parse_scancode(scancode_set1, false);
		case PS2_SCANCODE_SET2:
		default:
			return __ps2_parse_scancode(scancode_set2, false);
		case PS2_SCANCODE_SET3:
			return __ps2_parse_scancode(scancode_set3, false);
	}
}

bool ps2_is_mouse()
{
	return (ps2_identity & PS2_IS_MOUSE);
}

char* ps2_get_identity()
{
	switch(ps2_identity)
	{
		case PS2_IDENTITY_IS_MF2_KEYBOARD:
			return "MF2 keyboard";
			break;
		case PS2_IDENTITY_IS_SHORT_KEYBOARD:
			return "Short keyboard";
			break;
		case PS2_IDENTITY_IS_HOST_KEYBOARD:
			return "Host-connected keyboard or NCD N-97 keyboard";
			break;
		case PS2_IDENTITY_IS_122KEY_KEYBOARD:
			return "122-key keyboard";
			break;
		case PS2_IDENTITY_IS_JAPG_KEYBOARD:
			return "Japanese G keyboard";
			break;
		case PS2_IDENTITY_IS_JAPP_KEYBOARD:
			return "Japanese P keyboard";
			break;
		case PS2_IDENTITY_IS_JAPA_KEYBOARD:
			return "Japanese A keyboard";
			break;
		case PS2_IDENTITY_IS_SUN_KEYBOARD:
			return "NCD Sun keyboard";
			break;
		case PS2_IDENTITY_IS_MOUSE:
			return "Two-button mouse";
			break;
		case PS2_IDENTITY_IS_WHEEL_MOUSE:
			return "Two-button mouse with scroll wheel";
			break;
		case PS2_IDENTITY_IS_5KEY_MOUSE:
			return "Five-button mouse";
			break;
		default:
			return "Older AT keyboard or unknown device";
			break;
	}
}

bool ps2_set_default()
{
	if(!ps2_initilized) { return false; }
	
	disable_interrupts();

	bool ok = (
		__ps2_device_command(PS2_COMMAND_SET_DEFAULTS, 0, 0)
	);

	if(ok)
	{
		__ps2_select_scancode_table(PS2_SCANCODE_SET2);
		ps2_autorepeat_rate = PS2_AUTOREPEAT_RATE_10P9HZ;
		ps2_autorepeat_delay = PS2_AUTOREPEAT_DELAY_500MS;
	}

	enable_interrupts();

	return ok;

	
}

bool ps2_init()
{
	bool ok = false;

	if(is_spot_box())
	{
		ok = (
			__ps2_controller_self_test()
			&& __ps2_port_enable()
			&& __ps2_device_reset()
			&& __ps2_set_controller_config(PS2_CNFG_INT2_EN | PS2_CNFG_TRANSLATE1)
		);

		if(ok)
		{
			register_DEV_PS2_handler(__ps2_input_callback);
			set_DEV_PS2_interrupt(1);

			ps2_initilized = ok;
			ps2_led_state = 0x00;
			ps2_buffer_head = 0;
			ps2_buffer_tail = 0;
			ps2_identity = __ps2_identify();

			ps2_set_default();

			__ps2_select_scancode_table(__ps2_get_scancode_set());
		}
	}

	return ok;
}

bool ps2_close()
{
	if(!ps2_initilized) { return false; }

	unregister_DEV_PS2_handler(__ps2_input_callback);
	set_DEV_PS2_interrupt(0);

	bool ok = (
		__ps2_port_disable()
	);

	return ok;
}