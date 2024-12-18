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
//    Rendering main loop and setup functions,
//     utility functions (BSP, geometry, trigonometry).
//    See tables.c, too.
//
//-----------------------------------------------------------------------------

//EMAC:#include <stdlib.h>
#include <math.h>

#include "doomdef.h"
#include "d_net.h"

#include "m_bbox.h"

#include "r_local.h"
#include "r_sky.h"
#include "r_draw.h"

//extern int setup_level_debug;

// Fineangles in the SCREENWIDTH wide window.
#define FIELDOFVIEW        2048    

// used https://www.dcode.fr/function-equation-finder
// parabola / hyperbola using curve fitting
// 3 multiplies, an add and a subtract
// vs an uncached memory access
// this wins
#define tantoangle(x) ((angle_t)((-47*((x)*(x))) + (359628*(x)) - 3150270))


int            viewangleoffset;

// increment every time a check is made
int            validcount = 1;        


lighttable_t*        fixedcolormap;
extern lighttable_t**    walllights;

int            centerx;
int            centery;

fixed_t            centerxfrac;
fixed_t            centeryfrac;
fixed_t            projection;


int            sscount;
int            linecount;
int            loopcount;

fixed_t            viewx;
fixed_t            viewy;
fixed_t            viewz;

angle_t            viewangle;
fixed_t            viewcos;
fixed_t            viewsin;

player_t*        viewplayer;

// bumped light from gun blasts
int            extralight;            
// 0 = high, 1 = low
int            detailshift;    
//
// precalculated math tables
//
angle_t            clipangle;

// The viewangletox[viewangle + FINEANGLES/4] lookup
// maps the visible view angles to screen X coordinates,
// flattening the arc to a flat projection plane.
// There will be many angles mapped to the same X. 
//int            c_viewangletox[FINEANGLES/2];

int            viewangletox[FINEANGLES/2];
//int*           viewangletox;

// The xtoviewangleangle[] table maps a screen pixel
// to the lowest viewangle that maps back to x ranges
// from clipangle to -clipangle.
//angle_t            c_xtoviewangle[SCREENWIDTH+1];

angle_t            xtoviewangle[SCREENWIDTH+1];
//angle_t*           xtoviewangle;

lighttable_t*        scalelight[LIGHTLEVELS][MAXLIGHTSCALE];
lighttable_t*        scalelightfixed[MAXLIGHTSCALE];
lighttable_t*        zlight[LIGHTLEVELS][MAXLIGHTZ];

void (*colfunc) (int yl, int yh, int x);
void (*skycolfunc) (int yl, int yh, int x);
void (*basecolfunc) (int yl, int yh, int x);
void (*fuzzcolfunc) (int yl, int yh, int x);
void (*transcolfunc) (int yl, int yh, int x);
void (*spanfunc) (int x1, int x2, int y);

static inline  __attribute__((always_inline)) int SlopeDiv(uint32_t num, uint32_t den)
{
    uint32_t ans;

    if (den < 512)
    {
        return SLOPERANGE;
    }

    ans = (num<<3) / (den>>8);

    return ((ans <= SLOPERANGE) ? ans : SLOPERANGE);
}

//
// R_AddPointToBox
// Expand a given bbox
// so that it encloses a given point.
//
void
R_AddPointToBox
( int        x,
  int        y,
  fixed_t*    box )
{
    if (x< box[BOXLEFT])
        box[BOXLEFT] = x;

    if (x> box[BOXRIGHT])
        box[BOXRIGHT] = x;

    if (y< box[BOXBOTTOM])
        box[BOXBOTTOM] = y;

    if (y> box[BOXTOP])
        box[BOXTOP] = y;
}


//
// R_PointOnSide
// Traverse BSP (sub) tree,
//  check point against partition plane.
// Returns side 0 (front) or 1 (back).
//
static inline int
R_PointOnSide
( fixed_t    x,
  fixed_t    y,
  node_t*    node )
{
    fixed_t    dx;
    fixed_t    dy;
    fixed_t    left;
    fixed_t    right;

    if (!node->dx)
    {
        if (x <= node->x)
            return node->dy > 0;

        return node->dy < 0;
    }

    if (!node->dy)
    {
        if (y <= node->y)
            return node->dx < 0;

        return node->dx > 0;
    }

    dx = (x - node->x);
    dy = (y - node->y);

    // Try to quickly decide by looking at sign bits.
    if ((node->dy ^ node->dx ^ dx ^ dy)&0x80000000)
    {
        if  ((node->dy ^ dx) & 0x80000000)
        {
            // (left is negative)
            return 1;
        }

        return 0;
    }

    left = FixedMul ( node->dy>>FRACBITS , dx );
    right = FixedMul ( dy , node->dx>>FRACBITS );

    if (right < left)
    {
        // front side
        return 0;
    }

    // back side
    return 1;
}



//
// R_PointToAngle
// To get a global angle from cartesian coordinates,
//  the coordinates are flipped until they are in
//  the first octant of the coordinate system, then
//  the y (<=x) is scaled and divided by x to get a
//  tangent (slope) value which is looked up in the
//  tantoangle[] table.

//


angle_t
R_PointToAngle
( fixed_t    x,
  fixed_t    y )
{
    x -= viewx;
    y -= viewy;

    if ((!x) && (!y))
        return 0;

    if (x>= 0)
    {
        // x >=0
        if (y>= 0)
        {
            // y>= 0

            if (x>y)
            {
                // octant 0
                return tantoangle(SlopeDiv(y,x));
            }
            else
            {
                // octant 1
                return ANG90-1-tantoangle(SlopeDiv(x,y));
            }
        }
        else
        {
            // y<0
            y = -y;

            if (x>y)
            {
                // octant 8
                return -tantoangle(SlopeDiv(y,x));
            }
            else
            {
                // octant 7
                return ANG270+tantoangle(SlopeDiv(x,y));
            }
        }
    }
    else
    {
        // x<0
        x = -x;

        if (y>= 0)
        {
            // y>= 0
            if (x>y)
            {
                // octant 3
                return ANG180-1-tantoangle(SlopeDiv(y,x));
            }
            else
            {
                // octant 2
                return ANG90+ tantoangle(SlopeDiv(x,y));
            }
        }
        else
        {
            // y<0
            y = -y;

            if (x>y)
            {
                // octant 4
                return ANG180+tantoangle(SlopeDiv(y,x));
            }
            else
            {
                // octant 5
                return ANG270-1-tantoangle(SlopeDiv(x,y));
            }
        }
    }

    return 0;
}


angle_t
R_PointToAngle2 (fixed_t x1, fixed_t y1, fixed_t x2, fixed_t y2)
{
    viewx = x1;
    viewy = y1;

    return R_PointToAngle (x2, y2);
}







//
// R_InitPointToAngle
//
void R_InitPointToAngle (void)
{
    // UNUSED - now getting from tables.c
}



//
// R_ScaleFromGlobalAngle
// Returns the texture mapping scale
//  for the current line (horizontal span)
//  at the given angle.
// rw_distance must be calculated first.
//
fixed_t R_ScaleFromGlobalAngle (angle_t visangle)
{
    fixed_t        scale;
    int            anglea;
    int            angleb;
    int            sinea;
    int            sineb;
    //EMAC:
    fixed_t        num = 0;
    //EMAC:
    int            den = 0;

    anglea = ANG90 + (visangle-viewangle);
    angleb = ANG90 + (visangle-rw_normalangle);

    // both sines are allways positive
    sinea = finesine((anglea>>ANGLETOFINESHIFT));
    sineb = finesine((angleb>>ANGLETOFINESHIFT));
    //EMAC:
    if(sineb > 0) {
        num = FixedMul(projection, sineb) << detailshift;
    //EMAC:
    }
    //EMAC:
    if(sinea > 0) {
        den = FixedMul(rw_distance, sinea);
    //EMAC:
    }

    if (den > (num>>16))
    {
        scale = FixedDiv (num, den);

        if (scale > (FRACUNIT<<6))
            scale = (FRACUNIT<<6);
        else if (scale < 256)
            scale = 256;
    }
    else
        scale = (FRACUNIT << 6);

    return scale;
}



//
// R_InitTables
//
void R_InitTables (void)
{
    // UNUSED: now getting from tables.c
}



//
// R_InitTextureMapping
//
void R_InitTextureMapping (void)
{
    int            i;
    int            x;
    int            t;
    fixed_t        focallength;

    // Use tangent table to generate viewangletox:
    //  viewangletox will give the next greatest x
    //  after the view angle.
    //
    // Calc focallength
    //  so FIELDOFVIEW angles covers SCREENWIDTH.

    // FixedDiv(centerxfrac, finetangent[3072])
    // centerxfrac * 65536 / roughly 65536
    focallength = centerxfrac; //FixedDiv (centerxfrac, finetangentf(FINEANGLES/4+FIELDOFVIEW/2));

    for (i=0 ; i<FINEANGLES/2 ; i++)
    {
        if (finetangentf(i) > (FRACUNIT*2))
        {
            t = -1;
        }
		else if (finetangentf(i) < -(FRACUNIT*2))
        {
            t = viewwidth+1;
        }
        else
        {
            t = FixedMul (finetangentf(i), focallength);
            t = (centerxfrac - t+FRACUNIT-1)>>FRACBITS;

            if (t < -1)
                t = -1;
            else if (t>viewwidth+1)
                t = viewwidth+1;
        }

        viewangletox[i] = t;
    }

    // Scan viewangletox[] to generate xtoviewangle[]:
    //  xtoviewangle will give the smallest view angle
    //  that maps to x.
    for (x=0;x<=viewwidth;x++)
    {
        i = 0;
 
        while (viewangletox[i]>x)
            i++;

        xtoviewangle[x] = (i<<ANGLETOFINESHIFT)-ANG90;
    }

    // Take out the fencepost cases from viewangletox.
    for (i=0 ; i<FINEANGLES/2 ; i++)
    {
        // why was this code still here?
        //t = FixedMul (finetangent[i], focallength);
        //t = centerx - t;

        if (viewangletox[i] == -1)
            viewangletox[i] = 0;
        else if (viewangletox[i] == viewwidth+1)
            viewangletox[i]  = viewwidth;
    }

    clipangle = xtoviewangle[0];
}



//
// R_InitLightTables
// Only inits the zlight table,
//  because the scalelight table changes with view size.
//
#define DISTMAP        2

void R_InitLightTables (void)
{
    int        i;
    int        j;
    int        level;
    int        startmap;
    int        scale;

    // Calculate the light levels to use
    //  for each level / distance combination.
    for (i=0 ; i< LIGHTLEVELS ; i++)
    {
        startmap = ((LIGHTLEVELS-1-i)*2)*NUMCOLORMAPS/LIGHTLEVELS;
        for (j=0 ; j<MAXLIGHTZ ; j++)
        {
            scale = FixedDiv ((SCREENWIDTH/2*FRACUNIT), (j+1)<<LIGHTZSHIFT);
            scale >>= LIGHTSCALESHIFT;
            level = startmap - scale/DISTMAP;

            if (level < 0)
                level = 0;

            if (level >= NUMCOLORMAPS)
                level = NUMCOLORMAPS-1;

            zlight[i][j] = colormaps + level*256;
        }
    }
}


//
// R_SetViewSize
// Do not really change anything here,
//  because it might be in the middle of a refresh.
// The change will take effect next refresh.
//
boolean        setsizeneeded;
int        setblocks;
int        setdetail;


void
R_SetViewSize
( int        blocks,
  int        detail )
{
    setsizeneeded = true;
    setblocks = blocks;
    setdetail = detail;
}


//
// R_ExecuteSetViewSize
//
void R_ExecuteSetViewSize (void)
{
    fixed_t    cosadj;
    fixed_t    dy;
    int        i;
    int        j;
    int        level;
    int        startmap;
    setsizeneeded = false;

    if (setblocks == 11)
    {
        scaledviewwidth = SCREENWIDTH;
        viewheight = SCREENHEIGHT;
    }
    else
    {
        scaledviewwidth = setblocks << 5;
        viewheight = (setblocks*168 /10)&~7;
    }

    detailshift = setdetail;
    viewwidth = scaledviewwidth>>detailshift;

    centery = viewheight>>1;
    centerx = viewwidth>>1;
    centerxfrac = centerx<<FRACBITS;
    centeryfrac = centery<<FRACBITS;
    projection = centerxfrac;

    if (!detailshift)
    {
        colfunc = skycolfunc = basecolfunc = R_DrawColumn;
        fuzzcolfunc = R_DrawFuzzColumn;
        transcolfunc = R_DrawTranslatedColumn;
        spanfunc = R_DrawSpan;
    }
    else
    {
        colfunc = skycolfunc = basecolfunc = R_DrawColumnLow;
        fuzzcolfunc = R_DrawFuzzColumnLow;
        transcolfunc = R_DrawTranslatedColumnLow;
        spanfunc = R_DrawSpanLow;
    }

    R_InitBuffer (scaledviewwidth, viewheight);

    R_InitTextureMapping ();

    // psprite scales
    pspritescale = FRACUNIT * viewwidth/(SCREENWIDTH);
    pspriteiscale = FRACUNIT * (SCREENWIDTH)/viewwidth;

    // thing clipping
    for (i=0 ; i<viewwidth ; i++)
        screenheightarray[i] = viewheight;

    // planes
    for (i=0 ; i<viewheight ; i++)
    {
        dy = ((i-viewheight/2)<<FRACBITS)+FRACUNIT/2;
        dy = D_abs(dy);
        yslope[i] = FixedDiv ( (viewwidth<<detailshift)/2*FRACUNIT, dy);
    }

    for (i=0 ; i<viewwidth ; i++)
    {
        cosadj = D_abs(finecosine(xtoviewangle[i]>>ANGLETOFINESHIFT));
        distscale[i] = FixedDiv (FRACUNIT,cosadj);
    }

    // Calculate the light levels to use
    //  for each level / scale combination.
    for (i=0 ; i< LIGHTLEVELS ; i++)
    {
        startmap = ((LIGHTLEVELS-1-i)*2)*NUMCOLORMAPS/LIGHTLEVELS;

        for (j=0 ; j<MAXLIGHTSCALE ; j++)
        {
            level = startmap - j*(SCREENWIDTH)/(viewwidth<<detailshift)/DISTMAP;

            if (level < 0)
                level = 0;

            if (level >= NUMCOLORMAPS)
                level = NUMCOLORMAPS-1;

            scalelight[i][j] = colormaps + level*256;
        }
    }
}



//
// R_Init
//
extern int    detailLevel;
extern int    screenblocks;

//extern void setup_finetangent_fpconst();

void R_Init (void)
{
//    setup_finetangent_fpconst();
    R_InitData ();
    R_InitPointToAngle ();
    R_InitTables ();
    // viewwidth / viewheight / detailLevel are set by the defaults

    R_SetViewSize (screenblocks, detailLevel);
    R_InitPlanes ();
    R_InitLightTables ();
    R_InitSkyMap ();
    R_InitTranslationTables ();
}


//
// R_PointInSubsector
//
subsector_t*
R_PointInSubsector
( fixed_t    x,
  fixed_t    y )
{
    node_t*    node;
    int        side;
    int        nodenum;

    // single subsector is a special case
    if (!numnodes)
        return subsectors;

    nodenum = numnodes-1;

    while (!(nodenum & NF_SUBSECTOR))
    {
        node = &nodes[nodenum];
        side = R_PointOnSide (x, y, node);
        nodenum = node->children[side];
    }

    return &subsectors[nodenum & ~NF_SUBSECTOR];
}



//
// R_SetupFrame
//
void R_SetupFrame (player_t* player)
{
    int        i;

    viewplayer = player;
    viewx = player->mo->x;
    viewy = player->mo->y;
    viewangle = player->mo->angle + viewangleoffset;
    extralight = player->extralight;

    viewz = player->viewz;

    viewsin = finesine(viewangle>>ANGLETOFINESHIFT);
    viewcos = finecosine(viewangle>>ANGLETOFINESHIFT);

    sscount = 0;

    if (player->fixedcolormap)
    {
        fixedcolormap = colormaps + player->fixedcolormap*256*sizeof(lighttable_t);

        walllights = scalelightfixed;

        for (i=0 ; i<MAXLIGHTSCALE ; i++)
            scalelightfixed[i] = fixedcolormap;
    }
    else
        fixedcolormap = 0;

    validcount++;
}

//
// R_RenderView
//
void R_RenderPlayerView (player_t* player)
{
    R_SetupFrame (player);

    // Clear buffers.
    R_ClearClipSegs ();
    R_ClearDrawSegs ();
    R_ClearPlanes ();
    R_ClearSprites ();

    // check for new console commands.
    NetUpdate ();

    // The head node is the last node output.
    R_RenderBSPNode (numnodes-1);

    // Check for new console commands.
    NetUpdate ();

    R_DrawPlanes ();

    // Check for new console commands.
    NetUpdate ();

    R_DrawMasked ();

    // Check for new console commands.
    NetUpdate ();
}
