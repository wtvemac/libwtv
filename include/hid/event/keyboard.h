
#ifndef __LIBWTV_HID_EVENT_KEYBOARD_H
#define __LIBWTV_HID_EVENT_KEYBOARD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


#define KEYMOD_METAL            0x00000001
#define KEYMOD_METAR            0x00000002
#define KEYMOD_META             (KEYMOD_METAL | KEYMOD_METAR)
#define KEYMOD_WINDOWS          KEYMOD_META
#define KEYMOD_CMD              KEYMOD_META
#define KEYMOD_CTRLL            0x00000004
#define KEYMOD_CTRLR            0x00000008
#define KEYMOD_CTRL             (KEYMOD_CTRLL | KEYMOD_CTRLR)
#define KEYMOD_NUMB_LOCK        0x00000010
#define KEYMOD_CAPS_LOCK        0x00000020
#define KEYMOD_SCRL_LOCK        0x00000040
#define KEYMOD_SHIFTL           0x00000080
#define KEYMOD_SHIFTR           0x00000100
#define KEYMOD_SHIFT            (KEYMOD_SHIFTL | KEYMOD_SHIFTR)
#define KEYMOD_ALTL             0x00000200
#define KEYMOD_ALTR             0x00000400
#define KEYMOD_ALT              (KEYMOD_ALTL | KEYMOD_ALTR)
#define KEYMOD_MENU             KEYMOD_ALT
#define KEYMOD_ALTGR            0x00000800 // ISO/IEC 9995 Level 3
#define KEYMOD_FN               0x00001000
#define KEYMOD_FN_LOCK          0x00002000
#define KEYMOD_HYPER            0x00004000
#define KEYMOD_SUPER            0x00008000
#define KEYMOD_SYMBOL           0x00010000
#define KEYMOD_SYMBOL_LOCK      0x00020000

#define KEY_NULL                0x0000

#define KEY_ESC                 0x001b
#define KEY_PRINT_SCREEN        0x0017
#define KEY_PAUSE               0x0016
#define KEY_BACKSPACE           0x0008
#define KEY_INSERT              0x001a
#define KEY_DELETE              0x007f
#define KEY_HOME                0x0001
#define KEY_END                 0x0003
#define KEY_PAGE_UP             0x000b
#define KEY_PAGE_DOWN           0x000c
#define KEY_TAB                 0x0009
#define KEY_ENTER               0x000d
#define KEY_RETURN              KEY_ENTER
#define KEY_UP                  0x0012
#define KEY_LEFT                0x0011
#define KEY_DOWN                0x0014
#define KEY_RIGHT               0x0013

//////////////////////////

#define KEY_SPACE               0x0020
#define KEY_COMMA               0x002c
#define KEY_PERIOD              0x002e
#define KEY_MINUS               0x002d
#define KEY_EQUALS              0x003d
#define KEY_SEMICOLON           0x003b
#define KEY_SLASH               0x002f
#define KEY_BACKSLASH           0x005c
#define KEY_OPEN_BRACKET        0x005b
#define KEY_CLOSE_BRACKET       0x005d
#define KEY_QUOTE               0x0027
#define KEY_BACKTICK            0x0060

#define KEY_0                   0x0030
#define KEY_1                   0x0031
#define KEY_2                   0x0032
#define KEY_3                   0x0033
#define KEY_4                   0x0034
#define KEY_5                   0x0035
#define KEY_6                   0x0036
#define KEY_7                   0x0037
#define KEY_8                   0x0038
#define KEY_9                   0x0039

#define KEY_A                   0x0041
#define KEY_B                   0x0042
#define KEY_C                   0x0043
#define KEY_D                   0x0044
#define KEY_E                   0x0045
#define KEY_F                   0x0046
#define KEY_G                   0x0047
#define KEY_H                   0x0048
#define KEY_I                   0x0049
#define KEY_J                   0x004a
#define KEY_K                   0x004b
#define KEY_L                   0x004c
#define KEY_M                   0x004d
#define KEY_N                   0x004e
#define KEY_O                   0x004f
#define KEY_P                   0x0050
#define KEY_Q                   0x0051
#define KEY_R                   0x0052
#define KEY_S                   0x0053
#define KEY_T                   0x0054
#define KEY_U                   0x0055
#define KEY_V                   0x0056
#define KEY_W                   0x0057
#define KEY_X                   0x0058
#define KEY_Y                   0x0059
#define KEY_Z                   0x005a

////////////////////

#define KEY_F1                  0x1001
#define KEY_F2                  0x1002
#define KEY_F3                  0x1003
#define KEY_F4                  0x1004
#define KEY_F5                  0x1005
#define KEY_F6                  0x1006
#define KEY_F7                  0x1007
#define KEY_F8                  0x1008
#define KEY_F9                  0x1009
#define KEY_F10                 0x100a
#define KEY_F11                 0x100b
#define KEY_F12                 0x100c
#define KEY_F13                 0x100d
#define KEY_F14                 0x100e
#define KEY_F15                 0x100f
#define KEY_F16                 0x1010
#define KEY_F17                 0x1011
#define KEY_F18                 0x1012
#define KEY_F19                 0x1013
#define KEY_F20                 0x1014
#define KEY_F21                 0x1015
#define KEY_F22                 0x1016
#define KEY_F23                 0x1017
#define KEY_F24                 0x1018

#define KEY_NPAD0               0x1130
#define KEY_NPAD1               0x1131
#define KEY_NPAD2               0x1132
#define KEY_NPAD3               0x1133
#define KEY_NPAD4               0x1134
#define KEY_NPAD5               0x1135
#define KEY_NPAD6               0x1136
#define KEY_NPAD7               0x1137
#define KEY_NPAD8               0x1138
#define KEY_NPAD9               0x1139
#define KEY_NPADMUL             0x112a
#define KEY_NPADADD             0x112b
#define KEY_NPADSUB             0x112d
#define KEY_NPADDEC             0x112e
#define KEY_NPADDIV             0x112f
#define KEY_NPADSEP             0x112c
#define KEY_NPADENT             KEY_ENTER

#define KEY_MEDIA_PREV_TRACK    0x1201
#define KEY_MEDIA_NEXT_TRACK    0x1202
#define KEY_MEDIA_STOP          0x1203
#define KEY_MEDIA_VOL_DOWN      0x1205
#define KEY_MEDIA_VOL_UP        0x1206
#define KEY_MEDIA_MUTE          0x1207
#define KEY_MEDIA_PLAYPAUSE     0x1208
#define KEY_MEDIA_CHAN_DOWN     0x1208
#define KEY_MEDIA_CHAN_UP       0x1208

#define KEY_BROWSER_BACK        0x1221
#define KEY_BROWSER_FORWARD     0x1222
#define KEY_BROWSER_STOP        0x1223
#define KEY_BROWSER_REFRESH     0x1224
#define KEY_BROWSER_HOME        0x1228
#define KEY_BROWSER_FAVORITES   0x122c
#define KEY_BROWSER_SEARCH      0x1230

#define KEY_LAUNCH_MYCOMPUTER   0x1250
#define KEY_LAUNCH_APPS         0x1251
#define KEY_LAUNCH_MEDIA_PLAYER 0x1252
#define KEY_LAUNCH_BROWSER      0x1253
#define KEY_LAUNCH_EMAIL        0x1254
#define KEY_LAUNCH_CALCULATOR   0x1255

#define KEY_WTV_RECENT          0x1280
#define KEY_WTV_BACK            0x1281
#define KEY_WTV_HOME            0x1288
#define KEY_WTV_FAVORITES       0x128c
#define KEY_WTV_SAVE            0x128e
#define KEY_WTV_SEARCH          0x1290
#define KEY_WTV_GOTO            0x1291
#define KEY_WTV_INFO            0x1292
#define KEY_WTV_FIND            0x1293
#define KEY_WTV_MAIL            0x1284
#define KEY_WTV_EDIT            0x1294
#define KEY_WTV_VIEW            0x1295
#define KEY_WTV_SEND            0x1285
#define KEY_WTV_OPTION          0x1286
#define KEY_WTV_PIP             0x1287
#define KEY_WTV_PIP_LOCATION    0x1288
#define KEY_WTV_TV              0x1289

//////////////////////////

#define KEY_METAL               0x2000 // \m/
#define KEY_METAR               0x2001
#define KEY_CTRLL               0x2002
#define KEY_CTRLR               0x2003
#define KEY_SHIFTL              0x2021
#define KEY_SHIFTR              0x2022
#define KEY_ALTL                0x2004
#define KEY_ALTR                0x2005
#define KEY_ALTGR               0x2006
#define KEY_CMD                 0x2007
#define KEY_FN                  0x2009

#define KEY_CAPS_LOCK           0x2020
#define KEY_SCRL_LOCK           0x2040
#define KEY_NUMB_LOCK           0x2080

#define KEY_POWER               0x2100
#define KEY_SLEEP               0x2101
#define KEY_WAKE                0x2102

//////////////////////////
//////////////////////////

#define KEY_PRESSED(keycode)   keycode
#define KEY_RELEASED(keycode) (keycode * -1)

// https://www.branah.com/chinese
//#define KEY_EMOJI

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

#define MAX_KEY_MEMORY 4

typedef struct __attribute__((packed)) keyboard_event_s
{
	uint32_t modifier;
	int16_t key;
	int16_t down_keys[MAX_KEY_MEMORY];
} keyboard_event_t;

#ifdef __cplusplus
}
#endif

#endif