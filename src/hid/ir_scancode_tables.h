#ifndef __LIBWTV_HID_IR_SCANCODE_TABLES_H
#define __LIBWTV_HID_IR_SCANCODE_TABLES_H

#include <stdint.h>
#include "hid/event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SENJINKB_SCANCODE_COUNT 0x80
#define GET_SENJINKB_RESULT_CODE(scancode_index, key_released) \
	( \
		(key_released) \
			? KEY_RELEASED(senjinkb_scancode_table[scancode_index & (SENJINKB_SCANCODE_COUNT - 1)]) \
			: KEY_PRESSED(senjinkb_scancode_table[scancode_index & (SENJINKB_SCANCODE_COUNT - 1)]) \
	)

const int16_t senjinkb_scancode_table[SENJINKB_SCANCODE_COUNT] =
{
	KEY_NULL,            /* 0x00 */
	KEY_NULL,            /* 0x01 */
	KEY_NULL,            /* 0x02 */
	KEY_NULL,            /* 0x03 */
	KEY_BACKTICK,        /* 0x04 */
	KEY_WTV_VIEW,        /* 0x05 */
	KEY_NULL,            /* 0x06 */
	KEY_NULL,            /* 0x07 */
	KEY_NULL,            /* 0x08 */
	KEY_CAPS_LOCK,       /* 0x09 */
	KEY_X,               /* 0x0a */
	KEY_F5,              /* 0x0b */
	KEY_F1,              /* 0x0c */
	KEY_2,               /* 0x0d */
	KEY_S,               /* 0x0e */
	KEY_W,               /* 0x0f */
	KEY_NULL,            /* 0x10 */
	KEY_F3,              /* 0x11 */
	KEY_C,               /* 0x12 */
	KEY_F4,              /* 0x13 */
	KEY_F2,              /* 0x14 */
	KEY_3,               /* 0x15 */
	KEY_D,               /* 0x16 */
	KEY_E,               /* 0x17 */
	KEY_ALTL,            /* 0x18 */
	KEY_TAB,             /* 0x19 */
	KEY_Z,               /* 0x1a */
	KEY_POWER,           /* 0x1b */
	KEY_CMD,             /* 0x1c */
	KEY_1,               /* 0x1d */
	KEY_A,               /* 0x1e */
	KEY_Q,               /* 0x1f */
	KEY_B,               /* 0x20 */
	KEY_T,               /* 0x21 */
	KEY_V,               /* 0x22 */
	KEY_G,               /* 0x23 */
	KEY_5,               /* 0x24 */
	KEY_4,               /* 0x25 */
	KEY_F,               /* 0x26 */
	KEY_R,               /* 0x27 */
	KEY_ALTR,            /* 0x28 */
	KEY_ESC,             /* 0x29 */
	KEY_NULL,            /* 0x2a */
	KEY_CTRLL,           /* 0x2b */
	KEY_NULL,            /* 0x2c */
	KEY_WTV_GOTO,        /* 0x2d */
	KEY_NULL,            /* 0x2e */
	KEY_WTV_SAVE,        /* 0x2f */
	KEY_NULL,            /* 0x30 */
	KEY_SHIFTL,          /* 0x31 */
	KEY_SHIFTR,          /* 0x32 */
	KEY_NULL,            /* 0x33 */
	KEY_NULL,            /* 0x34 */
	KEY_NULL,            /* 0x35 */
	KEY_MEDIA_CHAN_DOWN, /* 0x36 */
	KEY_MEDIA_CHAN_UP,   /* 0x37 */
	KEY_SPACE,           /* 0x38 */
	KEY_BACKSPACE,       /* 0x39 */
	KEY_RETURN,          /* 0x3a */
	KEY_F6,              /* 0x3b */
	KEY_WTV_HOME,        /* 0x3c */ /* aka F9 */
	KEY_WTV_SEARCH,      /* 0x3d */ /* aka F10 */
	KEY_BACKSLASH,       /* 0x3e */
	KEY_NULL,            /* 0x3f */
	KEY_NULL,            /* 0x40 */
	KEY_NULL,            /* 0x41 */
	KEY_NULL,            /* 0x42 */
	KEY_NULL,            /* 0x43 */
	KEY_NULL,            /* 0x44 */
	KEY_NULL,            /* 0x45 */
	KEY_FN,              /* 0x46 */
	KEY_NULL,            /* 0x47 */
	KEY_LEFT,            /* 0x48 */
	KEY_NULL,            /* 0x49 */
	KEY_WTV_SEND,        /* 0x4a */
	KEY_UP,              /* 0x4b */
	KEY_WTV_EDIT,        /* 0x4c */ /* aka Home */
	KEY_NULL,            /* 0x4d */
	KEY_WTV_BACK,        /* 0x4e */
	KEY_NULL,            /* 0x4f */
	KEY_DOWN,            /* 0x50 */
	KEY_NULL,            /* 0x51 */
	KEY_WTV_INFO,        /* 0x52 */
	KEY_NULL,            /* 0x53 */
	KEY_WTV_OPTION,      /* 0x54 */
	KEY_WTV_MAIL,        /* 0x55 */ /* aka F11 */
	KEY_PAGE_UP,         /* 0x56 */ /* aka Scroll Up */
	KEY_NULL,            /* 0x57 */
	KEY_RIGHT,           /* 0x58 */
	KEY_NULL,            /* 0x59 */
	KEY_NULL,            /* 0x5a */
	KEY_PAGE_DOWN,       /* 0x5b */ /* aka Scroll Down */
	KEY_WTV_RECENT,      /* 0x5c */
	KEY_WTV_FIND,        /* 0x5d */ /* aka F12 */
	KEY_NULL,            /* 0x5e */
	KEY_NULL,            /* 0x5f */
	KEY_NULL,            /* 0x60 */
	KEY_F7,              /* 0x61 */
	KEY_PERIOD,          /* 0x62 */
	KEY_NULL,            /* 0x63 */
	KEY_WTV_FAVORITES,   /* 0x64 */ /* aka F8 */
	KEY_9,               /* 0x65 */
	KEY_L,               /* 0x66 */
	KEY_O,               /* 0x67 */
	KEY_SLASH,           /* 0x68 */
	KEY_OPEN_BRACKET,    /* 0x69 */
	KEY_NULL,            /* 0x6a */
	KEY_QUOTE,           /* 0x6b */
	KEY_MINUS,           /* 0x6c */
	KEY_0,               /* 0x6d */
	KEY_SEMICOLON,       /* 0x6e */
	KEY_P,               /* 0x6f */
	KEY_NULL,            /* 0x70 */
	KEY_CLOSE_BRACKET,   /* 0x71 */
	KEY_COMMA,           /* 0x72 */
	KEY_NULL,            /* 0x73 */
	KEY_EQUALS,          /* 0x74 */
	KEY_8,               /* 0x75 */
	KEY_K,               /* 0x76 */
	KEY_I,               /* 0x77 */
	KEY_N,               /* 0x78 */
	KEY_Y,               /* 0x79 */
	KEY_M,               /* 0x7a */
	KEY_H,               /* 0x7b */
	KEY_6,               /* 0x7c */
	KEY_7,               /* 0x7d */
	KEY_J,               /* 0x7e */
	KEY_U,               /* 0x7f */
};

#ifdef __cplusplus
}
#endif

#endif
