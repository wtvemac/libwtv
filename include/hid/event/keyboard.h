
#ifndef __LIBWTV_HID_EVENT_KEYBOARD_H
#define __LIBWTV_HID_EVENT_KEYBOARD_H

#include "hid/ps2.h"
#include "hid/ir.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define KYMOD_METAL            0x00000001
#define KYMOD_METAR            0x00000002
#define KYMOD_META             (KYMOD_METAL | KYMOD_METAR)
#define KYMOD_WINDOWS          KYMOD_META
#define KYMOD_CMD              KYMOD_META
#define KYMOD_CTRLL            0x00000004
#define KYMOD_CTRLR            0x00000008
#define KYMOD_CTRL             (KYMOD_CTRLL | KYMOD_CTRLR)
#define KYMOD_NUMB_LOCK        0x00000010
#define KYMOD_CAPS_LOCK        0x00000020
#define KYMOD_SCRL_LOCK        0x00000040
#define KYMOD_SHIFTL           0x00000080
#define KYMOD_SHIFTR           0x00000100
#define KYMOD_SHIFT            (KYMOD_SHIFTL | KYMOD_SHIFTR)
#define KYMOD_ALTL             0x00000200
#define KYMOD_ALTR             0x00000400
#define KYMOD_ALT              (KYMOD_ALTL | KYMOD_ALTR)
#define KYMOD_MENU             KYMOD_ALT
#define KYMOD_ALTGR            0x00000800 // ISO/IEC 9995 Level 3
#define KYMOD_FN               0x00001000
#define KYMOD_FN_LOCK          0x00002000
#define KYMOD_HYPER            0x00004000
#define KYMOD_SUPER            0x00008000
#define KYMOD_SYMBOL           0x00010000
#define KYMOD_SYMBOL_LOCK      0x00020000

#define KY_NULL                0x0000

#define KY_ESC                 0x001b
#define KY_PRINT_SCREEN        0x0017
#define KY_PAUSE               0x0016
#define KY_BACKSPACE           0x0008
#define KY_INSERT              0x001a
#define KY_DELETE              0x007f
#define KY_HOME                0x0001
#define KY_END                 0x0003
#define KY_PAGE_UP             0x000b
#define KY_PAGE_DOWN           0x000c
#define KY_TAB                 0x0009
#define KY_ENTER               0x000d
#define KY_RETURN              KY_ENTER
#define KY_UP                  0x0012
#define KY_LEFT                0x0011
#define KY_DOWN                0x0014
#define KY_RIGHT               0x0013

//////////////////////////

#define KY_SPACE               0x0020
#define KY_COMMA               0x002c
#define KY_PERIOD              0x002e
#define KY_MINUS               0x002d
#define KY_EQUALS              0x003d
#define KY_SEMICOLON           0x003b
#define KY_SLASH               0x002f
#define KY_BACKSLASH           0x005c
#define KY_OPEN_BRACKET        0x005b
#define KY_CLOSE_BRACKET       0x005d
#define KY_QUOTE               0x0027
#define KY_BACKTICK            0x0060

#define KY_0                   0x0030
#define KY_1                   0x0031
#define KY_2                   0x0032
#define KY_3                   0x0033
#define KY_4                   0x0034
#define KY_5                   0x0035
#define KY_6                   0x0036
#define KY_7                   0x0037
#define KY_8                   0x0038
#define KY_9                   0x0039

#define KY_A                   0x0041
#define KY_B                   0x0042
#define KY_C                   0x0043
#define KY_D                   0x0044
#define KY_E                   0x0045
#define KY_F                   0x0046
#define KY_G                   0x0047
#define KY_H                   0x0048
#define KY_I                   0x0049
#define KY_J                   0x004a
#define KY_K                   0x004b
#define KY_L                   0x004c
#define KY_M                   0x004d
#define KY_N                   0x004e
#define KY_O                   0x004f
#define KY_P                   0x0050
#define KY_Q                   0x0051
#define KY_R                   0x0052
#define KY_S                   0x0053
#define KY_T                   0x0054
#define KY_U                   0x0055
#define KY_V                   0x0056
#define KY_W                   0x0057
#define KY_X                   0x0058
#define KY_Y                   0x0059
#define KY_Z                   0x005a

////////////////////

#define KY_F1                  0x1001
#define KY_F2                  0x1002
#define KY_F3                  0x1003
#define KY_F4                  0x1004
#define KY_F5                  0x1005
#define KY_F6                  0x1006
#define KY_F7                  0x1007
#define KY_F8                  0x1008
#define KY_F9                  0x1009
#define KY_F10                 0x100a
#define KY_F11                 0x100b
#define KY_F12                 0x100c
#define KY_F13                 0x100d
#define KY_F14                 0x100e
#define KY_F15                 0x100f
#define KY_F16                 0x1010
#define KY_F17                 0x1011
#define KY_F18                 0x1012
#define KY_F19                 0x1013
#define KY_F20                 0x1014
#define KY_F21                 0x1015
#define KY_F22                 0x1016
#define KY_F23                 0x1017
#define KY_F24                 0x1018

#define KY_NPAD0               0x1130
#define KY_NPAD1               0x1131
#define KY_NPAD2               0x1132
#define KY_NPAD3               0x1133
#define KY_NPAD4               0x1134
#define KY_NPAD5               0x1135
#define KY_NPAD6               0x1136
#define KY_NPAD7               0x1137
#define KY_NPAD8               0x1138
#define KY_NPAD9               0x1139
#define KY_NPADMUL             0x112a
#define KY_NPADADD             0x112b
#define KY_NPADSUB             0x112d
#define KY_NPADDEC             0x112e
#define KY_NPADDIV             0x112f
#define KY_NPADSEP             0x112c
#define KY_NPADENT             KY_ENTER

#define KY_MEDIA_PREV_TRACK    0x1201
#define KY_MEDIA_NEXT_TRACK    0x1202
#define KY_MEDIA_STOP          0x1203
#define KY_MEDIA_VOL_DOWN      0x1205
#define KY_MEDIA_VOL_UP        0x1206
#define KY_MEDIA_MUTE          0x1207
#define KY_MEDIA_PLAYPAUSE     0x1208
#define KY_MEDIA_CHAN_DOWN     0x1208
#define KY_MEDIA_CHAN_UP       0x1208

#define KY_BROWSER_BACK        0x1221
#define KY_BROWSER_FORWARD     0x1222
#define KY_BROWSER_STOP        0x1223
#define KY_BROWSER_REFRESH     0x1224
#define KY_BROWSER_HOME        0x1228
#define KY_BROWSER_FAVORITES   0x122c
#define KY_BROWSER_SEARCH      0x1230

#define KY_LAUNCH_MYCOMPUTER   0x1250
#define KY_LAUNCH_APPS         0x1251
#define KY_LAUNCH_MEDIA_PLAYER 0x1252
#define KY_LAUNCH_BROWSER      0x1253
#define KY_LAUNCH_EMAIL        0x1254
#define KY_LAUNCH_CALCULATOR   0x1255

#define KY_WTV_RECENT          0x1280
#define KY_WTV_BACK            0x1281
#define KY_WTV_HOME            0x1288
#define KY_WTV_FAVORITES       0x128c
#define KY_WTV_SAVE            0x128e
#define KY_WTV_SEARCH          0x1290
#define KY_WTV_GOTO            0x1291
#define KY_WTV_INFO            0x1292
#define KY_WTV_FIND            0x1293
#define KY_WTV_MAIL            0x1284
#define KY_WTV_EDIT            0x1294
#define KY_WTV_VIEW            0x1295
#define KY_WTV_SEND            0x1285
#define KY_WTV_OPTION          0x1286
#define KY_WTV_PIP             0x1287
#define KY_WTV_PIP_LOCATION    0x1288
#define KY_WTV_TV              0x1289

//////////////////////////

#define KY_METAL               0x2000 // \m/
#define KY_METAR               0x2001
#define KY_CTRLL               0x2002
#define KY_CTRLR               0x2003
#define KY_SHIFTL              0x2021
#define KY_SHIFTR              0x2022
#define KY_ALTL                0x2004
#define KY_ALTR                0x2005
#define KY_ALTGR               0x2006
#define KY_CMD                 0x2007
#define KY_FN                  0x2009

#define KY_CAPS_LOCK           0x2020
#define KY_SCRL_LOCK           0x2040
#define KY_NUMB_LOCK           0x2080

#define KY_POWER               0x2100
#define KY_SLEEP               0x2101
#define KY_WAKE                0x2102

//////////////////////////
//////////////////////////

#define KY_PRESSED(keycode)   keycode
#define KY_RELEASED(keycode) (keycode * -1)

// https://www.branah.com/chinese
//#define KY_EMOJI

// SELECT
// EXECUTE
// HELP
// APPLICATIONS
// SLEEP
// APP1
// APP2

// https://en.wikipedia.org/wiki/Language_input_keys
// IME process
// IME Kana mode
// IME Hangul mode
// IME On
// IME Junja mode
// IME final mode
// IME Hanja mode
// IME Kanji
// IME off
// IME convert
// IME nonconvert
// IME accept
// IME mode change request

// PACKET 0xe

// ATTN
// CrSel
// ExSel
// Erase EOF
// Play
// Zoom
// PA1
// OEM_CLEAR

///
/// Negative number for up (break), positive for down (make)
///

#define MAX_KY_MEMORY 4

typedef struct __attribute__((packed)) keyboard_event_s
{
	uint32_t modifier;
	int16_t key;
	int16_t down_keys[MAX_KY_MEMORY];
} keyboard_event_t;

#ifdef __cplusplus
}
#endif

#endif