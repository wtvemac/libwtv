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
			? KY_RELEASED(senjinkb_scancode_table[scancode_index & (SENJINKB_SCANCODE_COUNT - 1)]) \
			: KY_PRESSED(senjinkb_scancode_table[scancode_index & (SENJINKB_SCANCODE_COUNT - 1)]) \
	)

const int16_t senjinkb_scancode_table[SENJINKB_SCANCODE_COUNT] =
{
	KY_NULL,            /* 0x00 */
	KY_NULL,            /* 0x01 */
	KY_NULL,            /* 0x02 */
	KY_NULL,            /* 0x03 */
	KY_BACKTICK,        /* 0x04 */
	KY_WTV_VIEW,        /* 0x05 */
	KY_NULL,            /* 0x06 */
	KY_NULL,            /* 0x07 */
	KY_NULL,            /* 0x08 */
	KY_CAPS_LOCK,       /* 0x09 */
	KY_X,               /* 0x0a */
	KY_F5,              /* 0x0b */
	KY_F1,              /* 0x0c */
	KY_2,               /* 0x0d */
	KY_S,               /* 0x0e */
	KY_W,               /* 0x0f */
	KY_NULL,            /* 0x10 */
	KY_F3,              /* 0x11 */
	KY_C,               /* 0x12 */
	KY_F4,              /* 0x13 */
	KY_F2,              /* 0x14 */
	KY_3,               /* 0x15 */
	KY_D,               /* 0x16 */
	KY_E,               /* 0x17 */
	KY_ALTL,            /* 0x18 */
	KY_TAB,             /* 0x19 */
	KY_Z,               /* 0x1a */
	KY_POWER,           /* 0x1b */
	KY_CMD,             /* 0x1c */
	KY_1,               /* 0x1d */
	KY_A,               /* 0x1e */
	KY_Q,               /* 0x1f */
	KY_B,               /* 0x20 */
	KY_T,               /* 0x21 */
	KY_V,               /* 0x22 */
	KY_G,               /* 0x23 */
	KY_5,               /* 0x24 */
	KY_4,               /* 0x25 */
	KY_F,               /* 0x26 */
	KY_R,               /* 0x27 */
	KY_ALTR,            /* 0x28 */
	KY_ESC,             /* 0x29 */
	KY_NULL,            /* 0x2a */
	KY_CTRLL,           /* 0x2b */
	KY_NULL,            /* 0x2c */
	KY_WTV_GOTO,        /* 0x2d */
	KY_NULL,            /* 0x2e */
	KY_WTV_SAVE,        /* 0x2f */
	KY_NULL,            /* 0x30 */
	KY_SHIFTL,          /* 0x31 */
	KY_SHIFTR,          /* 0x32 */
	KY_NULL,            /* 0x33 */
	KY_NULL,            /* 0x34 */
	KY_NULL,            /* 0x35 */
	KY_MEDIA_CHAN_DOWN, /* 0x36 */
	KY_MEDIA_CHAN_UP,   /* 0x37 */
	KY_SPACE,           /* 0x38 */
	KY_BACKSPACE,       /* 0x39 */
	KY_RETURN,          /* 0x3a */
	KY_F6,              /* 0x3b */
	KY_WTV_HOME,        /* 0x3c */ /* aka F9 */
	KY_WTV_SEARCH,      /* 0x3d */ /* aka F10 */
	KY_BACKSLASH,       /* 0x3e */
	KY_NULL,            /* 0x3f */
	KY_NULL,            /* 0x40 */
	KY_NULL,            /* 0x41 */
	KY_NULL,            /* 0x42 */
	KY_NULL,            /* 0x43 */
	KY_NULL,            /* 0x44 */
	KY_NULL,            /* 0x45 */
	KY_FN,              /* 0x46 */
	KY_NULL,            /* 0x47 */
	KY_LEFT,            /* 0x48 */
	KY_NULL,            /* 0x49 */
	KY_WTV_SEND,        /* 0x4a */
	KY_UP,              /* 0x4b */
	KY_WTV_EDIT,        /* 0x4c */ /* aka Home */
	KY_NULL,            /* 0x4d */
	KY_WTV_BACK,        /* 0x4e */
	KY_NULL,            /* 0x4f */
	KY_DOWN,            /* 0x50 */
	KY_NULL,            /* 0x51 */
	KY_WTV_INFO,        /* 0x52 */
	KY_NULL,            /* 0x53 */
	KY_WTV_OPTION,      /* 0x54 */
	KY_WTV_MAIL,        /* 0x55 */ /* aka F11 */
	KY_PAGE_UP,         /* 0x56 */ /* aka Scroll Up */
	KY_NULL,            /* 0x57 */
	KY_RIGHT,           /* 0x58 */
	KY_NULL,            /* 0x59 */
	KY_NULL,            /* 0x5a */
	KY_PAGE_DOWN,       /* 0x5b */ /* aka Scroll Down */
	KY_WTV_RECENT,      /* 0x5c */
	KY_WTV_FIND,        /* 0x5d */ /* aka F12 */
	KY_NULL,            /* 0x5e */
	KY_NULL,            /* 0x5f */
	KY_NULL,            /* 0x60 */
	KY_F7,              /* 0x61 */
	KY_PERIOD,          /* 0x62 */
	KY_NULL,            /* 0x63 */
	KY_WTV_FAVORITES,   /* 0x64 */ /* aka F8 */
	KY_9,               /* 0x65 */
	KY_L,               /* 0x66 */
	KY_O,               /* 0x67 */
	KY_SLASH,           /* 0x68 */
	KY_OPEN_BRACKET,    /* 0x69 */
	KY_NULL,            /* 0x6a */
	KY_QUOTE,           /* 0x6b */
	KY_MINUS,           /* 0x6c */
	KY_0,               /* 0x6d */
	KY_SEMICOLON,       /* 0x6e */
	KY_P,               /* 0x6f */
	KY_NULL,            /* 0x70 */
	KY_CLOSE_BRACKET,   /* 0x71 */
	KY_COMMA,           /* 0x72 */
	KY_NULL,            /* 0x73 */
	KY_EQUALS,          /* 0x74 */
	KY_8,               /* 0x75 */
	KY_K,               /* 0x76 */
	KY_I,               /* 0x77 */
	KY_N,               /* 0x78 */
	KY_Y,               /* 0x79 */
	KY_M,               /* 0x7a */
	KY_H,               /* 0x7b */
	KY_6,               /* 0x7c */
	KY_7,               /* 0x7d */
	KY_J,               /* 0x7e */
	KY_U,               /* 0x7f */
};

#ifdef __cplusplus
}
#endif

#endif
