#ifndef __LIBWTV_HID_PS2_H
#define __LIBWTV_HID_PS2_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PS2_NOK               0xff

// Kana mode, compose
#define PS2_LED_BIT7          0x10
#define PS2_LED_BIT6          0x10
#define PS2_LED_BIT5          0x10
#define PS2_LED_BIT4          0x10
#define PS2_LED_BIT3          0x08
#define PS2_LED_CAPSLOCK      0x04
#define PS2_LED_NUMBLOCK      0x02
#define PS2_LED_SCRLLOCK      0x01

#define PS2_AUTOREPEAT_DELAY_250MS  0x00
#define PS2_AUTOREPEAT_DELAY_500MS  0x01
#define PS2_AUTOREPEAT_DELAY_750MS  0x02
#define PS2_AUTOREPEAT_DELAY_1000MS 0x03

#define PS2_AUTOREPEAT_RATE_30P0HZ  0x00
#define PS2_AUTOREPEAT_RATE_27P7HZ  0x01
#define PS2_AUTOREPEAT_RATE_24P0HZ  0x02
#define PS2_AUTOREPEAT_RATE_21P8HZ  0x03
#define PS2_AUTOREPEAT_RATE_20P7HZ  0x04
#define PS2_AUTOREPEAT_RATE_18P5HZ  0x05
#define PS2_AUTOREPEAT_RATE_17P1HZ  0x06
#define PS2_AUTOREPEAT_RATE_16P0HZ  0x07
#define PS2_AUTOREPEAT_RATE_15P0HZ  0x08
#define PS2_AUTOREPEAT_RATE_13P3HZ  0x09
#define PS2_AUTOREPEAT_RATE_12P0HZ  0x0a
#define PS2_AUTOREPEAT_RATE_10P9HZ  0x0b
#define PS2_AUTOREPEAT_RATE_10P0HZ  0x0c
#define PS2_AUTOREPEAT_RATE_9P2HZ   0x0d
#define PS2_AUTOREPEAT_RATE_8P6HZ   0x0e
#define PS2_AUTOREPEAT_RATE_8P0HZ   0x0f
#define PS2_AUTOREPEAT_RATE_7P5HZ   0x10
#define PS2_AUTOREPEAT_RATE_6P7HZ   0x11
#define PS2_AUTOREPEAT_RATE_6P0HZ   0x12
#define PS2_AUTOREPEAT_RATE_5P5HZ   0x13
#define PS2_AUTOREPEAT_RATE_5P0HZ   0x14
#define PS2_AUTOREPEAT_RATE_4P6HZ   0x15
#define PS2_AUTOREPEAT_RATE_4P3HZ   0x16
#define PS2_AUTOREPEAT_RATE_4P0HZ   0x17
#define PS2_AUTOREPEAT_RATE_3P7HZ   0x18
#define PS2_AUTOREPEAT_RATE_3P3HZ   0x19
#define PS2_AUTOREPEAT_RATE_3P0HZ   0x1a
#define PS2_AUTOREPEAT_RATE_2P7HZ   0x1b
#define PS2_AUTOREPEAT_RATE_2P5HZ   0x1c
#define PS2_AUTOREPEAT_RATE_2P3HZ   0x1d
#define PS2_AUTOREPEAT_RATE_2P1HZ   0x1e
#define PS2_AUTOREPEAT_RATE_2P0HZ   0x1f


bool ps2_init();
bool ps2_set_default();
bool ps2_set_autorepeat_params(uint8_t repeat_rate, uint8_t repeat_delay);
uint8_t ps2_set_leds(uint8_t led_state);
char* ps2_get_identity();
bool ps2_is_mouse();
uint16_t dequeue_ps2_buffer();
bool ps2_close();

#ifdef __cplusplus
}
#endif

#endif
