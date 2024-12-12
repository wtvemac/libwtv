// Emacs style mode select   -*- C++ -*- h
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//    Doom input handler for Nintendo 64, libdragon
//
//-----------------------------------------------------------------------------


#include "libwtv.h"
//EMAC:#include <stdlib.h>

#include "i_input.h"

#include "doomstat.h"
#include "i_system.h"
#include "v_video.h"
#include "m_argv.h"
#include "d_main.h"

#include "doomdef.h"

void n64_do_cheat(int cheat);

static int GODDED;

uint32_t current_map;
uint32_t current_episode;
GameMode_t current_mode;

static int pad_weapon = 1;
static char weapons[8] = { '1', '2', '3', '3', '4', '5', '6', '7' };
static int lz_count = 0;
static int shift_times = 0;

int controller_mapping = 0;
int last_x = 0;
int last_y = 0;
int center_x = 0;
int center_y = 0;
int shift = 0;

//EMAC:void pressed_key(struct controller_data *p_data);//, int player);
//EMAC:void held_key(struct controller_data *h_data);//, int player);
//EMAC:void released_key(struct controller_data *r_data);//, int player);

int current_player_for_input = 0;


//
// I_GetEvent
// called by I_StartTic, scans player 1 controller for keys
// held_key is only used to scan analog stick to handle "mouse" event
// pressed_key/released_key are used to handle "keyboard" events
//
void I_GetEvent(void)
{
    controller_scan();

    hid_event event_object = dequeue_hid_event();

    if(event_object.source != EVENT_NULL) {
        event_t doom_input_event;

        uint16_t keycode = GET_KEYCODE(event_object.data);

        switch(keycode) {
            case KY_RIGHT:
                doom_input_event.data1 = KEY_RIGHTARROW;
                break;
            case KY_LEFT:
                doom_input_event.data1 = KEY_LEFTARROW;
                break;
            case KY_UP:
                doom_input_event.data1 = KEY_UPARROW;
                break;
            case KY_DOWN:
                doom_input_event.data1 = KEY_DOWNARROW;
                break;

            case KY_ESC:
                doom_input_event.data1 = KEY_ESCAPE;
                break;
            case KY_ENTER:
                doom_input_event.data1 = KEY_ENTER;
                break;
            case KY_TAB:
                doom_input_event.data1 = KEY_TAB;
                break;
            case KY_F1:
                doom_input_event.data1 = KEY_F1;
                break;
            case KY_F2:
                doom_input_event.data1 = KEY_F2;
                break;
            case KY_F3:
                doom_input_event.data1 = KEY_F3;
                break;
            case KY_F4:
                doom_input_event.data1 = KEY_F4;
                break;
            case KY_F5:
                doom_input_event.data1 = KEY_F5;
                break;
            case KY_F6:
                doom_input_event.data1 = KEY_F6;
                break;
            case KY_F7:
                doom_input_event.data1 = KEY_F7;
                break;
            case KY_F8:
                doom_input_event.data1 = KEY_F8;
                break;
            case KY_F9:
                doom_input_event.data1 = KEY_F9;
                break;
            case KY_F10:
                doom_input_event.data1 = KEY_F10;
                break;
            case KY_F11:
                doom_input_event.data1 = KEY_F11;
                break;
            case KY_F12:
                doom_input_event.data1 = KEY_F12;
                break;

            case KY_BACKSPACE:
                doom_input_event.data1 = KEY_BACKSPACE;
                break;
            case KY_PAUSE:
                doom_input_event.data1 = KEY_PAUSE;
                break;

            case KY_EQUALS:
                doom_input_event.data1 = KEY_EQUALS;
                break;
            case KY_NPADSUB:
            case KY_MINUS:
                doom_input_event.data1 = KEY_MINUS;
                break;
            case KY_NPADADD:
                doom_input_event.data1 = KEY_PLUS;
                break;

            case KY_SHIFTR:
                doom_input_event.data1 = KEY_RSHIFT;
                break;
            case KY_CTRLR:
                doom_input_event.data1 = KEY_RCTRL;
                break;
            case KY_ALTL:
                doom_input_event.data1 = KEY_LALT;
                break;
            case KY_ALTR:
                doom_input_event.data1 = KEY_RALT;
                break;
                
            case KY_CAPS_LOCK:
                doom_input_event.data1 = KEY_CAPSLOCK;
                break;
        }

        doom_input_event.type = (KY_IS_PRESSED(event_object.data)) ? ev_keydown : ev_keyup;
        D_PostEvent(&doom_input_event);
    }
}


//
// I_StartTic
// just calls I_GetEvent
//
void I_StartTic(void)
{
    I_GetEvent();
}

static char __attribute__((aligned(8))) clevstr[9];

extern    char*    get_GAMEID();
extern    char*    doom1wad;
extern    char*    doomwad;
extern    char*    doomuwad;
extern    char*    doom2wad;
extern    char*    plutoniawad;
extern    char*    tntwad;

//EMAC:#define stricmp strcasecmp

void n64_do_cheat(int cheat)
{
    char       *str;
    int        i;
    event_t    event;

    switch (cheat)
    {
        case 1: // God Mode
        {
            str = "iddqd";
            break;
        }
        case 2: // Fucking Arsenal
        {
            str = "idfa";
            break;
        }
        case 3: // Key Full Ammo
        {
            str = "idkfa";
            break;
        }
        case 4: // No Clipping
        {
            str = "idclip";
            break;
        }
        case 5: // Toggle Map
        {
            str = "iddt";
            break;
        }
        case 6: // Invincible with Chainsaw
        {
            str = "idchoppers";
            break;
        }
        case 7: // Berserker Strength Power-up
        {
            str = "idbeholds";
            break;
        }
        case 8: // Invincibility Power-up
        {
            str = "idbeholdv";
            break;
        }
        case 9: // Invisibility Power-Up
        {
            str = "idbeholdi";
            break;
        }
        case 10: // Automap Power-up
        {
            str = "idbeholda";
            break;
        }
        case 11: // Anti-Radiation Suit Power-up
        {
            str = "idbeholdr";
            break;
        }
        case 12: // Light-Amplification Visor Power-up
        {
            str = "idbeholdl";
            break;
        }
        case 13: // change level
        {
            const char *gameid = get_GAMEID();

            if (!stricmp(doom2wad,gameid) || !stricmp(tntwad,gameid) || !stricmp(plutoniawad,gameid))
            {
                if (!stricmp(doom2wad,gameid))
                {
                    if (current_map < 32)
                    {
                        current_map++;
                    }
                    else
                    {
                        current_map = 1;
                    }
                }
                else
                {
                    if(current_map < 34) {
                        current_map++;
                    }
                    else
                    {
                        current_map = 1;
                    }
                }

                sprintf(clevstr, "idclev%02d", current_map);//EMAC:02ld top 02d

                str = clevstr;
            }
            else
            {
                // shut up compiler...
                if (current_episode > 4)
                {
                    I_Error("n64_do_cheat: Invalid current_episode: %d.", current_episode);//EMAC:ld top d
                    return;
                }

                if (current_map < 9)
                {
                    current_map++;
                }
                else
                {
                    current_map = 1;
                }

                sprintf(clevstr, "idclev%1d%1d", current_episode, current_map);//EMAC:ld top d
                str = clevstr;
            }
            break;
        }
        default:
        {
            return;
        }
    }

    for (i=0; i<strlen(str); i++)
    {
        event.type = ev_keydown;
        event.data1 = str[i];
        D_PostEvent (&event);
        event.type = ev_keyup;
        event.data1 = str[i];
        D_PostEvent (&event);
    }
}

