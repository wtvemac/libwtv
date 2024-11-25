#include "sprite.h"
#include "wtvsys.h"
//EMAC:#include "debug.h"
#include "surface.h"
#include "sprite_internal.h"
#include "asset_internal.h"
#include "asset.h"
#include "utils.h"
#include "libc.h"
#include <string.h>
#include <assert.h>

static sprite_t *last_spritemap = NULL;

/** @brief Access the sprite extended structure, or NULL if the structure does not exist */
__attribute__((noinline))
sprite_ext_t *__sprite_ext(sprite_t *sprite)
{
    if (!(sprite->flags & SPRITE_FLAGS_EXT))
        return NULL;

    uint8_t *data = (uint8_t*)sprite->data;
    pixel_mode_t format = sprite_get_format(sprite);
    data += ROUND_UP(TEX_FORMAT_PIX2BYTES(format, sprite->width) * sprite->height, 8);

    // Access extended header
    sprite_ext_t *sx = (sprite_ext_t*)data;
    //EMAC:assertf(sx->version == 4, "Invalid sprite version (%d); please regenerate your asset files", sx->version);
    return sx;
}

bool __sprite_upgrade(sprite_t *sprite)
{
    // Check if the sprite header begins with ASSET_MAGIC, which indicates a 
    // compressed sprite loaded with old-style file reading. In this case, we
    // can emit an assertion.
    //EMAC:assertf(memcmp(sprite, ASSET_MAGIC, 3) != 0, 
    //    "Sprite is compressed: use sprite_load() instead of reading the file manually");

    // Previously, the "format" field of the sprite structure (now renamed "flags")
    // was unused and always contained 0. Sprites could only be RGBA16 and RGBA32 anyway,
    // so only a bitdepth field could be used to understand the format.
    // To help backward compatibility, we want to try and still support this
    // old format.
    // Notice also that it is not enough to do this in sprite_load, because
    // sprite_load didn't exist at the time, and sprites were loaded manually
    // via fopen/fread.
    if (sprite->flags == 0) {
        // Read the bitdepth field without triggering the deprecation warning
        uint8_t bitdepth = ((uint8_t*)sprite)[4];
        sprite->flags = bitdepth == 2 ? FMT_YUV16 : FMT_YUV32;
        return true;
    }
    return false;
}

sprite_t *sprite_load_buf(void *buf, int sz)
{
    sprite_t *s = buf;
    //EMAC:assertf(sz >= sizeof(sprite_t), "Sprite buffer too small (sz=%d)", sz);
    __sprite_upgrade(s);
    (void)__sprite_ext(s); // just check if the sprite is valid (the version is checked in __sprite_ext)
    data_cache_hit_writeback(s, sz);
    return s;
}

sprite_t *sprite_load(const char *fn)
{
    int sz;
    void *buf = asset_load(fn, &sz);
    sprite_t *s = sprite_load_buf(buf, sz);
    s->flags |= SPRITE_FLAGS_OWNEDBUFFER;
    return s;
}

void sprite_free(sprite_t *s)
{
    if(s->flags & SPRITE_FLAGS_OWNEDBUFFER) {
        #ifndef NDEBUG
        //To help debugging, zero the sprite structure as well
        memset(s, 0, sizeof(sprite_t));
        #endif
        free(s);
    }
    
    if (last_spritemap == s)
        last_spritemap = NULL;
}

surface_t sprite_get_pixels(sprite_t *sprite) {
    return surface_make_linear(sprite->data, sprite_get_format(sprite),
        sprite->width, sprite->height);
}

surface_t sprite_get_lod_pixels(sprite_t *sprite, int num_level) {
    //EMAC:assert(num_level >= 0 && num_level < 8);

    // First LOD = image. Return the image pixels
    if (num_level == 0)
        return sprite_get_pixels(sprite);

    // Get access to the extended sprite structure
    sprite_ext_t *sx = __sprite_ext(sprite);
    if (!sx)
        return (surface_t){0};

    // Get access to the lod structure
    struct sprite_lod_s *lod = &sx->lods[num_level-1];
    if (lod->width == 0)
        return (surface_t){0};

    // Return the surface that refers to this LOD
    pixel_mode_t fmt = lod->fmt_file_pos >> 24;
    void *pixels = (void*)sprite + (lod->fmt_file_pos & 0x00FFFFFF);
    return surface_make_linear(pixels, fmt, lod->width, lod->height);
}

uint16_t* sprite_get_palette(sprite_t *sprite) {
    sprite_ext_t *sx = __sprite_ext(sprite);
    if(!sx || !sx->pal_file_pos)
        return NULL;
    return (void*)sprite + sx->pal_file_pos;
}

surface_t sprite_get_tile(sprite_t *sprite, int h, int v) {
    static int tile_width = 0, tile_height = 0;

    // Compute tile width and height. Unfortunately, the sprite structure
    // store the number of tile rather than the size of a tile, so we are
    // forced to do divisions. Cache the result, as it is common to call
    // this function multiple times anyway.
    if (last_spritemap != sprite) {
        last_spritemap = sprite;
        tile_width = sprite->width / sprite->hslices;
        tile_height = sprite->height / sprite->vslices;
    }

    surface_t surf = sprite_get_pixels(sprite);
    return surface_make_sub(&surf, 
        h*tile_width, v*tile_height,
        tile_width, tile_height);
}

int sprite_get_lod_count(sprite_t *sprite) {
    sprite_ext_t *sx = __sprite_ext(sprite);
    if (!sx)
        return 1;

    int count = 1; // start from main texture
    for (int i=0; i<7; i++)
        if (sx->lods[i].width)
            count++;
    return count;
}

bool sprite_fits_tmem(sprite_t *sprite)
{
    sprite_ext_t *sx = __sprite_ext(sprite);
    if (!sx)
        // FIXME: we don't have the information readily available for old sprites
        return false;
    
    return (sx->flags & SPRITE_FLAG_FITS_TMEM) != 0;
}

extern inline pixel_mode_t sprite_get_format(sprite_t *sprite);
