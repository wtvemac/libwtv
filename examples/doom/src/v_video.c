// Emacs style mode select   -*- C++ -*-
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
//    Gamma correction LUT stuff.
//    Functions to draw patches (by post) directly to screen.
//    Functions to blit a block to the screen.
//
//-----------------------------------------------------------------------------
#include "z_zone.h"
#include "i_video.h"

#include "i_system.h"
#include "r_local.h"

#include "doomdef.h"
#include "doomdata.h"

#include "m_bbox.h"
#include "m_swap.h"

#include "v_video.h"

#define ytab(y) (((y)<<8)+((y)<<6))

extern uint32_t*      palarray;
extern void*          bufptr;

int                  usegamma;

//
// V_MarkRect
//
void V_MarkRect(int x, int y, int width, int height)
{
    //M_AddToBox (dirtybox, x, y);
    //M_AddToBox (dirtybox, x+width-1, y+height-1);
}

//
// V_CopyRect
//
void V_CopyRect( int srcx, int srcy, int srcscrn, int width, int height, int destx, int desty, int destscrn)
{
}

//
// V_DrawPatch
// Masks a column based masked pic to the screen.
//
//__attribute__ ((optimize(1)))
void V_DrawPatch ( int x, int y, patch_t* patch )
{
    int          count;
    int          col; 
    column_t*    column; 
    uint16_t*    desttop;
    uint16_t*    destmax;
    uint16_t*    dest;
    byte*        source; 
    int          w;

    y -= SHORT(patch->topoffset); 
    x -= SHORT(patch->leftoffset); 
#ifdef RANGECHECK 
    if (x<0
        ||x+SHORT(patch->width) >SCREENWIDTH
        || y<0
        || y+SHORT(patch->height)>SCREENHEIGHT)
    {
        fprintf( stderr, "Patch at %d,%d exceeds LFB\n", x,y );
        // No I_Error abort - what is up with TNT.WAD?
        fprintf( stderr, "V_DrawPatch: bad patch (ignored)\n");
    }
#endif


    col = 0; 
    desttop = (uint16_t*)((uintptr_t)bufptr + (uintptr_t)((((y)*SCREENWIDTH)+x)*2));

    w = SHORT(patch->width);

    for ( ; col<w; x++, col++, desttop++)
    { 
        column = (column_t *)((byte *)patch + LONG(patch->columnofs[col])); 

        // step through the posts in a column 
        while (column->topdelta != 0xff) 
        { 
            source = (byte *)column + 3; 
            dest = (uint16_t*)((uintptr_t)desttop + (uintptr_t)(column->topdelta*SCREENWIDTH*2));
            count = column->length; 

            while (count--) 
            { 
                if(((uint32_t)dest) & 2)
                {
                    *dest = (palarray[*source++] >> 0x00) & 0xffff;
                }
                else
                {
                    *dest = (palarray[*source++] >> 0x10) & 0xffff;
                }
                dest += SCREENWIDTH;
            }

            column = (column_t *)(  (byte *)column + column->length + 4 );
        } 
    }
}


void V_DrawPatchBuf ( int x, int y, patch_t* patch, uint16_t *buf)
{
    int          count;
    int          col; 
    column_t*    column;
    uint16_t*    desttop;
    uint16_t*    dest;
    byte*        source; 
    int          w; 

    y -= SHORT(patch->topoffset); 
    x -= SHORT(patch->leftoffset); 

#ifdef RANGECHECK 
    if (x<0
        ||x+SHORT(patch->width) >SCREENWIDTH
        || y<0
        || y+SHORT(patch->height)>SCREENHEIGHT)
    {
        fprintf( stderr, "Patch at %d,%d exceeds LFB\n", x,y );
        // No I_Error abort - what is up with TNT.WAD?
        fprintf( stderr, "V_DrawPatch: bad patch (ignored)\n");
    }
#endif 

    col = 0; 
    desttop = (uint16_t*)((uintptr_t)buf + (uintptr_t)((((y)*SCREENWIDTH)+x)*2));

    w = SHORT(patch->width); 

    //EMAC:printf("V_DrawPatchBuf x=%08x, y=%08x :: w=%08x, h=%08x, buf=%p\x0a\x0d", x, y, w, SHORT(patch->height), buf);


    for ( ; col<w ; x++, col++, desttop++)
    { 
        column = (column_t *)((byte *)patch + LONG(patch->columnofs[col])); 

        // step through the posts in a column 
        while (column->topdelta != 0xff ) 
        { 
            source = (byte *)column + 3; 
            dest = (uint16_t*)((uintptr_t)desttop + (uintptr_t)(column->topdelta*SCREENWIDTH*2)); 
            count = column->length; 

            while (count--) 
            { 
                if(((uint32_t)dest) & 2)
                {
                    *dest = (palarray[*source++] >> 0x00) & 0xffff;
                }
                else
                {
                    *dest = (palarray[*source++] >> 0x10) & 0xffff;
                }
                dest += SCREENWIDTH; 
            } 
            column = (column_t *)(  (byte *)column + column->length + 4 ); 
        } 
    }
}


//
// V_DrawPatchFlipped
// Masks a column based masked pic to the screen.
// Flips horizontally, e.g. to mirror face.
//
void V_DrawPatchFlipped ( int x, int y, patch_t* patch )
{
    int          count;
    int          col;
    column_t*    column;
    uint16_t*    desttop;
    uint16_t*    dest;
    byte*        source;
    int          w;

    y -= SHORT(patch->topoffset);
    x -= SHORT(patch->leftoffset);
#ifdef RANGECHECK 
    if (x<0
        ||x+SHORT(patch->width) >SCREENWIDTH
        || y<0
        || y+SHORT(patch->height)>SCREENHEIGHT)
    {
        fprintf( stderr, "Patch origin %d,%d exceeds LFB\n", x,y );
        I_Error ("Bad V_DrawPatch in V_DrawPatchFlipped");
    }
#endif

    col = 0;
    desttop = (uint16_t*)((uintptr_t)bufptr + (uintptr_t)((((y)*SCREENWIDTH)+x)*2));

    w = SHORT(patch->width);

    for ( ; col<w ; x++, col++, desttop++)
    {
        column = (column_t *)((byte *)patch + LONG(patch->columnofs[w-1-col]));

        // step through the posts in a column
        while (column->topdelta != 0xff )
        {
            y+=column->topdelta;

            source = (byte *)column + 3;
            dest = (uint16_t*)((uintptr_t)desttop + (uintptr_t)(column->topdelta*SCREENWIDTH*2)); 
            count = column->length;

            while (count--)
            {
                if(((uint32_t)dest) & 2)
                {
                    *dest = (palarray[*source++] >> 0x00) & 0xffff;
                }
                else
                {
                    *dest = (palarray[*source++] >> 0x10) & 0xffff;
                }
                dest += SCREENWIDTH; 
            }

            column = (column_t *)(  (byte *)column + column->length + 4 );
        }
    }
}


//
// V_DrawBlock
// Draw a linear block of pixels into the view buffer.
//
void
V_DrawBlock
( int		x,
  int		y,
  int		width,
  int		height,
  uint16_t*		src )
{
    // hack for f_wipe
    memcpy(bufptr, src, 320*200*2);
}


//
// V_GetBlock
// Gets a linear block of pixels from the view buffer.
//
void
V_GetBlock
( int		x,
  int		y,
  int		width,
  int		height,
  uint16_t*		dest )
{
    // hack for f_wipe
    memcpy(dest, bufptr, 320*200*2);
}


//
// V_Init
//
void V_Init (void)
{
}
