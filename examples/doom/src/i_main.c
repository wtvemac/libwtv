// Emacs style mode select   -*- C++ -*-
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// $Log:$
//
// DESCRIPTION:
//    Main program, simply calls D_DoomMain high level loop.
//
//-----------------------------------------------------------------------------
#include <limits.h>

//EMAC:#include <stdlib.h>

#include <inttypes.h>
#include "libwtv.h"

#include "doomdef.h"
#include "m_argv.h"
#include "d_main.h"
#include "g_game.h"

extern void DoomIsOver();

//EMAC:
/*
void check_and_init_mempak(void)
{
    struct controller_data output;
    get_accessories_present(&output);

    switch (identify_accessory(0))
    {
        case ACCESSORY_NONE:
        {
            printf("No accessory inserted!\x0a\x0d");
            break;
        }
        case ACCESSORY_MEMPAK:
        {
            int err;
            // I left the below in the code #ifdef'd out for when I needed an easy way to nuke a controller pak
#if 0
            if (format_mempak(0))
            {
                printf("Error formatting mempak!\x0a\x0d");
            }
            else
            {
                printf("Memory card formatted!\x0a\x0d");
            }
#endif
            if ((err = validate_mempak(0)))
            {
                if (err == -3)
                {
                    printf("Mempak is not formatted! Formatting it automatically.\x0a\x0d");

                    if (format_mempak(0))
                    {
                        printf("Error formatting mempak!\x0a\x0d");
                    }
                    else
                    {
                        printf("Memory card formatted!\x0a\x0d");
                    }
                }
                else
                {
                    printf("Mempak bad or removed during read!\x0a\x0d");
                }
            }
            else
            {
                // this will print the details of each entry on the controller pak
#if 0
                for(int j = 0; j < 16; j++)
                {
                    entry_structure_t entry;

                    get_mempak_entry(0, j, &entry);

                    if(entry.valid)
                    {
                        printf("%s - %d blocks\x0a\x0d", entry.name, entry.blocks);
                    }
                    else
                    {
                        printf("(EMPTY)\x0a\x0d");
                    }
                }
#endif
                printf("\x0a\x0dFree space: %d blocks\x0a\x0d", get_mempak_free_space(0));
            }

            break;
        }
        case ACCESSORY_RUMBLEPAK:
        {
            printf("Cannot read data off of rumblepak!\x0a\x0d");
            break;
        }
    }
}
*/

extern int center_x, center_y;

int main(int argc, char **argv)
{
	set_leds(2);
	serial_io_init();
	set_leds(3);

    printf("Heap init\x0a\x0d");
	heap_init2();

	set_leds(4);
    
    printf("Console init\x0a\x0d");
    console_init();
    printf("Console console_set_render_mode\x0a\x0d");
    console_set_render_mode(RENDER_AUTOMATIC);

	set_leds(7);

    //EMAC:if (dfs_init( DFS_DEFAULT_LOCATION ) != DFS_ESUCCESS)
    //EMAC:{
    //EMAC:     printf("Could not initialize filesystem!\x0a\x0d");
    //EMAC:     while(1);
    //EMAC:  }
    //EMAC: controller_init();

    // center joystick...
    //EMAC:controller_scan();
    //EMAC:struct controller_data keys_pressed = get_keys_down();
    //EMAC:struct SI_condat pressed = keys_pressed.c[0];
    //EMAC:center_x = pressed.x;
    //EMAC:center_y = pressed.y;

    printf("Custom WebTV DOOM by eMac\x0a\x0d");
    printf("built %s %s\x0a\x0d", __DATE__, __TIME__);

    //int available_memory_size = get_memory_size();
    //printf("Available memory: %d bytes\x0a\x0d", *(int *)(0x80000318));

    //printf("Expansion Pak found.\x0a\x0d");

    //printf("Checking for Mempak:\x0a\x0d");
    //EMAC:check_and_init_mempak();

    D_DoomMain();
    DoomIsOver();

    return 0;
}
