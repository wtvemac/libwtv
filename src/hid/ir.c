#include "hid/ir.h"
#include "../regs_internal.h"
#include "ir_scancode_tables.h"
#include "interrupt.h"
#include "wtvsys.h"
#include "serial.h"
#include "libc.h"

typedef struct ir_bit_time_limit_s
{
	uint16_t max;
	uint16_t med;
	uint16_t min;
} ir_bit_time_limit_t;

// (1000 * 1000) / 38KHz = 26.3us * 30 pulses per bit = 789us per bit / 15us per sample = 52.6 samples
ir_bit_time_limit_t seijinkb_bit_time_limits[] = {
	// 0
	{
		.max = 0x3d,
		.med = 0x30,
		.min = 0x2a
	},
	// 1
	{
		.max = 0x38,
		.med = 0x2b,
		.min = 0x1c
	},
};

#define IR_FLUSH_RETRY_MAX                      16
#define IR_USECS_PER_IR_SAMPLE                  15
#define IR_TICKS_PER_IR_SAMPLE                  TICKS_FROM_US(IR_USECS_PER_IR_SAMPLE)
#define IR_REJECT_TIMING                        0xa
#define IR_INTERRUPT_THRESHOLD                  8 // 0b110000 1000 0 011

#define IR_BUFFER_SIZE                          80 // needs to be a multiple of 2.
#define IR_BUFFER_INDEX_MASK                    (IR_BUFFER_SIZE - 1)

#define GET_INCREMENTED_IR_INDEX(index)         index = (index + 1) & IR_BUFFER_INDEX_MASK

/** SOLO IR input transition data
 *
 *  The SOLO chip uses an IR transistion buffer that adds an entry everytime
 *  the IR bit transitions from 1 to 0 or 0 to 1. It logs the time between 
 *  entries as well as the bit value. The time can be used to tell the amount
 *  to repeat the bit value. For example, a bit value of 1 across 60uS could 
 *  mean two 1's were sent @ 30uS per bit.
 *
 * Every read of the IR transition register shifts an entry off the buffer.
 *
 *  IR transition register data bits:
 *      SSSS | V | TTTTTTTTTTT
 *
 *  SSSS        = the number of transition entries in the buffer (FIFO)
 *  V           = value of the current bit
 *  TTTTTTTTTTT = the time of the current bit transition measured in sample clocks. 
 *                1 sample clock is defined in the register DEV_IR_IN_SAMPLE_TICKS 
 *                which is number of system clock cycles per sample clock.
 *
 *                DEV_IR_IN_SAMPLE_TICKS is set to IR_TICKS_PER_IR_SAMPLE in ir_init()
 */

#define IR_TRANSITION_SAMPLE_COUNT(ir_data)         ((ir_data >> 0x0c))
#define IR_TRANSITION_CURRENT_VALUE(ir_data)        ((ir_data >> 0x0b) & 0x01)
#define IR_TRANSITION_CURRENT_TIME(ir_data)         ((ir_data & 0x07ff))

#define IS_IR_TRANSITION_SAMPLE_COUNT_GOOD(ir_data) (IR_TRANSITION_SAMPLE_COUNT(ir_data) != 0xf)

/** Seijin Keyboard Data Format
 *
 * Seijin's wireless IR keyboard uses a 38KHz carrier frequency and each bit is 789uS wide (30 pulses per bit)
 * There 22 bits per key event. This allows about 57.6 key events per second.
 *
 *  There's two sections in Seijin's key event data format. Each section starts with a 0 and end with a 1.
 *
 *  Seijin key event data bits:
 *     [1 P 0 SSSSSSS 0] [1 P 1 M DD 0100 0]
 *
 *  DD      = ID bit, usually 01 for WebTV's keyboard (01 = 103/86 keyboard)
 *  M       = 0=make (key press), 1=break (key release)
    P       = odd parity bit
 *  SSSSSSS = scancode for the key pressed
 *
 * Because there's bits that are always 1 or 0, we can use the mask 0x200508 (0b1000000000010100001000)
 * to loosly verify the integirty of the data.
 */

#define IS_SEIJINKB_TIMING_GOOD(ir_data)            (seijinkb_bit_time_limits[IR_TRANSITION_CURRENT_VALUE(ir_data) & 0x1].min <= IR_TRANSITION_CURRENT_TIME(ir_data) && (IR_TRANSITION_CURRENT_TIME(ir_data) <= seijinkb_bit_time_limits[IR_TRANSITION_CURRENT_VALUE(ir_data) & 0x1].max || IR_TRANSITION_CURRENT_TIME(ir_data) >= (seijinkb_bit_time_limits[IR_TRANSITION_CURRENT_VALUE(ir_data) & 0x1].min * 2)))
#define IS_SEIJINKB_GOOD(ir_data)                   (IS_IR_TRANSITION_SAMPLE_COUNT_GOOD(ir_data) && IS_SEIJINKB_TIMING_GOOD(ir_data))
#define IR_SEIJINKB_BIT_COUNT(value, time)          ((time > seijinkb_bit_time_limits[time & 0x1].max) ? (time / seijinkb_bit_time_limits[time & 0x1].med) : 1)
#define IR_SEIJINKB_DATA_BITS                       0x16
#define IR_SEIJINKB_DATA_MASK                       0x200508
#define IS_SEIJINKB_DATA_GOOD(result_data)          ((result_data & IR_SEIJINKB_DATA_MASK) == IR_SEIJINKB_DATA_MASK)

static bool     ir_initilized       = false;
static uint16_t ir_buffer[IR_BUFFER_SIZE];
static uint8_t  ir_buffer_head = 0;
static uint8_t  ir_buffer_tail = 0;

static uint32_t ir_working_result_bit_count = 0;
static uint32_t ir_working_result_data = 0;

void enqueue_ir_buffer(uint16_t ir_data)
{
	if((ir_buffer_head + 1) != ir_buffer_tail)
	{
		ir_buffer[GET_INCREMENTED_IR_INDEX(ir_buffer_head)] = ir_data;
	}
}

uint32_t dequeue_ir_buffer()
{
	while(ir_buffer_tail != ir_buffer_head)
	{
		uint16_t ir_data = ir_buffer[GET_INCREMENTED_IR_INDEX(ir_buffer_tail)];

		if(IS_SEIJINKB_GOOD(ir_data))
		{
			uint8_t bit_count = IR_SEIJINKB_BIT_COUNT(bit_value, IR_TRANSITION_CURRENT_TIME(ir_data));
			uint8_t bit_value = IR_TRANSITION_CURRENT_VALUE(ir_data);

			if(bit_value == 1)
			{
				ir_working_result_data |= (((1 << bit_count) - 1) << ir_working_result_bit_count);
			}
			
			ir_working_result_bit_count += bit_count;

			if(ir_working_result_bit_count >= IR_SEIJINKB_DATA_BITS)
			{
				uint32_t ir_final_result_data = ir_working_result_data;

				ir_working_result_bit_count = 0;
				ir_working_result_data = 0;

				// Carry over bit data to next key event if bit count isn't over IR_SEIJINKB_DATA_BITS (which is usually a reset)
				if(bit_count < IR_SEIJINKB_DATA_BITS && ir_working_result_bit_count > IR_SEIJINKB_DATA_BITS)
				{
					ir_working_result_bit_count = (ir_working_result_bit_count - IR_SEIJINKB_DATA_BITS);
					if(bit_value == 1)
					{
						ir_working_result_data |= ((1 << ir_working_result_bit_count) - 1);
					}
				}

				if(IS_SEIJINKB_DATA_GOOD(ir_final_result_data))
				{
					return GET_SENJINKB_RESULT_CODE((ir_final_result_data >> 0x0c), ((ir_final_result_data & 0x80) == 0x80));
				}
				else
				{
					return IR_NOK;
				}
			}
		}
	}

	return IR_NOK;
}

void __solo_ir_input_callback()
{
	//serial_put_string("__solo_ir_input_callback()\x0a\x0d");
	while(true)
	{
		uint16_t ir_data = (REGISTER_READ(DEV_IR_IN_TRANS_DATA) & 0xffff);

		if(ir_data & 0xf000)
		{
			enqueue_ir_buffer(ir_data);
		}
		else
		{
			break;
		}
	}
}

bool __ir_flush()
{
	if(is_spot_box())
	{
	}
	else
	{
		for(int retry_countdown = IR_FLUSH_RETRY_MAX; retry_countdown > 0; retry_countdown--)
		{
			if(!(REGISTER_READ(DEV_IR_IN_TRANS_DATA) & 0xf000))
			{
				return true;
			}
		}
	}

	return true;
}

bool ir_init()
{
    disable_interrupts();

	bool ok = false;

	if(is_spot_box())
	{
		//register_DEV_PS2_handler(void (*callback)())
	}
	else
	{
		ir_initilized = true;

		REGISTER_WRITE(DEV_IR_IN_SAMPLE_TICKS, IR_TICKS_PER_IR_SAMPLE);
		REGISTER_WRITE(DEV_IR_IN_REJECT_TIME, IR_REJECT_TIMING);
		REGISTER_WRITE(DEV_IR_IN_STAT_CNTL, (IR_STATUS_IN_INTTHRESH(IR_INTERRUPT_THRESHOLD) | IR_STATUS_IN_ENABLED | IR_STATUS_IN_RESET));

		register_DEV_IR_IN_handler(__solo_ir_input_callback);
		set_DEV_IR_IN_interrupt(1);

		__ir_flush();

		ok = true;
	}

	enable_interrupts();

	return ok;
}

bool ir_close()
{
	if(!ir_initilized) { return false; }

	if(is_spot_box())
	{
		//unregister_DEV_IR_IN_handler(void (*callback)())
	}
	else
	{
		unregister_DEV_IR_OUT_handler(__solo_ir_input_callback);
		set_DEV_IR_IN_interrupt(0);
	}

	return true;
}