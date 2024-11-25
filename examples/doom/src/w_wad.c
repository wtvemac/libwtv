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
//    Handles WAD file header, directory, lump I/O.
//
//-----------------------------------------------------------------------------


#include "libwtv.h"

#include "miniwad.h"
//EMAC:#include "doomwad.h"

//#include "gamma.h"
#include "resoluti.h"
#include "rzhigh.h"
#include "rzlow.h"
#include "vidttl.h"
#include "videoset.h"

//EMAC:#include <ctype.h>
#include <sys/types.h>
#include <string.h>
//EMAC:#include <unistd.h>
//EMAC:#include <fcntl.h>
#include <sys/stat.h>
#include <alloca.h>

#include "doomdef.h"

#include "doomtype.h"
#include "m_swap.h"
#include "i_system.h"
#include "z_zone.h"

#ifdef __GNUG__
#pragma implementation "w_wad.h"
#endif
#include "w_wad.h"

#include "hash.h"

static hashtable_t ht;


void wadupr(char *s)
{
    int    i;

    for (i=0;i<8;i++)
    {
        if (s[i] == '\0')
        {
            return;
        }

        s[i] = toupper(s[i]);
    }
}

//
// GLOBALS
//

// Location of each lump on disk.
lumpinfo_t*    lumpinfo;
int            numlumps;

void**         lumpcache;


//EMAC:#define strcmpi strcasecmp

void ExtractFileBase(char* path, char* dest)
{
    char*    src;
#ifdef RANGECHECK    
    int      length;
#endif

    src = path + strlen(path) - 1;

    // back up until a \ or the start
    while ((src != path) && (*(src-1) != '\\') && (*(src-1) != '/'))
    {
        src--;
    }

    // copy up to eight characters
    memset(dest, 0, 8);
#ifdef RANGECHECK
    length = 0;
#endif

    while ((*src) && (*src != '.'))
    {
#ifdef RANGECHECK
        if (++length == 9)
        {
            I_Error("Filename base of %s >8 chars", path);
        }
#endif
        *dest++ = toupper((int)*src++);
    }
}


//
// LUMP BASED ROUTINES.
//

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
//
// If filename starts with a tilde, the file is handled
//  specially to allow map reloads.
// But: the reload feature is a fragile hack...


int      reloadlump;
char*    reloadname;


// Hash table for fast lookups
int comp_keys(void *el1, void *el2)
{
    char *t1 = ((lumpinfo_t *)el1)->name;
    char *t2 = ((lumpinfo_t *)el2)->name;
    int i;

    for (i=0;i<8;i++)
    {
        if (t1[i] != t2[i])
            return 1;
    }

    return 0;
}


unsigned long int W_LumpNameHash(char *s)
{
    // This is the djb2 string hash function, modded to work on strings
    // that have a maximum length of 8.
    unsigned long int result = 5381;
    unsigned long int i;

    for (i=0; i < 8 && s[i] != '\0'; ++i)
    {
        result = ((result << 5) ^ result ) ^ toupper(s[i]);
    }

    return result;
}


unsigned long int hash(void *element, void *params)
{
    wadupr(((lumpinfo_t *)element)->name);
    return W_LumpNameHash(((lumpinfo_t *)element)->name) & 255;
}


// lumps for 64Doom-specific menu graphics (created with SLADE)
static void* GAMMA_lmp;
static void* RZHIGH_lmp;
static void* RZLOW_lmp;
static void* VIDTTL_lmp;
static void* VIDEOSET_lmp;
static void* RESOLUTI_lmp;     

//EMAC:
/*
#define LOAD_MENU_LUMP(menulump_name,menulump_ptr) \
    {\
    int menulump_fd = dfs_open(menulump_name); \
    if (menulump_fd < 0) I_Error("W_Init: missing %s.\x0a\x0d", menulump_name); \
    size_t menulump_len = dfs_size(menulump_fd); \
    menulump_ptr = malloc(menulump_len); \
    if (menulump_ptr == NULL) I_Error("W_Init: could not allocate memory for %s.\x0a\x0d", menulump_name); \
    size_t menulump_read = dfs_read(menulump_ptr, sizeof(uint8_t), menulump_len, menulump_fd); \
    if (menulump_len != menulump_read) I_Error("W_Init: could not read lump data for %s.\x0a\x0d", menulump_name); \
    dfs_close(menulump_fd); \
    }
*/

void W_Init (void)
{
    reloadname = 0;
    hashtable_init(&ht, 256, comp_keys, hash, 0);

    /*
    GAMMA_lmp = malloc(gamma_bin_len);
    memcpy(GAMMA_lmp, &gamma_bin, gamma_bin_len);

    //EMAC:LOAD_MENU_LUMP("./menulumps/gamma.bin",GAMMA_lmp);
    GAMMA_lmp = malloc(rzhigh_bin_len);
    memcpy(GAMMA_lmp, &rzhigh_bin, rzhigh_bin_len);

    //EMAC:LOAD_MENU_LUMP("./menulumps/rzhigh.bin",RZHIGH_lmp);
    GAMMA_lmp = malloc(rzlow_bin_len);
    memcpy(GAMMA_lmp, &rzlow_bin, rzlow_bin_len);

    //EMAC: LOAD_MENU_LUMP("./menulumps/rzlow.bin",RZLOW_lmp);
    GAMMA_lmp = malloc(vidttl_bin_len);
    memcpy(GAMMA_lmp, &vidttl_bin, vidttl_bin_len);

    //EMAC:LOAD_MENU_LUMP("./menulumps/vidttl.bin",VIDTTL_lmp);
    GAMMA_lmp = malloc(videoset_bin_len);
    memcpy(GAMMA_lmp, &videoset_bin, videoset_bin_len);

    //EMAC:LOAD_MENU_LUMP("./menulumps/videoset.bin",VIDEOSET_lmp);
    GAMMA_lmp = malloc(resoluti_bin_len);
    memcpy(GAMMA_lmp, &resoluti_bin, resoluti_bin_len);

    //EMAC:LOAD_MENU_LUMP("./menulumps/resoluti.bin",RESOLUTI_lmp);
    */
}


void W_AddFile(char *filename)
{
    wadinfo_t      header;
    lumpinfo_t*    lump_p;
    unsigned int   i;
    int            handle = -1;
    int            length;
    int            startlump;
    filelump_t*    fileinfo;
    int            storehandle;

    // open the file and add to directory

    // handle reload indicator.
    if (filename[0] == '~')
    {
        filename++;
        reloadname = filename;
        reloadlump = numlumps;
    }

    startlump = numlumps;

    // WAD file
#ifdef RANGECHECK
    size_t size_r =    
#endif    
    memcpy(&header, &miniwad_wad, sizeof(header));

    header.numlumps = LONG(header.numlumps);
    header.infotableofs = LONG(header.infotableofs);
    length = header.numlumps*sizeof(filelump_t);
    fileinfo = alloca(length);

memcpy(fileinfo, &miniwad_wad[header.infotableofs], length);

    numlumps += header.numlumps;

    lumpinfo = (lumpinfo_t *)malloc(numlumps*sizeof(lumpinfo_t));
    if (!lumpinfo)
    {
        I_Error("W_AddFile: Couldn't realloc lumpinfo\x0a\x0d");
    }

    lump_p = &lumpinfo[startlump];

    storehandle = reloadname ? -1 : handle;

    // hash the lumps
    for (i=startlump ; i<numlumps ; i++,lump_p++, fileinfo++)
    {
        lump_p->handle = storehandle;
        lump_p->position = LONG(fileinfo->filepos);
        lump_p->size = LONG(fileinfo->size);

        strncpy(lump_p->name, fileinfo->name, 8);

        hashtable_insert(&ht, (void*)lump_p, -1);
    }
    return;

    //EMAC:
    /*

    // open the file and add to directory

    // handle reload indicator.
    if (filename[0] == '~')
    {
        filename++;
        reloadname = filename;
        reloadlump = numlumps;
    }

    handle = dfs_open(filename);
#ifdef RANGECHECK
    if (-1 == handle)
    {
        I_Error("W_AddFile: DFS could not open file \"%s\"\x0a\x0d",filename);
    }
#endif    
    startlump = numlumps;

    // WAD file
#ifdef RANGECHECK
    size_t size_r =    
#endif    
    dfs_read(&header, sizeof(header), 1, handle);
#ifdef RANGECHECK
    if (sizeof(header) != size_r)
    {
        I_Error("W_AddFile: DFS failed to read WAD header after opening.\x0a\x0d");
    }    
#endif

#ifdef RANGECHECK        
    if (strncmp(header.identification,"IWAD",4))
    {
        // Homebrew levels?
        if (strncmp(header.identification,"PWAD",4))
        {
            I_Error("W_AddFile: %s != IWAD/PWAD in %s\x0a\x0d", header.identification, filename);
        }
        // ???modifiedgame = true;
    }
#endif

    header.numlumps = LONG(header.numlumps);
    header.infotableofs = LONG(header.infotableofs);
    length = header.numlumps*sizeof(filelump_t);
    fileinfo = alloca(length);
#ifdef RANGECHECK
    if (0 == fileinfo)
    {
        I_Error("W_AddFile: unable to allocate memory for fileinfo read.\x0a\x0d");
    }
#endif

#ifdef RANGECHECK
    int sr = 
#endif
    dfs_seek(handle, header.infotableofs, SEEK_SET);
#ifdef RANGECHECK
    if (DFS_ESUCCESS != sr)
    {
        I_Error("W_AddFile: Error while seeking to infotableofs.\x0a\x0d");
    }
#endif

#ifdef RANGECHECK
    size_r = 
#endif
    dfs_read(fileinfo, sizeof(uint8_t), length, handle);
#ifdef RANGECHECK
    if (length != size_r)
    {
        I_Error("W_AddFile: Error while reading fileinfo.\x0a\x0d");
    }
#endif

    numlumps += header.numlumps;

    lumpinfo = (lumpinfo_t *)realloc(lumpinfo, numlumps*sizeof(lumpinfo_t));
    if (!lumpinfo)
    {
        I_Error("W_AddFile: Couldn't realloc lumpinfo");
    }

    lump_p = &lumpinfo[startlump];

    storehandle = reloadname ? -1 : handle;

    // hash the lumps
    for (i=startlump ; i<numlumps ; i++,lump_p++, fileinfo++)
    {
        lump_p->handle = storehandle;
        lump_p->position = LONG(fileinfo->filepos);
        lump_p->size = LONG(fileinfo->size);

        strncpy(lump_p->name, fileinfo->name, 8);

        hashtable_insert(&ht, (void*)lump_p, -1);
    }

    if (reloadname)
    {
        dfs_close(handle);
    }
    */
}


//
// W_Reload
// Flushes any of the reloadable lumps in memory
//  and reloads the directory.
//
void W_Reload (void)
{
    wadinfo_t      header;
    int            lumpcount;
    lumpinfo_t*    lump_p;
    unsigned       i;
    int            handle;
    int            length;
    filelump_t*    fileinfo;

    if (!reloadname)
    {
        return;
    }

    printf("EMAC: W_Reload: %s\x0a\x0d", reloadname);

    //EMAC:
    /*

    if ( (handle = dfs_open(reloadname)) == -1 )
    {
        I_Error("W_Reload: couldn't open %s", reloadname);
    }

    dfs_seek(handle, 0, SEEK_SET);

    dfs_read(&header, sizeof(header), 1, handle);

    lumpcount = LONG(header.numlumps);
    header.infotableofs = LONG(header.infotableofs);
    length = lumpcount*sizeof(filelump_t);
    fileinfo = alloca(length);

    dfs_seek(handle, header.infotableofs, SEEK_SET);
    dfs_read(fileinfo, sizeof(filelump_t), lumpcount, handle);

    // Fill in lumpinfo
    lump_p = &lumpinfo[reloadlump];

    for (i=reloadlump ; i<reloadlump+lumpcount; i++,lump_p++,fileinfo++)
    {
        if (lumpcache[i])
        {
            Z_Free(lumpcache[i]);
        }

        lump_p->position = LONG(fileinfo->filepos);
        lump_p->size = LONG(fileinfo->size);
    }
    */
}


//
// W_InitMultipleFiles
// Pass a null terminated list of files to use.
// All files are optional, but at least one file
//  must be found.
// Files with a .wad extension are idlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
// Lump names can appear multiple times.
// The name searcher looks backwards, so a later file
//  does override all earlier ones.
//
void W_InitMultipleFiles(char** filenames)
{
    int        size;

    W_Init();

    // open all the files, load headers, and count lumps
    numlumps = 0;
    // will be realloced as lumps are added
    lumpinfo = (lumpinfo_t *)malloc(1);

    for ( ; *filenames ; filenames++)
    {
        printf("W_InitMultipleFiles: adding %s\x0a\x0d", *filenames);

        W_AddFile(*filenames);
    }

#ifdef RANGECHECK
    if (!numlumps)
    {
        I_Error("W_InitMultipleFiles: no files found");
    }
#endif

    // set up caching
    size = numlumps * sizeof(*lumpcache);
    lumpcache = (void **)malloc (size);
    printf("lumpcache=%p\x0a\x0d", lumpcache);

#ifdef RANGECHECK
    if (!lumpcache)
    {
        I_Error("W_InitMultipleFiles: Couldn't allocate lumpcache");
    }
#endif

    memset(lumpcache, 0, size);
}


//
// W_InitFile
// Just initialize from a single file.
//
void W_InitFile (char* filename)
{
    char*    names[2];

    names[0] = filename;
    names[1] = NULL;
    W_InitMultipleFiles(names);
}


//
// W_NumLumps
//
int W_NumLumps(void)
{
    return numlumps;
}


//
// W_CheckNumForName
// Returns -1 if name not found.
//
int W_CheckNumForName (char* name)
{
    lumpinfo_t *testlump = (lumpinfo_t *)alloca(sizeof(lumpinfo_t));
#ifdef RANGECHECK
    if (0 == testlump)
    {
        I_Error("W_CheckNumForName: Could not allocate memory for hashtable check.\x0a\x0d");
    }        
#endif
    strncpy(testlump->name, name, 8);
    wadupr(testlump->name);

    void *ret_node;
    lumpinfo_t *retlump;
    retlump = (lumpinfo_t *)is_in_hashtable( &ht, testlump, &ret_node );
    
    if (!retlump)
    {
        return -1;
    }

    return retlump - lumpinfo;
}


//
// W_GetNumForName
// Calls W_CheckNumForName.
// It is ok to return -1.
//
int W_GetNumForName (char* name)
{
    int    i;

    i = W_CheckNumForName (name);

    return i;
}


//
// W_LumpLength
// Returns the buffer size needed to load the given lump.
//
int W_LumpLength (int lump)
{
#ifdef RANGECHECK    
    if (lump >= numlumps)
    {
        I_Error("W_LumpLength: %i >= numlumps", lump);
    }
#endif
    return lumpinfo[lump].size;
}


//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//
void W_ReadLump (int lump, void* dest)
{
    lumpinfo_t*    l;

    l = lumpinfo+lump;

    //EMAC:printf("W_ReadLump: size=%08x, position=%08x\x0a\x0d", l->size, l->position);

    memcpy(dest, &miniwad_wad[l->position], l->size);


    //EMAC:
    /*
#ifdef RANGECHECK
    int            c;
#endif
    lumpinfo_t*    l;
    int            handle;

#ifdef RANGECHECK
    if (lump >= numlumps)
    {
        I_Error("W_ReadLump: %i >= numlumps", lump);
    }
#endif
    l = lumpinfo+lump;

    if (l->handle == -1)
    {
        // reloadable file, so use open / read / close
        if ((handle = dfs_open(reloadname)) == -1 )
        {
            I_Error("W_ReadLump: couldn't open %s", reloadname);
        }
    }
    else
    {
        handle = l->handle;
    }

    dfs_seek(handle, l->position, SEEK_SET);

#ifdef RANGECHECK
    c =
#endif
    dfs_read(dest, sizeof(uint8_t), l->size, handle);

#ifdef RANGECHECK
    if (l->size != c)
    {
        I_Error("W_ReadLump: only read %i of %i on lump %i", c, l->size, lump);
    }
#endif
    if(l->handle == -1)
    {
        dfs_close(handle);
    }
    */
}


//
// W_CacheLumpNum
//
void* W_CacheLumpNum (int lump, int tag)
{
    //EMAC:printf("W_CacheLumpNum1: lump=%08x\x0a\x0d", lump);
    //EMAC:
    if(lump < 0) {
        return NULL;
    }

#ifdef RANGECHECK
    byte*    ptr;
    if ((unsigned)lump >= numlumps)
    {
        I_Error("W_CacheLumpNum: %i >= numlumps", lump);
    }
#endif    
    if (!lumpcache[lump])
    {
        // read the lump in
#ifdef RANGECHECK
        ptr = 
#endif        
        Z_Malloc(W_LumpLength(lump), tag, &lumpcache[lump]);
#ifdef RANGECHECK
        if (ptr == NULL || ptr != lumpcache[lump])
        {
            I_Error("W_CacheLumpNum: !=cache allocation error on lump %i\x0a\x0d", lump);
        }
#endif
    //EMAC:printf("W_ReadLump: lump=%08x lumpcache[lump]=%p\x0a\x0d", lump, &lumpcache[lump]);
        W_ReadLump(lump, lumpcache[lump]);
    }
    else
    {
        Z_ChangeTag(lumpcache[lump],tag);
    }

    return lumpcache[lump];
}

//
// W_CacheLumpName
//
void* W_CacheLumpName (char* name, int tag)
{
    int numforname = W_GetNumForName(name);

    if(numforname >= 0)
    {
        return W_CacheLumpNum(numforname, tag);
    }
    // these lumps get loaded from DFS in W_Init
    else
    {
        if (0 == strncmp(name,"X_G",3))
        {
            return (void *)GAMMA_lmp;
        }
        else if (0 == strncmp(name,"X_RZH",5))
        {
            return (void *)RZHIGH_lmp;
        }
        else if (0 == strncmp(name,"X_RZL",5))
        {
            return (void *)RZLOW_lmp;
        }
        else if (0 == strncmp(name, "X_VIDT", 6))
        {
            return (void *)VIDTTL_lmp;
        }
        else if (0 == strncmp(name,"X_VI",4))
        {
            return (void *)VIDEOSET_lmp;
        }
        else if (0 == strncmp(name, "X_RESOLU",8))
        {
            return (void *)RESOLUTI_lmp;
        }
        else
        {
            return W_CacheLumpNum(W_GetNumForName(name), tag);
        }
    }
}
