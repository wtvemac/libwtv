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
//    DOOM main program (D_DoomMain) and game loop (D_DoomLoop),
//    plus functions to determine game mode (shareware, registered),
//    parse command line parameters, configure game parameters (turbo),
//    and call the startup functions.
//
//-----------------------------------------------------------------------------
#define    BGCOLOR        7
#define    FGCOLOR        8

//EMAC:#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "libwtv.h"

#include "doomdef.h"
#include "doomstat.h"

#include "dstrings.h"
#include "sounds.h"

#include "z_zone.h"
#include "w_wad.h"
#include "s_sound.h"
#include "v_video.h"

#include "f_finale.h"
#include "f_wipe.h"

#include "m_argv.h"
#include "m_misc.h"
#include "m_menu.h"

#include "i_system.h"
#include "i_sound.h"
#include "i_video.h"

#include "g_game.h"

#include "hu_stuff.h"
#include "wi_stuff.h"
#include "st_stuff.h"
#include "am_map.h"

#include "p_setup.h"
#include "r_local.h"

#include "d_main.h"

char shareware_banner[]  =    
                "==================================\x0a\x0d"
                "            Shareware!\x0a\x0d"
                "==================================\x0a\x0d";
char commercial_banner[] =    
                "==================================\x0a\x0d"
                "Commercial product - do not distribute!\x0a\x0d"
                "Please report software piracy to the SPA:\x0a\x0d"
                "1-800-388-PIR8\x0a\x0d"
                "==================================\x0a\x0d";

extern int return_from_D_DoomMain;

unsigned long I_GetTime(void);    

extern surface_t* lockVideo(int wait);
extern void unlockVideo(surface_t* dc);

extern volatile uint64_t timekeeping;

extern surface_t *_dc;

volatile int should_sound = 0;

//
// D-DoomLoop()
// Not a globally visible function,
//  just included for source reference,
//  called by D_DoomMain, never exits.
// Manages timing and IO,
//  calls all ?_Responder, ?_Ticker, and ?_Drawer,
//  calls I_GetTime, I_StartFrame, and I_StartTic
//
void D_DoomLoop(void);

char*        wadfiles[MAXWADFILES];
char CHANGEME[] = "CHANGEME!";

extern boolean    inhelpscreens;

boolean    devparm;    // started game with -devparm
boolean    nomonsters;    // checkparm of -nomonsters
boolean    respawnparm;    // checkparm of -respawn
boolean    fastparm;    // checkparm of -fast

boolean    drone;

boolean    singletics = false; // debug flag to cancel adaptiveness

skill_t    startskill;
int        startepisode;
int        startmap;
boolean    autostart;

FILE*      debugfile;

boolean    advancedemo;

char       wadfile[1024];        // primary wad file
char       mapdir[1024];           // directory of development maps
char       basedefault[1024];      // default file

void D_CheckNetGame(void);
void D_ProcessEvents(void);
void G_BuildTiccmd(ticcmd_t* cmd);
void D_DoAdvanceDemo(void);

//
// EVENT HANDLING
//
// Events are asynchronous inputs generally generated by the game user.
// Events can be discarded if no responder claims them
//
event_t    events[MAXEVENTS];
int        eventhead;
int        eventtail;


//
// D_PostEvent
// Called by the I/O functions when input is detected
//
extern int current_player_for_input;
void D_PostEvent(event_t* ev)
{
    ev->player = current_player_for_input;
    events[eventhead] = *ev;

//d_main.c: In function 'D_PostEvent':
//d_main.c:186: warning: operation on 'eventhead' may be undefined
//    eventhead = (++eventhead)&(MAXEVENTS-1);
    eventhead += 1;
    eventhead &= (MAXEVENTS-1);
}


//
// D_ProcessEvents
// Send all the events of the given timestamp down the responder chain
//
void D_ProcessEvents (void)
{
    event_t* ev;

    // IF STORE DEMO, DO NOT ACCEPT INPUT
    if ( (gamemode == commercial) && (W_CheckNumForName("map01") < 0) )
    {
        return;
    }

    //d_main.c: In function 'D_ProcessEvents':
    //d_main.c:207: warning: operation on 'eventtail' may be undefined
//        for ( ; eventtail != eventhead ; eventtail = (++eventtail)&(MAXEVENTS-1) )
    for ( ; eventtail != eventhead ; eventtail = (eventtail+1)&(MAXEVENTS-1) )
    {
        ev = &events[eventtail];
        if (M_Responder(ev))
        {
            continue;               // menu ate the event
        }
        G_Responder(ev);
    }
}


//
// D_Display
//  draw current display, possibly wiping it from the previous
//

// wipegamestate can be set to -1 to force a wipe on the next draw
gamestate_t        wipegamestate = GS_DEMOSCREEN;
extern  boolean    setsizeneeded;
extern  int        showMessages;
void R_ExecuteSetViewSize(void);

void D_Display(void)
{
    static  boolean        viewactivestate = false;
    static  boolean        menuactivestate = false;
    static  boolean        inhelpscreensstate = false;
    static  boolean        fullscreen = false;
    static  gamestate_t    oldgamestate = -1;
    static  int            borderdrawcount;
            int            nowtime;
            int            tics;
            int            y;
            int            wipestart;
            boolean        done;
            boolean        wipe;
            boolean        redrawsbar;

    if (nodrawers)
    {
        // for comparative timing / profiling
        return;    
    }

    redrawsbar = false;

    // change the view size if needed
    if (setsizeneeded)
    {
        R_ExecuteSetViewSize();

        oldgamestate = -1;                      // force background redraw
        borderdrawcount = 3;
    }

    // save the current screen if about to wipe
    if (gamestate != wipegamestate)
    {
        wipe = true;
        wipe_StartScreen(0, 0, SCREENWIDTH, SCREENHEIGHT); 
    }
    else
    {
        wipe = false;
    }

    if ((gamestate == GS_LEVEL) && gametic)
    {
        HU_Erase();
    }

    // do buffered drawing
    switch (gamestate)
    {
        case GS_LEVEL:
        {
            if (!gametic)
            {
                break;
            }

            if (automapactive)
            {
                AM_Drawer();
            }

            if (wipe || ((viewheight != SCREENHEIGHT) && fullscreen))
            {
                redrawsbar = true;
            }

            if (inhelpscreensstate && !inhelpscreens)
            {
                redrawsbar = false; // just put away the help screen
            }   

            //if(!automapactive) 
            {
                ST_Drawer((viewheight == SCREENHEIGHT), redrawsbar );
            }

            fullscreen = (viewheight == SCREENHEIGHT);

            break;
        }

        case GS_INTERMISSION:
        {
            WI_Drawer();
            break;
        }

        case GS_FINALE:
        {
            F_Drawer();
            break;
        }

        case GS_DEMOSCREEN:
        {
            D_PageDrawer();
            break;
        }
    }

    // draw the view directly
    if ((gamestate == GS_LEVEL) && !automapactive && gametic)
    {
        R_RenderPlayerView(&players[displayplayer]);
    }

    if ((gamestate == GS_LEVEL) && gametic)
    {
        HU_Drawer();
    }

    // clean up border stuff
    if ((gamestate != oldgamestate) && (gamestate != GS_LEVEL))
    {
        I_SetPalette(W_CacheLumpName ("PLAYPAL",PU_CACHE));
    }

    // see if the border needs to be initially drawn
    if ((gamestate == GS_LEVEL) && (oldgamestate != GS_LEVEL))
    {
        viewactivestate = false; // view was not active
    }

    // see if the border needs to be updated to the screen
    if ((gamestate == GS_LEVEL) && !automapactive && (scaledviewwidth != SCREENWIDTH))
    {
        if (menuactive || menuactivestate || !viewactivestate)
        {
            borderdrawcount = 3;
        }

        if (borderdrawcount)
        {
            R_DrawViewBorder(); // erase old menu stuff
            borderdrawcount--;
        }
    }

    menuactivestate = menuactive;
    viewactivestate = viewactive;
    inhelpscreensstate = inhelpscreens;
    oldgamestate = wipegamestate = gamestate;

    // draw pause pic
    if (paused)
    {
        if (automapactive)
        {
            y = 4;
        }
        else
        {
            y = viewwindowy+4;
        }

        V_DrawPatch(viewwindowx+((scaledviewwidth-68)/2),y,W_CacheLumpName("M_PAUSE", PU_CACHE));
    }

    // menus go directly to the screen
    M_Drawer(); // menu is drawn even on top of everything

    NetUpdate(); // send out any new accumulation

    if (!wipe)
    {
        I_FinishUpdate();
        return;
    }

    // wipe update
    wipe_EndScreen(0, 0, SCREENWIDTH, SCREENHEIGHT);

    wipestart = I_GetTime () - 1;

    do
    {
        do
        {
            nowtime = I_GetTime ();
            tics = nowtime - wipestart;
        }
        while (!tics);

        wipestart = nowtime;
        done = wipe_ScreenWipe(wipe_Melt, 0, 0, SCREENWIDTH, SCREENHEIGHT, tics);

        M_Drawer ();    // menu is drawn even on top of wipes

        I_FinishUpdate();
        I_StartFrame();
    }
    while (!done);    

    I_FinishUpdate();
}

// use this to hold _dc->buffer pointer whenever we get a surface in D_DoomLoop
// now each R_DrawColumn/R_DrawSpan call only needs one "lw" instruction to get screen
// instead of two
// saves (num cols + num spans) "lw" instructions per rendered frame
// that is 1000+ loads per frame
void *bufptr;

//
//  D_DoomLoop
//
void D_DoomLoop(void)
{
    I_SetPalette(W_CacheLumpName ("PLAYPAL",PU_CACHE));

    while (!return_from_D_DoomMain)
    {
        I_StartFrame();
        // process one or more tics
        if (singletics)
        {
            I_StartTic();
            D_ProcessEvents();
            G_BuildTiccmd(&netcmds[consoleplayer][maketic%BACKUPTICS]);
            if (advancedemo)
            {
                D_DoAdvanceDemo();
            }
            M_Ticker();
            G_Ticker();
            gametic++;
            maketic++;
        }
        else
        {
            TryRunTics(); // will run at least one tic
        }

        S_UpdateSounds(players[consoleplayer].mo);// move positional sounds

        D_Display();
    }
}


//
//  DEMO LOOP
//
int             demosequence;
int             pagetic;
char            *pagename;


//
// D_PageTicker
// Handles timing for warped projection
//
void D_PageTicker(void)
{
    if (--pagetic < 0)
    {
        D_AdvanceDemo();
    }
}


//
// D_PageDrawer
//
void D_PageDrawer(void)
{
    V_DrawPatch(0,0,W_CacheLumpName(pagename, PU_CACHE));
}


//
// D_AdvanceDemo
// Called after each demo or intro demosequence finishes
//
void D_AdvanceDemo (void)
{
    advancedemo = true;
}


//
// This cycles through the demo sequences.
// FIXME - version dependend demo numbers?
//
void D_DoAdvanceDemo(void)
{
    players[consoleplayer].playerstate = PST_LIVE;  // not reborn
    advancedemo = false;
    usergame = false;               // no save / end game here
    paused = false;
    gameaction = ga_nothing; // EMAC: default
    //gameaction = ga_newgame; // EMAC: start on game to test controls.

    if (gamemode == retail)
    {
        demosequence = (demosequence+1)%7;
    }
    else
    {
        demosequence = (demosequence+1)%6;
    }

    switch (demosequence)
    {
        case 0:
        {
            if (gamemode == commercial)
            {
                pagetic = TICRATE * 11;
            }
            else
            {
                pagetic = 170;
            }
            gamestate = GS_DEMOSCREEN;
            pagename = "TITLEPIC";
            if (gamemode == commercial)
            {
                S_StartMusic(mus_dm2ttl);
            }
            else
            {
                S_StartMusic(mus_intro);
            }
            break;
        }
        case 1:
        {
            G_DeferedPlayDemo("demo1");
            break;
        }
        case 2:
        {
            pagetic = 200;
            gamestate = GS_DEMOSCREEN;
            pagename = "CREDIT";
            break;
        }
        case 3:
        {
            G_DeferedPlayDemo("demo2");
            break;
        }
        case 4:
        {
            gamestate = GS_DEMOSCREEN;
            if (gamemode == commercial)
            {
                pagetic = TICRATE * 11;
                pagename = "TITLEPIC";
                S_StartMusic(mus_dm2ttl);
            }
            else
            {
                pagetic = 200;

                if (gamemode == retail)
                {
                   pagename = "CREDIT";
                }
                else
                {
                    pagename = "HELP2";
                }
            }
            break;
        }
        case 5:
        {
            G_DeferedPlayDemo("demo3");
            break;
        }
        // THE DEFINITIVE DOOM Special Edition demo
        case 6:
        {
            G_DeferedPlayDemo("demo4");
            break;
        }
    }
}


//
// D_StartTitle
//
void D_StartTitle(void)
{
    gameaction = ga_nothing;
    demosequence = -1;
    D_AdvanceDemo();
}


//      print title for every printed line
char title[128];

//
// D_AddFile
//
void D_AddFile(char *file)
{
    int numwadfiles;
    for (numwadfiles = 0 ; wadfiles[numwadfiles] ; numwadfiles++)
    {
        ;
    }
    wadfiles[numwadfiles] = file;
}

static char __attribute__((aligned(8))) gameid[16];
char *get_GAMEID()
{
    //EMAC:
    /*
    int fd = dfs_open("identifier");
    if (fd < 0)
    {
        I_Error("get_GAMEID: DFS could not open identifier file.\x0a\x0d");
    }
    size_t len = dfs_size(fd);
    if (len < 0 || 16 < len)
    {
        I_Error("get_GAMEID: DFS returned invalid size for identifiler file after opening.\x0a\x0d");
    }
    memset(gameid,0,16);

    if (len != dfs_read(gameid, sizeof(char), len, fd))
    {
        I_Error("get_GAMEID: DFS could not read identifier file after opening.\x0a\x0d");
    }
    dfs_close(fd);
    */
    //EMAC:
    size_t len = 10;
    gameid[0] = 'D';
    gameid[1] = 'O';
    gameid[2] = 'O';
    gameid[3] = 'M';
    gameid[4] = '1';
    gameid[5] = '.';
    gameid[6] = 'W';
    gameid[7] = 'A';
    gameid[8] = 'D';
    gameid[9] = '\0';


    // deal with newlines or other stray characters after the end of the filename
    for (size_t i=0;i<len;i++)
    {
        if (!(
            ('a' <= gameid[i] && gameid[i] <= 'z') ||
            ('A' <= gameid[i] && gameid[i] <= 'Z') ||
            ('0' <= gameid[i] && gameid[i] <= '9') ||
            ('.' == gameid[i]) ))
        {
            gameid[i] = '\0';
            break;
        }
    }

    return gameid;
}

//EMAC:#define stricmp strcasecmp
extern GameMode_t current_mode;

//
// IdentifyVersion
// Checks availability of IWAD files by name,
// to determine whether registered/commercial features
// should be executed (notably loading PWAD's).
//
char*    doom1wad = "DOOM1.WAD";
char*    doomwad = "DOOM.WAD";
char*    doomuwad = "DOOMU.WAD";
char*    doom2wad = "DOOM2.WAD";

char*    doom2fwad = "DOOM2F.WAD";
char*    plutoniawad = "PLUTONIA.WAD";
char*    tntwad = "TNT.WAD";

void IdentifyVersion(void)
{
    const char *gameid = get_GAMEID();

    printf("IdentifyVersion: %s\x0a\x0d", gameid);
    
    if (!stricmp(doom2fwad,gameid))
    {
        gamemode = commercial;
        // C'est ridicule!
        // Let's handle languages in config files, okay?
        language = french;
        D_AddFile(doom2fwad);
        return;
    }

    if (!stricmp(doom2wad,gameid))
    {
        gamemode = commercial;
        current_mode = commercial;
        D_AddFile(doom2wad);
        return;
    }

    if (!stricmp(plutoniawad,gameid))
    {
        gamemode = commercial;
        current_mode = pack_plut;
        D_AddFile(plutoniawad);
        return;
    }

    if (!stricmp(tntwad,gameid))
    {
        gamemode = commercial;
        current_mode = pack_tnt;
        D_AddFile(tntwad);
        return;
    }

    if (!stricmp(doomuwad,gameid))
    {
        gamemode = retail;
        current_mode = commercial;
        D_AddFile(doomuwad);
        return;
    }

    if (!stricmp(doomwad,gameid))
    {
        gamemode = registered;
        current_mode = commercial;
        D_AddFile(doomwad);
        return;
    }

    if (!stricmp(doom1wad,gameid))
    {
        gamemode = shareware;
        current_mode = commercial;
        D_AddFile(doom1wad);
        return;
    }

    printf("Game mode indeterminate.\x0a\x0d");
    gamemode = indetermined;
    current_mode = commercial;

    // We don't abort. Let's see what the PWAD contains.
    //exit(1);
    //I_Error ("Game mode indeterminate\x0a\x0d");*/
}

//
// D_DoomMain
//
void D_DoomMain(void)
{
    IdentifyVersion ();

    modifiedgame = false;

    nomonsters = M_CheckParm("-nomonsters");
    respawnparm = M_CheckParm("-respawn");
    fastparm = M_CheckParm("-fast");
    devparm = M_CheckParm("-devparm");
    if (M_CheckParm("-altdeath"))
    {
        deathmatch = 2;
    }
    else if (M_CheckParm ("-deathmatch"))
    {
        deathmatch = 1;
    }

    if (current_mode == pack_plut) {
    sprintf (title,
         "DOOM 2: Plutonia Experiment v%i.%i",
         VERSION/100,VERSION%100);
    }
    else if (current_mode == pack_tnt) {
            sprintf (title,
         "DOOM 2: TNT - Evilution v%i.%i",
         VERSION/100,VERSION%100);
    }
    else {
    switch (gamemode)
    {
        case retail:
    sprintf (title,
         "The Ultimate DOOM Startup v%i.%i",
         VERSION/100,VERSION%100);
    break;
      case shareware:
    sprintf (title,
         "DOOM Shareware Startup v%i.%i",
         VERSION/100,VERSION%100);
    break;
      case registered:
    sprintf (title,
         "DOOM Registered Startup v%i.%i",
         VERSION/100,VERSION%100);
    break;
      case commercial:
    sprintf (title,
         "DOOM 2: Hell on Earth v%i.%i",
         VERSION/100,VERSION%100);
    break;
      default:
    sprintf (title,
         "Public DOOM - v%i.%i",
         VERSION/100,VERSION%100);
    break;
    }
    }

    printf ("%s\x0a\x0d",title);

    // init subsystems
    printf ("V_Init: allocate screens.\x0a\x0d");
    V_Init ();

    printf ("M_LoadDefaults: Load system defaults.\x0a\x0d");
    M_LoadDefaults (); // load before initing other systems

    printf ("Z_Init: Init zone memory allocation daemon. \x0a\x0d");
    Z_Init ();

    printf ("W_Init: Init WADfiles.\x0a\x0d");
    W_InitMultipleFiles (wadfiles);

    // Check and print which version is executed.
    switch ( gamemode )
    {
      case shareware: 
      case indetermined:
    printf ( "%s", shareware_banner);
    break;
      case registered:
      case retail:
      case commercial:
    printf ( "%s", commercial_banner);
    break;

      default:
    // Ouch.
    break;
    }
    switch(gamemode) {
        case shareware:
        printf("Game mode: shareware\x0a\x0d");
        break;
        case registered:
        printf("Game mode: registered\x0a\x0d");
        break;
        case retail:
        printf("Game mode: retail\x0a\x0d");
        break;
        case commercial:
        printf("Game mode: commercial\x0a\x0d");
        break;
        default:
        break;
    }
    printf ("M_Init: Init miscellaneous info.\x0a\x0d");
    M_Init ();

    // clear the console as part of clearing screen before game starts
    console_close();
    
    printf ("R_Init: Init DOOM refresh daemon - \x0a\x0d");
    R_Init ();

    printf ("\x0a\x0dP_Init: Init Playloop state.\x0a\x0d");
    P_Init ();

    printf ("I_Init: Setting up machine state.\x0a\x0d");
    I_Init ();
    
    printf ("D_CheckNetGame: Checking network game status.\x0a\x0d");
    D_CheckNetGame ();

    printf ("S_Init: Setting up sound.\x0a\x0d");
    //EMAC:S_Init (snd_SfxVolume /* *8 */, snd_MusicVolume /* *8*/ );

    printf ("HU_Init: Setting up heads up display.\x0a\x0d");
    HU_Init ();

    printf ("ST_Init: Init status bar.\x0a\x0d");
    ST_Init ();
    
    if (gameaction != ga_loadgame)
    {
        if (autostart || netgame)
        {
            G_InitNew(startskill, startepisode, startmap);
        }
        else
        {
            D_StartTitle(); // start up intro loop
        }
    }

    // clear screen before game starts
    for(int i=0;i<2;i++)
    {
        _dc = lockVideo(1);
        memset(_dc->buffer, 0, SCREENWIDTH*SCREENHEIGHT*2);
        unlockVideo(_dc);
    }

    D_DoomLoop();  // never returns
}
