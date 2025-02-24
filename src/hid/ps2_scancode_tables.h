#ifndef __LIBWTV_HID_PS2_SCANCODE_TABLES_H
#define __LIBWTV_HID_PS2_SCANCODE_TABLES_H

#include <stdint.h>
#include <stddef.h>
#include "hid/event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PS2_IS_MOUSE                             0x02
#define PS2_IDENTITY_IS_UNKNOWN                  0xff
#define PS2_IDENTITY_IS_MF2_KEYBOARD             0x01
#define PS2_IDENTITY_IS_SHORT_KEYBOARD           0x05
#define PS2_IDENTITY_IS_HOST_KEYBOARD            0x09
#define PS2_IDENTITY_IS_122KEY_KEYBOARD          0x0d
#define PS2_IDENTITY_IS_JAPG_KEYBOARD            0x11
#define PS2_IDENTITY_IS_JAPP_KEYBOARD            0x15
#define PS2_IDENTITY_IS_JAPA_KEYBOARD            0x19
#define PS2_IDENTITY_IS_SUN_KEYBOARD             0x1d
#define PS2_IDENTITY_IS_MOUSE                    0x02
#define PS2_IDENTITY_IS_WHEEL_MOUSE              0x06
#define PS2_IDENTITY_IS_5KEY_MOUSE               0x0a

#define PS2_CODE_UNKNOWN                         0xff
#define PS2_CODE_CONTINUE                        0xfe
#define PS2_CODES_END                            0xff

#define PS2_TERMINATE_SCANCODE_TABLE             { PS2_CODES_END, PS2_CODES_END, NULL }

//typedef struct __attribute__((packed)) scancode_s
typedef struct scancode_s
{
	uint8_t input_code;
	int16_t result_code;
	struct scancode_s* next_code;
} scancode_t;

#define PS2_ADD_SCANCODE(icode, rcode) \
	{  \
		.input_code = icode, \
		.result_code = rcode, \
		.next_code = NULL \
	}

#define PS2_ADD_SCANCODE_TABLE(icode, ...) \
	{  \
		.input_code = icode, \
		.result_code = PS2_CODE_CONTINUE, \
		.next_code = (scancode_t[]) __VA_ARGS__ \
	}

#define IS_SCANCODE_TABLE_END(table, index) \
	(table[index].input_code == PS2_CODES_END && table[index].result_code == PS2_CODES_END)

scancode_t keyboard_identiy[] =
{
	PS2_ADD_SCANCODE(0x00, PS2_IDENTITY_IS_MOUSE),
	PS2_ADD_SCANCODE(0x03, PS2_IDENTITY_IS_WHEEL_MOUSE),
	PS2_ADD_SCANCODE(0x04, PS2_IDENTITY_IS_5KEY_MOUSE),
	PS2_ADD_SCANCODE_TABLE(0xab, {
		PS2_ADD_SCANCODE(0x83, PS2_IDENTITY_IS_MF2_KEYBOARD),
		PS2_ADD_SCANCODE(0xc1, PS2_IDENTITY_IS_MF2_KEYBOARD),
		PS2_ADD_SCANCODE(0x84, PS2_IDENTITY_IS_SHORT_KEYBOARD),
		PS2_ADD_SCANCODE(0x85, PS2_IDENTITY_IS_HOST_KEYBOARD),
		PS2_ADD_SCANCODE(0x86, PS2_IDENTITY_IS_122KEY_KEYBOARD),
		PS2_ADD_SCANCODE(0x90, PS2_IDENTITY_IS_JAPG_KEYBOARD),
		PS2_ADD_SCANCODE(0x91, PS2_IDENTITY_IS_JAPP_KEYBOARD),
		PS2_ADD_SCANCODE(0x92, PS2_IDENTITY_IS_JAPA_KEYBOARD),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_ADD_SCANCODE_TABLE(0xac, {
		PS2_ADD_SCANCODE(0xa1, PS2_IDENTITY_IS_SUN_KEYBOARD),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_TERMINATE_SCANCODE_TABLE
};

scancode_t scancode_set1[] =
{
	PS2_ADD_SCANCODE(0x01, KY_PRESSED(KY_ESC)),
	PS2_ADD_SCANCODE(0x02, KY_PRESSED(KY_1)),
	PS2_ADD_SCANCODE(0x03, KY_PRESSED(KY_2)),
	PS2_ADD_SCANCODE(0x04, KY_PRESSED(KY_3)),
	PS2_ADD_SCANCODE(0x05, KY_PRESSED(KY_4)),
	PS2_ADD_SCANCODE(0x06, KY_PRESSED(KY_5)),
	PS2_ADD_SCANCODE(0x07, KY_PRESSED(KY_6)),
	PS2_ADD_SCANCODE(0x08, KY_PRESSED(KY_7)),
	PS2_ADD_SCANCODE(0x09, KY_PRESSED(KY_8)),
	PS2_ADD_SCANCODE(0x0a, KY_PRESSED(KY_9)),
	PS2_ADD_SCANCODE(0x0b, KY_PRESSED(KY_0)),
	PS2_ADD_SCANCODE(0x0c, KY_PRESSED(KY_MINUS)),
	PS2_ADD_SCANCODE(0x0d, KY_PRESSED(KY_EQUALS)),
	PS2_ADD_SCANCODE(0x0e, KY_PRESSED(KY_BACKSPACE)),
	PS2_ADD_SCANCODE(0x0f, KY_PRESSED(KY_TAB)),
	PS2_ADD_SCANCODE(0x10, KY_PRESSED(KY_Q)),
	PS2_ADD_SCANCODE(0x11, KY_PRESSED(KY_W)),
	PS2_ADD_SCANCODE(0x12, KY_PRESSED(KY_E)),
	PS2_ADD_SCANCODE(0x13, KY_PRESSED(KY_R)),
	PS2_ADD_SCANCODE(0x14, KY_PRESSED(KY_T)),
	PS2_ADD_SCANCODE(0x15, KY_PRESSED(KY_Y)),
	PS2_ADD_SCANCODE(0x16, KY_PRESSED(KY_U)),
	PS2_ADD_SCANCODE(0x17, KY_PRESSED(KY_I)),
	PS2_ADD_SCANCODE(0x18, KY_PRESSED(KY_O)),
	PS2_ADD_SCANCODE(0x19, KY_PRESSED(KY_P)),
	PS2_ADD_SCANCODE(0x1a, KY_PRESSED(KY_OPEN_BRACKET)),
	PS2_ADD_SCANCODE(0x1b, KY_PRESSED(KY_CLOSE_BRACKET)),
	PS2_ADD_SCANCODE(0x1c, KY_PRESSED(KY_ENTER)),
	PS2_ADD_SCANCODE(0x1d, KY_PRESSED(KY_CTRLL)),
	PS2_ADD_SCANCODE(0x1e, KY_PRESSED(KY_A)),
	PS2_ADD_SCANCODE(0x1f, KY_PRESSED(KY_S)),
	PS2_ADD_SCANCODE(0x20, KY_PRESSED(KY_D)),
	PS2_ADD_SCANCODE(0x21, KY_PRESSED(KY_F)),
	PS2_ADD_SCANCODE(0x22, KY_PRESSED(KY_G)),
	PS2_ADD_SCANCODE(0x23, KY_PRESSED(KY_H)),
	PS2_ADD_SCANCODE(0x24, KY_PRESSED(KY_J)),
	PS2_ADD_SCANCODE(0x25, KY_PRESSED(KY_K)),
	PS2_ADD_SCANCODE(0x26, KY_PRESSED(KY_L)),
	PS2_ADD_SCANCODE(0x27, KY_PRESSED(KY_SEMICOLON)),
	PS2_ADD_SCANCODE(0x28, KY_PRESSED(KY_QUOTE)),
	PS2_ADD_SCANCODE(0x29, KY_PRESSED(KY_BACKTICK)),
	PS2_ADD_SCANCODE(0x2a, KY_PRESSED(KY_SHIFTL)),
	PS2_ADD_SCANCODE(0x2b, KY_PRESSED(KY_BACKSLASH)),
	PS2_ADD_SCANCODE(0x2c, KY_PRESSED(KY_Z)),
	PS2_ADD_SCANCODE(0x2d, KY_PRESSED(KY_X)),
	PS2_ADD_SCANCODE(0x2e, KY_PRESSED(KY_C)),
	PS2_ADD_SCANCODE(0x2f, KY_PRESSED(KY_V)),
	PS2_ADD_SCANCODE(0x30, KY_PRESSED(KY_B)),
	PS2_ADD_SCANCODE(0x31, KY_PRESSED(KY_N)),
	PS2_ADD_SCANCODE(0x32, KY_PRESSED(KY_M)),
	PS2_ADD_SCANCODE(0x33, KY_PRESSED(KY_COMMA)),
	PS2_ADD_SCANCODE(0x34, KY_PRESSED(KY_PERIOD)),
	PS2_ADD_SCANCODE(0x35, KY_PRESSED(KY_SLASH)),
	PS2_ADD_SCANCODE(0x36, KY_PRESSED(KY_SHIFTR)),
	PS2_ADD_SCANCODE(0x37, KY_PRESSED(KY_NPADMUL)),
	PS2_ADD_SCANCODE(0x38, KY_PRESSED(KY_ALTL)),
	PS2_ADD_SCANCODE(0x39, KY_PRESSED(KY_SPACE)),
	PS2_ADD_SCANCODE(0x3a, KY_PRESSED(KY_CAPS_LOCK)),
	PS2_ADD_SCANCODE(0x3b, KY_PRESSED(KY_F1)),
	PS2_ADD_SCANCODE(0x3c, KY_PRESSED(KY_F2)),
	PS2_ADD_SCANCODE(0x3d, KY_PRESSED(KY_F3)),
	PS2_ADD_SCANCODE(0x3e, KY_PRESSED(KY_F4)),
	PS2_ADD_SCANCODE(0x3f, KY_PRESSED(KY_F5)),
	PS2_ADD_SCANCODE(0x40, KY_PRESSED(KY_F6)),
	PS2_ADD_SCANCODE(0x41, KY_PRESSED(KY_F7)),
	PS2_ADD_SCANCODE(0x42, KY_PRESSED(KY_F8)),
	PS2_ADD_SCANCODE(0x43, KY_PRESSED(KY_F9)),
	PS2_ADD_SCANCODE(0x44, KY_PRESSED(KY_F10)),
	PS2_ADD_SCANCODE(0x45, KY_PRESSED(KY_NUMB_LOCK)),
	PS2_ADD_SCANCODE(0x46, KY_PRESSED(KY_SCRL_LOCK)),
	PS2_ADD_SCANCODE(0x47, KY_PRESSED(KY_NPAD7)),
	PS2_ADD_SCANCODE(0x48, KY_PRESSED(KY_NPAD8)),
	PS2_ADD_SCANCODE(0x49, KY_PRESSED(KY_NPAD9)),
	PS2_ADD_SCANCODE(0x4a, KY_PRESSED(KY_NPADSUB)),
	PS2_ADD_SCANCODE(0x4b, KY_PRESSED(KY_NPAD4)),
	PS2_ADD_SCANCODE(0x4c, KY_PRESSED(KY_NPAD5)),
	PS2_ADD_SCANCODE(0x4d, KY_PRESSED(KY_NPAD6)),
	PS2_ADD_SCANCODE(0x4e, KY_PRESSED(KY_NPADADD)),
	PS2_ADD_SCANCODE(0x4f, KY_PRESSED(KY_NPAD1)),
	PS2_ADD_SCANCODE(0x50, KY_PRESSED(KY_NPAD2)),
	PS2_ADD_SCANCODE(0x51, KY_PRESSED(KY_NPAD3)),
	PS2_ADD_SCANCODE(0x52, KY_PRESSED(KY_NPAD0)),
	PS2_ADD_SCANCODE(0x53, KY_PRESSED(KY_NPADDEC)),
	PS2_ADD_SCANCODE(0x57, KY_PRESSED(KY_F11)),
	PS2_ADD_SCANCODE(0x58, KY_PRESSED(KY_F12)),
	PS2_ADD_SCANCODE(0x81, KY_RELEASED(KY_ESC)),
	PS2_ADD_SCANCODE(0x82, KY_RELEASED(KY_1)),
	PS2_ADD_SCANCODE(0x83, KY_RELEASED(KY_2)),
	PS2_ADD_SCANCODE(0x84, KY_RELEASED(KY_3)),
	PS2_ADD_SCANCODE(0x85, KY_RELEASED(KY_4)),
	PS2_ADD_SCANCODE(0x86, KY_RELEASED(KY_5)),
	PS2_ADD_SCANCODE(0x87, KY_RELEASED(KY_6)),
	PS2_ADD_SCANCODE(0x88, KY_RELEASED(KY_7)),
	PS2_ADD_SCANCODE(0x89, KY_RELEASED(KY_8)),
	PS2_ADD_SCANCODE(0x8a, KY_RELEASED(KY_9)),
	PS2_ADD_SCANCODE(0x8b, KY_RELEASED(KY_0)),
	PS2_ADD_SCANCODE(0x8c, KY_RELEASED(KY_MINUS)),
	PS2_ADD_SCANCODE(0x8d, KY_RELEASED(KY_EQUALS)),
	PS2_ADD_SCANCODE(0x8e, KY_RELEASED(KY_BACKSPACE)),
	PS2_ADD_SCANCODE(0x8f, KY_RELEASED(KY_TAB)),
	PS2_ADD_SCANCODE(0x90, KY_RELEASED(KY_Q)),
	PS2_ADD_SCANCODE(0x91, KY_RELEASED(KY_W)),
	PS2_ADD_SCANCODE(0x92, KY_RELEASED(KY_E)),
	PS2_ADD_SCANCODE(0x93, KY_RELEASED(KY_R)),
	PS2_ADD_SCANCODE(0x94, KY_RELEASED(KY_T)),
	PS2_ADD_SCANCODE(0x95, KY_RELEASED(KY_Y)),
	PS2_ADD_SCANCODE(0x96, KY_RELEASED(KY_U)),
	PS2_ADD_SCANCODE(0x97, KY_RELEASED(KY_I)),
	PS2_ADD_SCANCODE(0x98, KY_RELEASED(KY_O)),
	PS2_ADD_SCANCODE(0x99, KY_RELEASED(KY_P)),
	PS2_ADD_SCANCODE(0x9a, KY_RELEASED(KY_OPEN_BRACKET)),
	PS2_ADD_SCANCODE(0x9b, KY_RELEASED(KY_CLOSE_BRACKET)),
	PS2_ADD_SCANCODE(0x9c, KY_RELEASED(KY_ENTER)),
	PS2_ADD_SCANCODE(0x9d, KY_RELEASED(KY_CTRLL)),
	PS2_ADD_SCANCODE(0x9e, KY_RELEASED(KY_A)),
	PS2_ADD_SCANCODE(0x9f, KY_RELEASED(KY_S)),
	PS2_ADD_SCANCODE(0xa0, KY_RELEASED(KY_D)),
	PS2_ADD_SCANCODE(0xa1, KY_RELEASED(KY_F)),
	PS2_ADD_SCANCODE(0xa2, KY_RELEASED(KY_G)),
	PS2_ADD_SCANCODE(0xa3, KY_RELEASED(KY_H)),
	PS2_ADD_SCANCODE(0xa4, KY_RELEASED(KY_J)),
	PS2_ADD_SCANCODE(0xa5, KY_RELEASED(KY_K)),
	PS2_ADD_SCANCODE(0xa6, KY_RELEASED(KY_L)),
	PS2_ADD_SCANCODE(0xa7, KY_RELEASED(KY_SEMICOLON)),
	PS2_ADD_SCANCODE(0xa8, KY_RELEASED(KY_QUOTE)),
	PS2_ADD_SCANCODE(0xa9, KY_RELEASED(KY_BACKTICK)),
	PS2_ADD_SCANCODE(0xaa, KY_RELEASED(KY_SHIFTL)),
	PS2_ADD_SCANCODE(0xab, KY_RELEASED(KY_BACKSLASH)),
	PS2_ADD_SCANCODE(0xac, KY_RELEASED(KY_Z)),
	PS2_ADD_SCANCODE(0xad, KY_RELEASED(KY_X)),
	PS2_ADD_SCANCODE(0xae, KY_RELEASED(KY_C)),
	PS2_ADD_SCANCODE(0xaf, KY_RELEASED(KY_V)),
	PS2_ADD_SCANCODE(0xb0, KY_RELEASED(KY_B)),
	PS2_ADD_SCANCODE(0xb1, KY_RELEASED(KY_N)),
	PS2_ADD_SCANCODE(0xb2, KY_RELEASED(KY_M)),
	PS2_ADD_SCANCODE(0xb3, KY_RELEASED(KY_COMMA)),
	PS2_ADD_SCANCODE(0xb4, KY_RELEASED(KY_PERIOD)),
	PS2_ADD_SCANCODE(0xb5, KY_RELEASED(KY_SLASH)),
	PS2_ADD_SCANCODE(0xb6, KY_RELEASED(KY_SHIFTR)),
	PS2_ADD_SCANCODE(0xb7, KY_RELEASED(KY_NPADMUL)),
	PS2_ADD_SCANCODE(0xb8, KY_RELEASED(KY_ALTL)),
	PS2_ADD_SCANCODE(0xb9, KY_RELEASED(KY_SPACE)),
	PS2_ADD_SCANCODE(0xba, KY_RELEASED(KY_CAPS_LOCK)),
	PS2_ADD_SCANCODE(0xbb, KY_RELEASED(KY_F1)),
	PS2_ADD_SCANCODE(0xbc, KY_RELEASED(KY_F3)),
	PS2_ADD_SCANCODE(0xbd, KY_RELEASED(KY_F3)),
	PS2_ADD_SCANCODE(0xbe, KY_RELEASED(KY_F4)),
	PS2_ADD_SCANCODE(0xbf, KY_RELEASED(KY_F5)),
	PS2_ADD_SCANCODE(0xc0, KY_RELEASED(KY_F6)),
	PS2_ADD_SCANCODE(0xc1, KY_RELEASED(KY_F7)),
	PS2_ADD_SCANCODE(0xc2, KY_RELEASED(KY_F8)),
	PS2_ADD_SCANCODE(0xc3, KY_RELEASED(KY_F9)),
	PS2_ADD_SCANCODE(0xc4, KY_RELEASED(KY_F10)),
	PS2_ADD_SCANCODE(0xc5, KY_RELEASED(KY_NUMB_LOCK)),
	PS2_ADD_SCANCODE(0xc6, KY_RELEASED(KY_SCRL_LOCK)),
	PS2_ADD_SCANCODE(0xc7, KY_RELEASED(KY_NPAD7)),
	PS2_ADD_SCANCODE(0xc8, KY_RELEASED(KY_NPAD8)),
	PS2_ADD_SCANCODE(0xc9, KY_RELEASED(KY_NPAD9)),
	PS2_ADD_SCANCODE(0xca, KY_RELEASED(KY_NPADSUB)),
	PS2_ADD_SCANCODE(0xcb, KY_RELEASED(KY_NPAD4)),
	PS2_ADD_SCANCODE(0xcc, KY_RELEASED(KY_NPAD5)),
	PS2_ADD_SCANCODE(0xcd, KY_RELEASED(KY_NPAD6)),
	PS2_ADD_SCANCODE(0xce, KY_RELEASED(KY_NPADADD)),
	PS2_ADD_SCANCODE(0xcf, KY_RELEASED(KY_NPAD1)),
	PS2_ADD_SCANCODE(0xd0, KY_RELEASED(KY_NPAD2)),
	PS2_ADD_SCANCODE(0xd1, KY_RELEASED(KY_NPAD3)),
	PS2_ADD_SCANCODE(0xd2, KY_RELEASED(KY_NPAD0)),
	PS2_ADD_SCANCODE(0xd3, KY_RELEASED(KY_NPADDEC)),
	PS2_ADD_SCANCODE(0xd7, KY_RELEASED(KY_F11)),
	PS2_ADD_SCANCODE(0xd8, KY_RELEASED(KY_F12)),
	PS2_ADD_SCANCODE_TABLE(0xe0, {
		PS2_ADD_SCANCODE(0x10, KY_PRESSED(KY_MEDIA_PREV_TRACK)),
		PS2_ADD_SCANCODE(0x19, KY_PRESSED(KY_MEDIA_NEXT_TRACK)),
		PS2_ADD_SCANCODE(0x1c, KY_PRESSED(KY_NPADENT)),
		PS2_ADD_SCANCODE(0x1d, KY_PRESSED(KY_CTRLR)),
		PS2_ADD_SCANCODE(0x20, KY_PRESSED(KY_MEDIA_MUTE)),
		PS2_ADD_SCANCODE(0x21, KY_PRESSED(KY_LAUNCH_CALCULATOR)),
		PS2_ADD_SCANCODE(0x22, KY_PRESSED(KY_MEDIA_PLAYPAUSE)),
		PS2_ADD_SCANCODE(0x24, KY_PRESSED(KY_MEDIA_STOP)),
		PS2_ADD_SCANCODE(0x2e, KY_PRESSED(KY_MEDIA_VOL_DOWN)),
		PS2_ADD_SCANCODE(0x30, KY_PRESSED(KY_MEDIA_VOL_UP)),
		PS2_ADD_SCANCODE(0x32, KY_PRESSED(KY_BROWSER_HOME)),
		PS2_ADD_SCANCODE(0x35, KY_PRESSED(KY_NPADDIV)),
		PS2_ADD_SCANCODE(0x38, KY_PRESSED(KY_ALTGR)),
		PS2_ADD_SCANCODE(0x47, KY_PRESSED(KY_HOME)),
		PS2_ADD_SCANCODE(0x48, KY_PRESSED(KY_UP)),
		PS2_ADD_SCANCODE(0x49, KY_PRESSED(KY_PAGE_UP)),
		PS2_ADD_SCANCODE(0x4b, KY_PRESSED(KY_LEFT)),
		PS2_ADD_SCANCODE(0x4d, KY_PRESSED(KY_RIGHT)),
		PS2_ADD_SCANCODE(0x4f, KY_PRESSED(KY_END)),
		PS2_ADD_SCANCODE(0x50, KY_PRESSED(KY_DOWN)),
		PS2_ADD_SCANCODE(0x51, KY_PRESSED(KY_PAGE_DOWN)),
		PS2_ADD_SCANCODE(0x52, KY_PRESSED(KY_INSERT)),
		PS2_ADD_SCANCODE(0x53, KY_PRESSED(KY_DELETE)),
		PS2_ADD_SCANCODE(0x5b, KY_PRESSED(KY_METAL)),
		PS2_ADD_SCANCODE(0x5c, KY_PRESSED(KY_METAR)),
		PS2_ADD_SCANCODE(0x5d, KY_PRESSED(KY_LAUNCH_APPS)),
		PS2_ADD_SCANCODE(0x5e, KY_PRESSED(KY_POWER)),
		PS2_ADD_SCANCODE(0x5f, KY_PRESSED(KY_SLEEP)),
		PS2_ADD_SCANCODE(0x63, KY_PRESSED(KY_WAKE)),
		PS2_ADD_SCANCODE(0x65, KY_PRESSED(KY_BROWSER_SEARCH)),
		PS2_ADD_SCANCODE(0x66, KY_PRESSED(KY_WTV_FAVORITES)),
		PS2_ADD_SCANCODE(0x67, KY_PRESSED(KY_BROWSER_REFRESH)),
		PS2_ADD_SCANCODE(0x68, KY_PRESSED(KY_BROWSER_STOP)),
		PS2_ADD_SCANCODE(0x69, KY_PRESSED(KY_BROWSER_FORWARD)),
		PS2_ADD_SCANCODE(0x6a, KY_PRESSED(KY_BROWSER_BACK)),
		PS2_ADD_SCANCODE(0x6b, KY_PRESSED(KY_LAUNCH_MYCOMPUTER)),
		PS2_ADD_SCANCODE(0x6c, KY_PRESSED(KY_LAUNCH_EMAIL)),
		PS2_ADD_SCANCODE(0x6d, KY_PRESSED(KY_LAUNCH_MEDIA_PLAYER)),
		PS2_ADD_SCANCODE(0x90, KY_RELEASED(KY_MEDIA_PREV_TRACK)),
		PS2_ADD_SCANCODE(0x99, KY_RELEASED(KY_MEDIA_NEXT_TRACK)),
		PS2_ADD_SCANCODE(0x9c, KY_RELEASED(KY_NPADENT)),
		PS2_ADD_SCANCODE(0x9d, KY_RELEASED(KY_CTRLR)),
		PS2_ADD_SCANCODE(0xa0, KY_RELEASED(KY_MEDIA_MUTE)),
		PS2_ADD_SCANCODE(0xa1, KY_RELEASED(KY_LAUNCH_CALCULATOR)),
		PS2_ADD_SCANCODE(0xa2, KY_RELEASED(KY_MEDIA_PLAYPAUSE)),
		PS2_ADD_SCANCODE(0xa4, KY_RELEASED(KY_MEDIA_STOP)),
		PS2_ADD_SCANCODE(0xae, KY_RELEASED(KY_MEDIA_VOL_DOWN)),
		PS2_ADD_SCANCODE(0xb0, KY_RELEASED(KY_MEDIA_VOL_UP)),
		PS2_ADD_SCANCODE(0xb2, KY_RELEASED(KY_BROWSER_HOME)),
		PS2_ADD_SCANCODE(0xb5, KY_RELEASED(KY_NPADDIV)),
		PS2_ADD_SCANCODE(0xb8, KY_RELEASED(KY_ALTGR)),
		PS2_ADD_SCANCODE(0xc7, KY_RELEASED(KY_HOME)),
		PS2_ADD_SCANCODE(0xc8, KY_RELEASED(KY_UP)),
		PS2_ADD_SCANCODE(0xc9, KY_RELEASED(KY_PAGE_UP)),
		PS2_ADD_SCANCODE(0xcb, KY_RELEASED(KY_LEFT)),
		PS2_ADD_SCANCODE(0xcd, KY_RELEASED(KY_RIGHT)),
		PS2_ADD_SCANCODE(0xcf, KY_RELEASED(KY_END)),
		PS2_ADD_SCANCODE(0xd0, KY_RELEASED(KY_DOWN)),
		PS2_ADD_SCANCODE(0xd1, KY_RELEASED(KY_PAGE_DOWN)),
		PS2_ADD_SCANCODE(0xd2, KY_RELEASED(KY_INSERT)),
		PS2_ADD_SCANCODE(0xd3, KY_RELEASED(KY_DELETE)),
		PS2_ADD_SCANCODE(0xdb, KY_RELEASED(KY_METAL)),
		PS2_ADD_SCANCODE(0xdc, KY_RELEASED(KY_METAR)),
		PS2_ADD_SCANCODE(0xdd, KY_RELEASED(KY_LAUNCH_APPS)),
		PS2_ADD_SCANCODE(0xde, KY_RELEASED(KY_POWER)),
		PS2_ADD_SCANCODE(0xdf, KY_RELEASED(KY_SLEEP)),
		PS2_ADD_SCANCODE(0xe3, KY_RELEASED(KY_WAKE)),
		PS2_ADD_SCANCODE(0xe5, KY_RELEASED(KY_BROWSER_SEARCH)),
		PS2_ADD_SCANCODE(0xe6, KY_RELEASED(KY_BROWSER_FAVORITES)),
		PS2_ADD_SCANCODE(0xe7, KY_RELEASED(KY_BROWSER_REFRESH)),
		PS2_ADD_SCANCODE(0xe8, KY_RELEASED(KY_BROWSER_STOP)),
		PS2_ADD_SCANCODE(0xe9, KY_RELEASED(KY_BROWSER_FORWARD)),
		PS2_ADD_SCANCODE(0xea, KY_RELEASED(KY_BROWSER_BACK)),
		PS2_ADD_SCANCODE(0xeb, KY_RELEASED(KY_LAUNCH_MYCOMPUTER)),
		PS2_ADD_SCANCODE(0xec, KY_RELEASED(KY_LAUNCH_EMAIL)),
		PS2_ADD_SCANCODE(0xed, KY_RELEASED(KY_LAUNCH_MEDIA_PLAYER)),
		PS2_ADD_SCANCODE_TABLE(0x2a, {
			PS2_ADD_SCANCODE(0x37, KY_PRESSED(KY_PRINT_SCREEN)),
			PS2_TERMINATE_SCANCODE_TABLE
		}),
		PS2_ADD_SCANCODE_TABLE(0xb7, {
			PS2_ADD_SCANCODE(0xaa, KY_RELEASED(KY_PRINT_SCREEN)),
			PS2_TERMINATE_SCANCODE_TABLE
		}),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_ADD_SCANCODE_TABLE(0xe1, {
		PS2_ADD_SCANCODE_TABLE(0x1d, {
			PS2_ADD_SCANCODE_TABLE(0x45, {
				PS2_ADD_SCANCODE_TABLE(0xe1, {
					PS2_ADD_SCANCODE_TABLE(0x9d, {
						PS2_ADD_SCANCODE(0xc5, KY_PRESSED(KY_PAUSE)),
						PS2_TERMINATE_SCANCODE_TABLE
					}),
					PS2_TERMINATE_SCANCODE_TABLE
				}),
				PS2_TERMINATE_SCANCODE_TABLE
			}),
			PS2_TERMINATE_SCANCODE_TABLE
		}),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_TERMINATE_SCANCODE_TABLE
};

/*
*/
scancode_t scancode_set2[] =
{
	PS2_ADD_SCANCODE(0x01, KY_PRESSED(KY_F9)),
	PS2_ADD_SCANCODE(0x03, KY_PRESSED(KY_F5)),
	PS2_ADD_SCANCODE(0x04, KY_PRESSED(KY_F3)),
	PS2_ADD_SCANCODE(0x05, KY_PRESSED(KY_F1)),
	PS2_ADD_SCANCODE(0x06, KY_PRESSED(KY_F2)),
	PS2_ADD_SCANCODE(0x07, KY_PRESSED(KY_F12)),
	PS2_ADD_SCANCODE(0x09, KY_PRESSED(KY_F10)),
	PS2_ADD_SCANCODE(0x0a, KY_PRESSED(KY_F8)),
	PS2_ADD_SCANCODE(0x0b, KY_PRESSED(KY_F6)),
	PS2_ADD_SCANCODE(0x0c, KY_PRESSED(KY_F4)),
	PS2_ADD_SCANCODE(0x0d, KY_PRESSED(KY_TAB)),
	PS2_ADD_SCANCODE(0x0e, KY_PRESSED(KY_BACKTICK)),
	PS2_ADD_SCANCODE(0x11, KY_PRESSED(KY_ALTL)),
	PS2_ADD_SCANCODE(0x12, KY_PRESSED(KY_SHIFTL)),
	PS2_ADD_SCANCODE(0x14, KY_PRESSED(KY_CTRLL)),
	PS2_ADD_SCANCODE(0x15, KY_PRESSED(KY_Q)),
	PS2_ADD_SCANCODE(0x16, KY_PRESSED(KY_1)),
	PS2_ADD_SCANCODE(0x1a, KY_PRESSED(KY_Z)),
	PS2_ADD_SCANCODE(0x1b, KY_PRESSED(KY_S)),
	PS2_ADD_SCANCODE(0x1c, KY_PRESSED(KY_A)),
	PS2_ADD_SCANCODE(0x1d, KY_PRESSED(KY_W)),
	PS2_ADD_SCANCODE(0x1e, KY_PRESSED(KY_2)),
	PS2_ADD_SCANCODE(0x21, KY_PRESSED(KY_C)),
	PS2_ADD_SCANCODE(0x22, KY_PRESSED(KY_X)),
	PS2_ADD_SCANCODE(0x23, KY_PRESSED(KY_D)),
	PS2_ADD_SCANCODE(0x24, KY_PRESSED(KY_E)),
	PS2_ADD_SCANCODE(0x25, KY_PRESSED(KY_4)),
	PS2_ADD_SCANCODE(0x26, KY_PRESSED(KY_3)),
	PS2_ADD_SCANCODE(0x29, KY_PRESSED(KY_SPACE)),
	PS2_ADD_SCANCODE(0x2a, KY_PRESSED(KY_V)),
	PS2_ADD_SCANCODE(0x2b, KY_PRESSED(KY_F)),
	PS2_ADD_SCANCODE(0x2c, KY_PRESSED(KY_T)),
	PS2_ADD_SCANCODE(0x2d, KY_PRESSED(KY_R)),
	PS2_ADD_SCANCODE(0x2e, KY_PRESSED(KY_5)),
	PS2_ADD_SCANCODE(0x31, KY_PRESSED(KY_N)),
	PS2_ADD_SCANCODE(0x32, KY_PRESSED(KY_B)),
	PS2_ADD_SCANCODE(0x33, KY_PRESSED(KY_H)),
	PS2_ADD_SCANCODE(0x34, KY_PRESSED(KY_G)),
	PS2_ADD_SCANCODE(0x35, KY_PRESSED(KY_Y)),
	PS2_ADD_SCANCODE(0x36, KY_PRESSED(KY_6)),
	PS2_ADD_SCANCODE(0x3a, KY_PRESSED(KY_M)),
	PS2_ADD_SCANCODE(0x3b, KY_PRESSED(KY_J)),
	PS2_ADD_SCANCODE(0x3c, KY_PRESSED(KY_U)),
	PS2_ADD_SCANCODE(0x3d, KY_PRESSED(KY_7)),
	PS2_ADD_SCANCODE(0x3e, KY_PRESSED(KY_8)),
	PS2_ADD_SCANCODE(0x41, KY_PRESSED(KY_COMMA)),
	PS2_ADD_SCANCODE(0x42, KY_PRESSED(KY_K)),
	PS2_ADD_SCANCODE(0x43, KY_PRESSED(KY_I)),
	PS2_ADD_SCANCODE(0x44, KY_PRESSED(KY_O)),
	PS2_ADD_SCANCODE(0x45, KY_PRESSED(KY_0)),
	PS2_ADD_SCANCODE(0x46, KY_PRESSED(KY_9)),
	PS2_ADD_SCANCODE(0x49, KY_PRESSED(KY_PERIOD)),
	PS2_ADD_SCANCODE(0x4a, KY_PRESSED(KY_SLASH)),
	PS2_ADD_SCANCODE(0x4b, KY_PRESSED(KY_L)),
	PS2_ADD_SCANCODE(0x4c, KY_PRESSED(KY_SEMICOLON)),
	PS2_ADD_SCANCODE(0x4d, KY_PRESSED(KY_P)),
	PS2_ADD_SCANCODE(0x4e, KY_PRESSED(KY_MINUS)),
	PS2_ADD_SCANCODE(0x52, KY_PRESSED(KY_QUOTE)),
	PS2_ADD_SCANCODE(0x54, KY_PRESSED(KY_OPEN_BRACKET)),
	PS2_ADD_SCANCODE(0x55, KY_PRESSED(KY_EQUALS)),
	PS2_ADD_SCANCODE(0x58, KY_PRESSED(KY_CAPS_LOCK)),
	PS2_ADD_SCANCODE(0x59, KY_PRESSED(KY_SHIFTR)),
	PS2_ADD_SCANCODE(0x5a, KY_PRESSED(KY_ENTER)),
	PS2_ADD_SCANCODE(0x5b, KY_PRESSED(KY_CLOSE_BRACKET)),
	PS2_ADD_SCANCODE(0x5d, KY_PRESSED(KY_BACKSLASH)),
	PS2_ADD_SCANCODE(0x66, KY_PRESSED(KY_BACKSPACE)),
	PS2_ADD_SCANCODE(0x69, KY_PRESSED(KY_NPAD1)),
	PS2_ADD_SCANCODE(0x6b, KY_PRESSED(KY_NPAD4)),
	PS2_ADD_SCANCODE(0x6c, KY_PRESSED(KY_NPAD7)),
	PS2_ADD_SCANCODE(0x70, KY_PRESSED(KY_NPAD0)),
	PS2_ADD_SCANCODE(0x71, KY_PRESSED(KY_NPADDEC)),
	PS2_ADD_SCANCODE(0x72, KY_PRESSED(KY_NPAD2)),
	PS2_ADD_SCANCODE(0x73, KY_PRESSED(KY_NPAD5)),
	PS2_ADD_SCANCODE(0x74, KY_PRESSED(KY_NPAD6)),
	PS2_ADD_SCANCODE(0x75, KY_PRESSED(KY_NPAD8)),
	PS2_ADD_SCANCODE(0x76, KY_PRESSED(KY_ESC)),
	PS2_ADD_SCANCODE(0x77, KY_PRESSED(KY_NUMB_LOCK)),
	PS2_ADD_SCANCODE(0x78, KY_PRESSED(KY_F11)),
	PS2_ADD_SCANCODE(0x79, KY_PRESSED(KY_NPADADD)),
	PS2_ADD_SCANCODE(0x7a, KY_PRESSED(KY_NPAD3)),
	PS2_ADD_SCANCODE(0x7b, KY_PRESSED(KY_NPADSUB)),
	PS2_ADD_SCANCODE(0x7c, KY_PRESSED(KY_NPADMUL)),
	PS2_ADD_SCANCODE(0x7d, KY_PRESSED(KY_NPAD9)),
	PS2_ADD_SCANCODE(0x7e, KY_PRESSED(KY_SCRL_LOCK)),
	PS2_ADD_SCANCODE(0x83, KY_PRESSED(KY_F7)),
	PS2_ADD_SCANCODE_TABLE(0xe0, {
		PS2_ADD_SCANCODE(0x10, KY_PRESSED(KY_BROWSER_SEARCH)),
		PS2_ADD_SCANCODE(0x11, KY_PRESSED(KY_ALTR)),
		PS2_ADD_SCANCODE(0x14, KY_PRESSED(KY_CTRLR)),
		PS2_ADD_SCANCODE(0x15, KY_PRESSED(KY_MEDIA_PREV_TRACK)),
		PS2_ADD_SCANCODE(0x18, KY_PRESSED(KY_BROWSER_FAVORITES)),
		PS2_ADD_SCANCODE(0x1f, KY_PRESSED(KY_METAL)),
		PS2_ADD_SCANCODE(0x20, KY_PRESSED(KY_BROWSER_REFRESH)),
		PS2_ADD_SCANCODE(0x21, KY_PRESSED(KY_MEDIA_VOL_DOWN)),
		PS2_ADD_SCANCODE(0x23, KY_PRESSED(KY_MEDIA_MUTE)),
		PS2_ADD_SCANCODE(0x27, KY_PRESSED(KY_METAR)),
		PS2_ADD_SCANCODE(0x28, KY_PRESSED(KY_BROWSER_STOP)),
		PS2_ADD_SCANCODE(0x2b, KY_PRESSED(KY_LAUNCH_CALCULATOR)),
		PS2_ADD_SCANCODE(0x2f, KY_PRESSED(KY_LAUNCH_APPS)),
		PS2_ADD_SCANCODE(0x30, KY_PRESSED(KY_BROWSER_FORWARD)),
		PS2_ADD_SCANCODE(0x32, KY_PRESSED(KY_MEDIA_VOL_UP)),
		PS2_ADD_SCANCODE(0x34, KY_PRESSED(KY_MEDIA_PLAYPAUSE)),
		PS2_ADD_SCANCODE(0x37, KY_PRESSED(KY_POWER)),
		PS2_ADD_SCANCODE(0x38, KY_PRESSED(KY_BROWSER_BACK)),
		PS2_ADD_SCANCODE(0x3a, KY_PRESSED(KY_BROWSER_HOME)),
		PS2_ADD_SCANCODE(0x3b, KY_PRESSED(KY_MEDIA_STOP)),
		PS2_ADD_SCANCODE(0x3f, KY_PRESSED(KY_SLEEP)),
		PS2_ADD_SCANCODE(0x40, KY_PRESSED(KY_LAUNCH_MYCOMPUTER)),
		PS2_ADD_SCANCODE(0x48, KY_PRESSED(KY_LAUNCH_EMAIL)),
		PS2_ADD_SCANCODE(0x4a, KY_PRESSED(KY_NPADDIV)),
		PS2_ADD_SCANCODE(0x4d, KY_PRESSED(KY_MEDIA_NEXT_TRACK)),
		PS2_ADD_SCANCODE(0x50, KY_PRESSED(KY_LAUNCH_MEDIA_PLAYER)),
		PS2_ADD_SCANCODE(0x5a, KY_PRESSED(KY_NPADENT)),
		PS2_ADD_SCANCODE(0x5e, KY_PRESSED(KY_WAKE)),
		PS2_ADD_SCANCODE(0x69, KY_PRESSED(KY_END)),
		PS2_ADD_SCANCODE(0x6b, KY_PRESSED(KY_LEFT)),
		PS2_ADD_SCANCODE(0x6c, KY_PRESSED(KY_HOME)),
		PS2_ADD_SCANCODE(0x70, KY_PRESSED(KY_INSERT)),
		PS2_ADD_SCANCODE(0x71, KY_PRESSED(KY_DELETE)),
		PS2_ADD_SCANCODE(0x72, KY_PRESSED(KY_DOWN)),
		PS2_ADD_SCANCODE(0x74, KY_PRESSED(KY_RIGHT)),
		PS2_ADD_SCANCODE(0x75, KY_PRESSED(KY_UP)),
		PS2_ADD_SCANCODE(0x7a, KY_PRESSED(KY_PAGE_DOWN)),
		PS2_ADD_SCANCODE(0x7d, KY_PRESSED(KY_PAGE_UP)),
		PS2_ADD_SCANCODE_TABLE(0x12, {
			PS2_ADD_SCANCODE_TABLE(0xe0, {
				PS2_ADD_SCANCODE(0x7c, KY_PRESSED(KY_PRINT_SCREEN)),
				PS2_TERMINATE_SCANCODE_TABLE
			}),
			PS2_TERMINATE_SCANCODE_TABLE
		}),
		PS2_ADD_SCANCODE_TABLE(0xf0, {
			PS2_ADD_SCANCODE(0x10, KY_RELEASED(KY_BROWSER_SEARCH)),
			PS2_ADD_SCANCODE(0x11, KY_RELEASED(KY_ALTR)),
			PS2_ADD_SCANCODE(0x14, KY_RELEASED(KY_CTRLR)),
			PS2_ADD_SCANCODE(0x15, KY_RELEASED(KY_MEDIA_PREV_TRACK)),
			PS2_ADD_SCANCODE(0x18, KY_RELEASED(KY_BROWSER_FAVORITES)),
			PS2_ADD_SCANCODE(0x1f, KY_RELEASED(KY_METAL)),
			PS2_ADD_SCANCODE(0x20, KY_RELEASED(KY_BROWSER_REFRESH)),
			PS2_ADD_SCANCODE(0x21, KY_RELEASED(KY_MEDIA_VOL_DOWN)),
			PS2_ADD_SCANCODE(0x23, KY_RELEASED(KY_MEDIA_MUTE)),
			PS2_ADD_SCANCODE(0x27, KY_RELEASED(KY_METAR)),
			PS2_ADD_SCANCODE(0x28, KY_RELEASED(KY_BROWSER_STOP)),
			PS2_ADD_SCANCODE(0x2b, KY_RELEASED(KY_LAUNCH_CALCULATOR)),
			PS2_ADD_SCANCODE(0x2f, KY_RELEASED(KY_LAUNCH_APPS)),
			PS2_ADD_SCANCODE(0x30, KY_RELEASED(KY_BROWSER_FORWARD)),
			PS2_ADD_SCANCODE(0x32, KY_RELEASED(KY_MEDIA_VOL_UP)),
			PS2_ADD_SCANCODE(0x34, KY_RELEASED(KY_MEDIA_PLAYPAUSE)),
			PS2_ADD_SCANCODE(0x37, KY_RELEASED(KY_POWER)),
			PS2_ADD_SCANCODE(0x38, KY_RELEASED(KY_BROWSER_BACK)),
			PS2_ADD_SCANCODE(0x3a, KY_RELEASED(KY_BROWSER_HOME)),
			PS2_ADD_SCANCODE(0x3b, KY_RELEASED(KY_MEDIA_STOP)),
			PS2_ADD_SCANCODE(0x3f, KY_RELEASED(KY_SLEEP)),
			PS2_ADD_SCANCODE(0x40, KY_RELEASED(KY_LAUNCH_MYCOMPUTER)),
			PS2_ADD_SCANCODE(0x48, KY_RELEASED(KY_LAUNCH_EMAIL)),
			PS2_ADD_SCANCODE(0x4a, KY_RELEASED(KY_NPADDIV)),
			PS2_ADD_SCANCODE(0x4d, KY_RELEASED(KY_MEDIA_NEXT_TRACK)),
			PS2_ADD_SCANCODE(0x50, KY_RELEASED(KY_LAUNCH_MEDIA_PLAYER)),
			PS2_ADD_SCANCODE(0x5a, KY_RELEASED(KY_NPADENT)),
			PS2_ADD_SCANCODE(0x5e, KY_RELEASED(KY_WAKE)),
			PS2_ADD_SCANCODE(0x69, KY_RELEASED(KY_END)),
			PS2_ADD_SCANCODE(0x6b, KY_RELEASED(KY_LEFT)),
			PS2_ADD_SCANCODE(0x6c, KY_RELEASED(KY_HOME)),
			PS2_ADD_SCANCODE(0x70, KY_RELEASED(KY_INSERT)),
			PS2_ADD_SCANCODE(0x71, KY_RELEASED(KY_DELETE)),
			PS2_ADD_SCANCODE(0x72, KY_RELEASED(KY_DOWN)),
			PS2_ADD_SCANCODE(0x74, KY_RELEASED(KY_RIGHT)),
			PS2_ADD_SCANCODE(0x75, KY_RELEASED(KY_UP)),
			PS2_ADD_SCANCODE(0x7a, KY_RELEASED(KY_PAGE_DOWN)),
			PS2_ADD_SCANCODE(0x7d, KY_RELEASED(KY_PAGE_UP)),
			PS2_ADD_SCANCODE_TABLE(0x7c, {
				PS2_ADD_SCANCODE_TABLE(0xe0, {
					PS2_ADD_SCANCODE_TABLE(0xf0, {
						PS2_ADD_SCANCODE(0x12, KY_RELEASED(KY_PRINT_SCREEN)),
						PS2_TERMINATE_SCANCODE_TABLE
					}),
					PS2_TERMINATE_SCANCODE_TABLE
				}),
				PS2_TERMINATE_SCANCODE_TABLE
			}),
			PS2_TERMINATE_SCANCODE_TABLE
		}),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_ADD_SCANCODE_TABLE(0xf0, {
		PS2_ADD_SCANCODE(0x01, KY_RELEASED(KY_F9)),
		PS2_ADD_SCANCODE(0x03, KY_RELEASED(KY_F5)),
		PS2_ADD_SCANCODE(0x04, KY_RELEASED(KY_F3)),
		PS2_ADD_SCANCODE(0x05, KY_RELEASED(KY_F1)),
		PS2_ADD_SCANCODE(0x06, KY_RELEASED(KY_F3)),
		PS2_ADD_SCANCODE(0x07, KY_RELEASED(KY_F12)),
		PS2_ADD_SCANCODE(0x09, KY_RELEASED(KY_F10)),
		PS2_ADD_SCANCODE(0x0a, KY_RELEASED(KY_F8)),
		PS2_ADD_SCANCODE(0x0b, KY_RELEASED(KY_F6)),
		PS2_ADD_SCANCODE(0x0c, KY_RELEASED(KY_F4)),
		PS2_ADD_SCANCODE(0x0d, KY_RELEASED(KY_TAB)),
		PS2_ADD_SCANCODE(0x0e, KY_RELEASED(KY_BACKTICK)),
		PS2_ADD_SCANCODE(0x11, KY_RELEASED(KY_ALTL)),
		PS2_ADD_SCANCODE(0x12, KY_RELEASED(KY_SHIFTL)),
		PS2_ADD_SCANCODE(0x14, KY_RELEASED(KY_CTRLL)),
		PS2_ADD_SCANCODE(0x15, KY_RELEASED(KY_Q)),
		PS2_ADD_SCANCODE(0x16, KY_RELEASED(KY_1)),
		PS2_ADD_SCANCODE(0x1a, KY_RELEASED(KY_Z)),
		PS2_ADD_SCANCODE(0x1b, KY_RELEASED(KY_S)),
		PS2_ADD_SCANCODE(0x1c, KY_RELEASED(KY_A)),
		PS2_ADD_SCANCODE(0x1d, KY_RELEASED(KY_W)),
		PS2_ADD_SCANCODE(0x1e, KY_RELEASED(KY_2)),
		PS2_ADD_SCANCODE(0x21, KY_RELEASED(KY_C)),
		PS2_ADD_SCANCODE(0x22, KY_RELEASED(KY_X)),
		PS2_ADD_SCANCODE(0x23, KY_RELEASED(KY_D)),
		PS2_ADD_SCANCODE(0x24, KY_RELEASED(KY_E)),
		PS2_ADD_SCANCODE(0x25, KY_RELEASED(KY_4)),
		PS2_ADD_SCANCODE(0x26, KY_RELEASED(KY_3)),
		PS2_ADD_SCANCODE(0x29, KY_RELEASED(KY_SPACE)),
		PS2_ADD_SCANCODE(0x2a, KY_RELEASED(KY_V)),
		PS2_ADD_SCANCODE(0x2b, KY_RELEASED(KY_F)),
		PS2_ADD_SCANCODE(0x2c, KY_RELEASED(KY_T)),
		PS2_ADD_SCANCODE(0x2d, KY_RELEASED(KY_R)),
		PS2_ADD_SCANCODE(0x2e, KY_RELEASED(KY_5)),
		PS2_ADD_SCANCODE(0x31, KY_RELEASED(KY_N)),
		PS2_ADD_SCANCODE(0x32, KY_RELEASED(KY_B)),
		PS2_ADD_SCANCODE(0x33, KY_RELEASED(KY_H)),
		PS2_ADD_SCANCODE(0x34, KY_RELEASED(KY_G)),
		PS2_ADD_SCANCODE(0x35, KY_RELEASED(KY_Y)),
		PS2_ADD_SCANCODE(0x36, KY_RELEASED(KY_6)),
		PS2_ADD_SCANCODE(0x3a, KY_RELEASED(KY_M)),
		PS2_ADD_SCANCODE(0x3b, KY_RELEASED(KY_J)),
		PS2_ADD_SCANCODE(0x3c, KY_RELEASED(KY_U)),
		PS2_ADD_SCANCODE(0x3d, KY_RELEASED(KY_7)),
		PS2_ADD_SCANCODE(0x3e, KY_RELEASED(KY_8)),
		PS2_ADD_SCANCODE(0x41, KY_RELEASED(KY_COMMA)),
		PS2_ADD_SCANCODE(0x42, KY_RELEASED(KY_K)),
		PS2_ADD_SCANCODE(0x43, KY_RELEASED(KY_I)),
		PS2_ADD_SCANCODE(0x44, KY_RELEASED(KY_O)),
		PS2_ADD_SCANCODE(0x45, KY_RELEASED(KY_0)),
		PS2_ADD_SCANCODE(0x46, KY_RELEASED(KY_9)),
		PS2_ADD_SCANCODE(0x49, KY_RELEASED(KY_PERIOD)),
		PS2_ADD_SCANCODE(0x4a, KY_RELEASED(KY_SLASH)),
		PS2_ADD_SCANCODE(0x4b, KY_RELEASED(KY_L)),
		PS2_ADD_SCANCODE(0x4c, KY_RELEASED(KY_SEMICOLON)),
		PS2_ADD_SCANCODE(0x4d, KY_RELEASED(KY_P)),
		PS2_ADD_SCANCODE(0x4e, KY_RELEASED(KY_MINUS)),
		PS2_ADD_SCANCODE(0x52, KY_RELEASED(KY_QUOTE)),
		PS2_ADD_SCANCODE(0x54, KY_RELEASED(KY_OPEN_BRACKET)),
		PS2_ADD_SCANCODE(0x55, KY_RELEASED(KY_EQUALS)),
		PS2_ADD_SCANCODE(0x58, KY_RELEASED(KY_CAPS_LOCK)),
		PS2_ADD_SCANCODE(0x59, KY_RELEASED(KY_SHIFTR)),
		PS2_ADD_SCANCODE(0x5a, KY_RELEASED(KY_ENTER)),
		PS2_ADD_SCANCODE(0x5b, KY_RELEASED(KY_CLOSE_BRACKET)),
		PS2_ADD_SCANCODE(0x5d, KY_RELEASED(KY_BACKSLASH)),
		PS2_ADD_SCANCODE(0x66, KY_RELEASED(KY_BACKSPACE)),
		PS2_ADD_SCANCODE(0x69, KY_RELEASED(KY_NPAD1)),
		PS2_ADD_SCANCODE(0x6b, KY_RELEASED(KY_NPAD4)),
		PS2_ADD_SCANCODE(0x6c, KY_RELEASED(KY_NPAD7)),
		PS2_ADD_SCANCODE(0x70, KY_RELEASED(KY_NPAD0)),
		PS2_ADD_SCANCODE(0x71, KY_RELEASED(KY_NPADDEC)),
		PS2_ADD_SCANCODE(0x72, KY_RELEASED(KY_NPAD2)),
		PS2_ADD_SCANCODE(0x73, KY_RELEASED(KY_NPAD5)),
		PS2_ADD_SCANCODE(0x74, KY_RELEASED(KY_NPAD6)),
		PS2_ADD_SCANCODE(0x75, KY_RELEASED(KY_NPAD8)),
		PS2_ADD_SCANCODE(0x76, KY_RELEASED(KY_ESC)),
		PS2_ADD_SCANCODE(0x77, KY_RELEASED(KY_NUMB_LOCK)),
		PS2_ADD_SCANCODE(0x78, KY_RELEASED(KY_F11)),
		PS2_ADD_SCANCODE(0x79, KY_RELEASED(KY_NPADADD)),
		PS2_ADD_SCANCODE(0x7a, KY_RELEASED(KY_NPAD3)),
		PS2_ADD_SCANCODE(0x7b, KY_RELEASED(KY_NPADSUB)),
		PS2_ADD_SCANCODE(0x7c, KY_RELEASED(KY_NPADMUL)),
		PS2_ADD_SCANCODE(0x7d, KY_RELEASED(KY_NPAD9)),
		PS2_ADD_SCANCODE(0x7e, KY_RELEASED(KY_SCRL_LOCK)),
		PS2_ADD_SCANCODE(0x83, KY_RELEASED(KY_F7)),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_ADD_SCANCODE_TABLE(0xe1, {
		PS2_ADD_SCANCODE_TABLE(0x14, {
			PS2_ADD_SCANCODE_TABLE(0x77, {
				PS2_ADD_SCANCODE_TABLE(0xe1, {
					PS2_ADD_SCANCODE_TABLE(0xf0, {
						PS2_ADD_SCANCODE_TABLE(0x14, {
							PS2_ADD_SCANCODE_TABLE(0xf0, {
								PS2_ADD_SCANCODE(0x77, KY_PRESSED(KY_PAUSE)),
								PS2_TERMINATE_SCANCODE_TABLE
							}),
							PS2_TERMINATE_SCANCODE_TABLE
						}),
						PS2_TERMINATE_SCANCODE_TABLE
					}),
					PS2_TERMINATE_SCANCODE_TABLE
				}),
				PS2_TERMINATE_SCANCODE_TABLE
			}),
			PS2_TERMINATE_SCANCODE_TABLE
		}),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_TERMINATE_SCANCODE_TABLE
};

scancode_t scancode_set3[] =
{
	PS2_ADD_SCANCODE(0x07, KY_PRESSED(KY_F1)),
	PS2_ADD_SCANCODE(0x08, KY_PRESSED(KY_ESC)),
	PS2_ADD_SCANCODE(0x0d, KY_PRESSED(KY_TAB)),
	PS2_ADD_SCANCODE(0x0e, KY_PRESSED(KY_BACKTICK)),
	PS2_ADD_SCANCODE(0x0f, KY_PRESSED(KY_F2)),
	PS2_ADD_SCANCODE(0x11, KY_PRESSED(KY_CTRLL)),
	PS2_ADD_SCANCODE(0x12, KY_PRESSED(KY_SHIFTL)),
	PS2_ADD_SCANCODE(0x14, KY_PRESSED(KY_CAPS_LOCK)),
	PS2_ADD_SCANCODE(0x15, KY_PRESSED(KY_Q)),
	PS2_ADD_SCANCODE(0x16, KY_PRESSED(KY_1)),
	PS2_ADD_SCANCODE(0x17, KY_PRESSED(KY_F3)),
	PS2_ADD_SCANCODE(0x19, KY_PRESSED(KY_ALTL)),
	PS2_ADD_SCANCODE(0x1a, KY_PRESSED(KY_Z)),
	PS2_ADD_SCANCODE(0x1b, KY_PRESSED(KY_S)),
	PS2_ADD_SCANCODE(0x1c, KY_PRESSED(KY_A)),
	PS2_ADD_SCANCODE(0x1d, KY_PRESSED(KY_W)),
	PS2_ADD_SCANCODE(0x1e, KY_PRESSED(KY_2)),
	PS2_ADD_SCANCODE(0x1f, KY_PRESSED(KY_F4)),
	PS2_ADD_SCANCODE(0x21, KY_PRESSED(KY_C)),
	PS2_ADD_SCANCODE(0x22, KY_PRESSED(KY_X)),
	PS2_ADD_SCANCODE(0x23, KY_PRESSED(KY_D)),
	PS2_ADD_SCANCODE(0x24, KY_PRESSED(KY_E)),
	PS2_ADD_SCANCODE(0x25, KY_PRESSED(KY_4)),
	PS2_ADD_SCANCODE(0x26, KY_PRESSED(KY_3)),
	PS2_ADD_SCANCODE(0x27, KY_PRESSED(KY_F5)),
	PS2_ADD_SCANCODE(0x29, KY_PRESSED(KY_SPACE)),
	PS2_ADD_SCANCODE(0x2a, KY_PRESSED(KY_V)),
	PS2_ADD_SCANCODE(0x2b, KY_PRESSED(KY_F)),
	PS2_ADD_SCANCODE(0x2c, KY_PRESSED(KY_T)),
	PS2_ADD_SCANCODE(0x2d, KY_PRESSED(KY_R)),
	PS2_ADD_SCANCODE(0x2f, KY_PRESSED(KY_F6)),
	PS2_ADD_SCANCODE(0x2e, KY_PRESSED(KY_5)),
	PS2_ADD_SCANCODE(0x31, KY_PRESSED(KY_N)),
	PS2_ADD_SCANCODE(0x32, KY_PRESSED(KY_B)),
	PS2_ADD_SCANCODE(0x33, KY_PRESSED(KY_H)),
	PS2_ADD_SCANCODE(0x34, KY_PRESSED(KY_G)),
	PS2_ADD_SCANCODE(0x35, KY_PRESSED(KY_Y)),
	PS2_ADD_SCANCODE(0x36, KY_PRESSED(KY_6)),
	PS2_ADD_SCANCODE(0x37, KY_PRESSED(KY_F7)),
	PS2_ADD_SCANCODE(0x39, KY_PRESSED(KY_ALTR)),
	PS2_ADD_SCANCODE(0x3a, KY_PRESSED(KY_M)),
	PS2_ADD_SCANCODE(0x3b, KY_PRESSED(KY_J)),
	PS2_ADD_SCANCODE(0x3c, KY_PRESSED(KY_U)),
	PS2_ADD_SCANCODE(0x3d, KY_PRESSED(KY_7)),
	PS2_ADD_SCANCODE(0x3e, KY_PRESSED(KY_8)),
	PS2_ADD_SCANCODE(0x3f, KY_PRESSED(KY_F8)),
	PS2_ADD_SCANCODE(0x41, KY_PRESSED(KY_COMMA)),
	PS2_ADD_SCANCODE(0x42, KY_PRESSED(KY_K)),
	PS2_ADD_SCANCODE(0x44, KY_PRESSED(KY_O)),
	PS2_ADD_SCANCODE(0x45, KY_PRESSED(KY_0)),
	PS2_ADD_SCANCODE(0x47, KY_PRESSED(KY_F9)),
	PS2_ADD_SCANCODE(0x49, KY_PRESSED(KY_PERIOD)),
	PS2_ADD_SCANCODE(0x4a, KY_PRESSED(KY_SLASH)),
	PS2_ADD_SCANCODE(0x4a, KY_PRESSED(KY_NPADDIV)),
	PS2_ADD_SCANCODE(0x4b, KY_PRESSED(KY_L)),
	PS2_ADD_SCANCODE(0x4c, KY_PRESSED(KY_SEMICOLON)),
	PS2_ADD_SCANCODE(0x4d, KY_PRESSED(KY_P)),
	PS2_ADD_SCANCODE(0x4e, KY_PRESSED(KY_NPADSUB)),
	PS2_ADD_SCANCODE(0x4f, KY_PRESSED(KY_F10)),
	PS2_ADD_SCANCODE(0x43, KY_PRESSED(KY_I)),
	PS2_ADD_SCANCODE(0x46, KY_PRESSED(KY_9)),
	PS2_ADD_SCANCODE(0x4e, KY_PRESSED(KY_MINUS)),
	PS2_ADD_SCANCODE(0x52, KY_PRESSED(KY_QUOTE)),
	PS2_ADD_SCANCODE(0x54, KY_PRESSED(KY_OPEN_BRACKET)),
	PS2_ADD_SCANCODE(0x55, KY_PRESSED(KY_EQUALS)),
	PS2_ADD_SCANCODE(0x56, KY_PRESSED(KY_F11)),
	PS2_ADD_SCANCODE(0x57, KY_PRESSED(KY_PRINT_SCREEN)),
	PS2_ADD_SCANCODE(0x58, KY_PRESSED(KY_CTRLR)),
	PS2_ADD_SCANCODE(0x59, KY_PRESSED(KY_SHIFTR)),
	PS2_ADD_SCANCODE(0x5a, KY_PRESSED(KY_ENTER)),
	PS2_ADD_SCANCODE(0x5b, KY_PRESSED(KY_CLOSE_BRACKET)),
	PS2_ADD_SCANCODE(0x5c, KY_PRESSED(KY_BACKSLASH)),
	PS2_ADD_SCANCODE(0x5e, KY_PRESSED(KY_F12)),
	PS2_ADD_SCANCODE(0x5f, KY_PRESSED(KY_SCRL_LOCK)),
	PS2_ADD_SCANCODE(0x60, KY_PRESSED(KY_DOWN)),
	PS2_ADD_SCANCODE(0x61, KY_PRESSED(KY_LEFT)),
	PS2_ADD_SCANCODE(0x62, KY_PRESSED(KY_PAUSE)),
	PS2_ADD_SCANCODE(0x63, KY_PRESSED(KY_UP)),
	PS2_ADD_SCANCODE(0x64, KY_PRESSED(KY_DELETE)),
	PS2_ADD_SCANCODE(0x65, KY_PRESSED(KY_END)),
	PS2_ADD_SCANCODE(0x66, KY_PRESSED(KY_BACKSPACE)),
	PS2_ADD_SCANCODE(0x67, KY_PRESSED(KY_INSERT)),
	PS2_ADD_SCANCODE(0x69, KY_PRESSED(KY_NPAD1)),
	PS2_ADD_SCANCODE(0x6a, KY_PRESSED(KY_RIGHT)),
	PS2_ADD_SCANCODE(0x6b, KY_PRESSED(KY_NPAD4)),
	PS2_ADD_SCANCODE(0x6c, KY_PRESSED(KY_NPAD7)),
	PS2_ADD_SCANCODE(0x6d, KY_PRESSED(KY_PAGE_DOWN)),
	PS2_ADD_SCANCODE(0x6e, KY_PRESSED(KY_HOME)),
	PS2_ADD_SCANCODE(0x6f, KY_PRESSED(KY_PAGE_UP)),
	PS2_ADD_SCANCODE(0x70, KY_PRESSED(KY_NPAD0)),
	PS2_ADD_SCANCODE(0x71, KY_PRESSED(KY_NPADDEC)),
	PS2_ADD_SCANCODE(0x72, KY_PRESSED(KY_NPAD2)),
	PS2_ADD_SCANCODE(0x73, KY_PRESSED(KY_NPAD5)),
	PS2_ADD_SCANCODE(0x74, KY_PRESSED(KY_NPAD6)),
	PS2_ADD_SCANCODE(0x75, KY_PRESSED(KY_NPAD8)),
	PS2_ADD_SCANCODE(0x76, KY_PRESSED(KY_NUMB_LOCK)),
	PS2_ADD_SCANCODE(0x79, KY_PRESSED(KY_NPADENT)),
	PS2_ADD_SCANCODE(0x7a, KY_PRESSED(KY_NPAD3)),
	PS2_ADD_SCANCODE(0x7c, KY_PRESSED(KY_NPADADD)),
	PS2_ADD_SCANCODE(0x7d, KY_PRESSED(KY_NPAD9)),
	PS2_ADD_SCANCODE(0x7e, KY_PRESSED(KY_NPADMUL)),
	PS2_ADD_SCANCODE(0x8b, KY_PRESSED(KY_METAL)),
	PS2_ADD_SCANCODE(0x8c, KY_PRESSED(KY_METAR)),
	PS2_ADD_SCANCODE(0x8d, KY_PRESSED(KY_LAUNCH_APPS)),
	PS2_ADD_SCANCODE_TABLE(0xf0, {
		PS2_ADD_SCANCODE(0x07, KY_RELEASED(KY_F1)),
		PS2_ADD_SCANCODE(0x08, KY_RELEASED(KY_ESC)),
		PS2_ADD_SCANCODE(0x0d, KY_RELEASED(KY_TAB)),
		PS2_ADD_SCANCODE(0x0e, KY_RELEASED(KY_BACKTICK)),
		PS2_ADD_SCANCODE(0x0f, KY_RELEASED(KY_F2)),
		PS2_ADD_SCANCODE(0x11, KY_RELEASED(KY_CTRLL)),
		PS2_ADD_SCANCODE(0x12, KY_RELEASED(KY_SHIFTL)),
		PS2_ADD_SCANCODE(0x14, KY_RELEASED(KY_CAPS_LOCK)),
		PS2_ADD_SCANCODE(0x15, KY_RELEASED(KY_Q)),
		PS2_ADD_SCANCODE(0x16, KY_RELEASED(KY_1)),
		PS2_ADD_SCANCODE(0x17, KY_RELEASED(KY_F3)),
		PS2_ADD_SCANCODE(0x19, KY_RELEASED(KY_ALTL)),
		PS2_ADD_SCANCODE(0x1a, KY_RELEASED(KY_Z)),
		PS2_ADD_SCANCODE(0x1b, KY_RELEASED(KY_S)),
		PS2_ADD_SCANCODE(0x1c, KY_RELEASED(KY_A)),
		PS2_ADD_SCANCODE(0x1d, KY_RELEASED(KY_W)),
		PS2_ADD_SCANCODE(0x1e, KY_RELEASED(KY_2)),
		PS2_ADD_SCANCODE(0x1f, KY_RELEASED(KY_F4)),
		PS2_ADD_SCANCODE(0x21, KY_RELEASED(KY_C)),
		PS2_ADD_SCANCODE(0x22, KY_RELEASED(KY_X)),
		PS2_ADD_SCANCODE(0x23, KY_RELEASED(KY_D)),
		PS2_ADD_SCANCODE(0x24, KY_RELEASED(KY_E)),
		PS2_ADD_SCANCODE(0x25, KY_RELEASED(KY_4)),
		PS2_ADD_SCANCODE(0x26, KY_RELEASED(KY_3)),
		PS2_ADD_SCANCODE(0x27, KY_RELEASED(KY_F5)),
		PS2_ADD_SCANCODE(0x29, KY_RELEASED(KY_SPACE)),
		PS2_ADD_SCANCODE(0x2a, KY_RELEASED(KY_V)),
		PS2_ADD_SCANCODE(0x2b, KY_RELEASED(KY_F)),
		PS2_ADD_SCANCODE(0x2c, KY_RELEASED(KY_T)),
		PS2_ADD_SCANCODE(0x2d, KY_RELEASED(KY_R)),
		PS2_ADD_SCANCODE(0x2f, KY_RELEASED(KY_F6)),
		PS2_ADD_SCANCODE(0x2e, KY_RELEASED(KY_5)),
		PS2_ADD_SCANCODE(0x31, KY_RELEASED(KY_N)),
		PS2_ADD_SCANCODE(0x32, KY_RELEASED(KY_B)),
		PS2_ADD_SCANCODE(0x33, KY_RELEASED(KY_H)),
		PS2_ADD_SCANCODE(0x34, KY_RELEASED(KY_G)),
		PS2_ADD_SCANCODE(0x35, KY_RELEASED(KY_Y)),
		PS2_ADD_SCANCODE(0x36, KY_RELEASED(KY_6)),
		PS2_ADD_SCANCODE(0x37, KY_RELEASED(KY_F7)),
		PS2_ADD_SCANCODE(0x39, KY_RELEASED(KY_ALTR)),
		PS2_ADD_SCANCODE(0x3a, KY_RELEASED(KY_M)),
		PS2_ADD_SCANCODE(0x3b, KY_RELEASED(KY_J)),
		PS2_ADD_SCANCODE(0x3c, KY_RELEASED(KY_U)),
		PS2_ADD_SCANCODE(0x3d, KY_RELEASED(KY_7)),
		PS2_ADD_SCANCODE(0x3e, KY_RELEASED(KY_8)),
		PS2_ADD_SCANCODE(0x3f, KY_RELEASED(KY_F8)),
		PS2_ADD_SCANCODE(0x41, KY_RELEASED(KY_COMMA)),
		PS2_ADD_SCANCODE(0x42, KY_RELEASED(KY_K)),
		PS2_ADD_SCANCODE(0x44, KY_RELEASED(KY_O)),
		PS2_ADD_SCANCODE(0x45, KY_RELEASED(KY_0)),
		PS2_ADD_SCANCODE(0x47, KY_RELEASED(KY_F9)),
		PS2_ADD_SCANCODE(0x49, KY_RELEASED(KY_PERIOD)),
		PS2_ADD_SCANCODE(0x4a, KY_RELEASED(KY_SLASH)),
		PS2_ADD_SCANCODE(0x4a, KY_RELEASED(KY_NPADDIV)),
		PS2_ADD_SCANCODE(0x4b, KY_RELEASED(KY_L)),
		PS2_ADD_SCANCODE(0x4c, KY_RELEASED(KY_SEMICOLON)),
		PS2_ADD_SCANCODE(0x4d, KY_RELEASED(KY_P)),
		PS2_ADD_SCANCODE(0x4e, KY_RELEASED(KY_NPADSUB)),
		PS2_ADD_SCANCODE(0x4f, KY_RELEASED(KY_F10)),
		PS2_ADD_SCANCODE(0x43, KY_RELEASED(KY_I)),
		PS2_ADD_SCANCODE(0x46, KY_RELEASED(KY_9)),
		PS2_ADD_SCANCODE(0x4e, KY_RELEASED(KY_MINUS)),
		PS2_ADD_SCANCODE(0x52, KY_RELEASED(KY_QUOTE)),
		PS2_ADD_SCANCODE(0x54, KY_RELEASED(KY_OPEN_BRACKET)),
		PS2_ADD_SCANCODE(0x55, KY_RELEASED(KY_EQUALS)),
		PS2_ADD_SCANCODE(0x56, KY_RELEASED(KY_F11)),
		PS2_ADD_SCANCODE(0x57, KY_RELEASED(KY_PRINT_SCREEN)),
		PS2_ADD_SCANCODE(0x58, KY_RELEASED(KY_CTRLR)),
		PS2_ADD_SCANCODE(0x59, KY_RELEASED(KY_SHIFTR)),
		PS2_ADD_SCANCODE(0x5a, KY_RELEASED(KY_ENTER)),
		PS2_ADD_SCANCODE(0x5b, KY_RELEASED(KY_CLOSE_BRACKET)),
		PS2_ADD_SCANCODE(0x5c, KY_RELEASED(KY_BACKSLASH)),
		PS2_ADD_SCANCODE(0x5e, KY_RELEASED(KY_F12)),
		PS2_ADD_SCANCODE(0x5f, KY_RELEASED(KY_SCRL_LOCK)),
		PS2_ADD_SCANCODE(0x60, KY_RELEASED(KY_DOWN)),
		PS2_ADD_SCANCODE(0x61, KY_RELEASED(KY_LEFT)),
		PS2_ADD_SCANCODE(0x62, KY_RELEASED(KY_PAUSE)),
		PS2_ADD_SCANCODE(0x63, KY_RELEASED(KY_UP)),
		PS2_ADD_SCANCODE(0x64, KY_RELEASED(KY_DELETE)),
		PS2_ADD_SCANCODE(0x65, KY_RELEASED(KY_END)),
		PS2_ADD_SCANCODE(0x66, KY_RELEASED(KY_BACKSPACE)),
		PS2_ADD_SCANCODE(0x67, KY_RELEASED(KY_INSERT)),
		PS2_ADD_SCANCODE(0x69, KY_RELEASED(KY_NPAD1)),
		PS2_ADD_SCANCODE(0x6a, KY_RELEASED(KY_RIGHT)),
		PS2_ADD_SCANCODE(0x6b, KY_RELEASED(KY_NPAD4)),
		PS2_ADD_SCANCODE(0x6c, KY_RELEASED(KY_NPAD7)),
		PS2_ADD_SCANCODE(0x6d, KY_RELEASED(KY_PAGE_DOWN)),
		PS2_ADD_SCANCODE(0x6e, KY_RELEASED(KY_HOME)),
		PS2_ADD_SCANCODE(0x6f, KY_RELEASED(KY_PAGE_UP)),
		PS2_ADD_SCANCODE(0x70, KY_RELEASED(KY_NPAD0)),
		PS2_ADD_SCANCODE(0x71, KY_RELEASED(KY_NPADDEC)),
		PS2_ADD_SCANCODE(0x72, KY_RELEASED(KY_NPAD2)),
		PS2_ADD_SCANCODE(0x73, KY_RELEASED(KY_NPAD5)),
		PS2_ADD_SCANCODE(0x74, KY_RELEASED(KY_NPAD6)),
		PS2_ADD_SCANCODE(0x75, KY_RELEASED(KY_NPAD8)),
		PS2_ADD_SCANCODE(0x76, KY_RELEASED(KY_NUMB_LOCK)),
		PS2_ADD_SCANCODE(0x79, KY_RELEASED(KY_NPADENT)),
		PS2_ADD_SCANCODE(0x7a, KY_RELEASED(KY_NPAD3)),
		PS2_ADD_SCANCODE(0x7c, KY_RELEASED(KY_NPADADD)),
		PS2_ADD_SCANCODE(0x7d, KY_RELEASED(KY_NPAD9)),
		PS2_ADD_SCANCODE(0x7e, KY_RELEASED(KY_NPADMUL)),
		PS2_ADD_SCANCODE(0x8b, KY_RELEASED(KY_METAL)),
		PS2_ADD_SCANCODE(0x8c, KY_RELEASED(KY_METAR)),
		PS2_ADD_SCANCODE(0x8d, KY_RELEASED(KY_LAUNCH_APPS)),
		PS2_TERMINATE_SCANCODE_TABLE
	}),
	PS2_TERMINATE_SCANCODE_TABLE
};

#ifdef __cplusplus
}
#endif

#endif
