/**
 * @file surface.h
 * @brief Surface buffers used to draw images
 * @ingroup graphics
 * 
 * This module implements a structure #surface_t which holds the basic
 * information for a buffer of memory to be used for graphics rendering.
 * 
 * A surface is described by the following properties:
 * 
 *  * Size (width, height)
 *  * Pixel format
 *  * Stride (distance in bytes between rows)
 * 
 * #surface_t simply represents an aggregation of these properties.
 * 
 * To allocate a new surface, use #surface_alloc. Then later, you can release
 * the memory using #surface_free.
 * 
 * @code{.c}
 *      // Allocate a 64x64 buffer in RGBA 16-bit format
 *      surface_t buf = surface_alloc(FMT_RGBA16, 64, 64);
 *  
 *      // Draw some text on it (with the CPU)
 *      graphics_draw_text(&buf, 0, 0, "ABC");
 * @endcode
 *
 * Sometimes, you might have an existing raw pointer to a buffer and need to pass it
 * to an API that accepts a #surface_t. For those cases, you can use
 * #surface_make to create a #surface_t instance, that you can throw away
 * after you called the function; #surface_free does nothing on these surfaces.
 * 
 * In some cases, you might want to interact with a rectangular portion of
 * an existing surface (for instance, you want to draw with RDP only in the
 * top portion of the screen for some reason). To do so, you can use
 * #surface_make_sub to create a #surface_t instance that is referring only to
 * a portion of the original surface:
 * 
 * @code{.c}
 *      surface_t *fb = display_get();  // wait for a framebuffer to be ready
 *      
 *      // Attach the RDP to the top 40 rows of the framebuffer
 *      surface_t fbtop = surface_make_sub(fb, 0, 0, 320, 40);
 *      rdpq_attach(&fbtop);
 * @endcode
 * 
 * Surfaces created by #surface_make_sub don't need to be freed as they
 * are just references to the parent surface; #surface_free does nothing
 * on them.
 */

#ifndef __LIBDRAGON_SURFACE_H
#define __LIBDRAGON_SURFACE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WSRFC_Y_DARKEST      0x10
#define WSRFC_Y_LIGHTEST     0xeb
#define WSRFC_Y_RANGE        (WSRFC_Y_LIGHTEST - WSRFC_Y_DARKEST)
#define WSRFC_UV_OFFSET      0x80

#define LUMINANCE_CEIL(Y) \
	((short)Y > (short)WSRFC_Y_LIGHTEST) ? (WSRFC_Y_LIGHTEST) : (Y)

#define LUMINANCE_FLOOR(Y) \
	((short)Y < (short)WSRFC_Y_DARKEST) ? (WSRFC_Y_DARKEST) : (Y)

#define LUMINANCE_CLAMP(Y) \
	LUMINANCE_CEIL(LUMINANCE_FLOOR(Y))

#define YCRCB_TO_WTVPIXEL(Y, Cr, Cb) \
	(((Y) << 0x18) | ((Cb) << 0x10))

#define Y1Y2CRCB_TO_WTV2PIXEL(Y1, Y2, Cr, Cb) \
	(((Y1) << 0x18) | ((Cb) << 0x10) | ((Y2) << 0x08) | (Cr))

#define RGB_TO_PREY(R, G, B) \
	((((short)0x4d * (short)R) + ((short)0x96 * (short)G) + ((short)0x1d * (short)B) + (short)WSRFC_UV_OFFSET) >> 8)

#define RGB_TO_Y(R, G, B) \
	LUMINANCE_CLAMP((((RGB_TO_PREY(R, G, B) * WSRFC_Y_RANGE) + WSRFC_UV_OFFSET) >> 8) + WSRFC_Y_DARKEST)

#define RGB_TO_CR(R, G, B) \
	LUMINANCE_CLAMP(((((short)0xb7 * (short)(R - RGB_TO_PREY(R, G, B))) + (short)WSRFC_UV_OFFSET) >> 8) + WSRFC_UV_OFFSET)

#define RGB_TO_CB(R, G, B) \
	LUMINANCE_CLAMP(((((short)0x90 * (short)(B - RGB_TO_PREY(R, G, B))) + (short)WSRFC_UV_OFFSET) >> 8) + WSRFC_UV_OFFSET)

#define	RGB_TO_WTVCOLOR(R, G, B) \
	(uint32_t)((RGB_TO_Y(R, G, B) << 0x10) | (RGB_TO_CR(R, G, B) << 0x08) | (RGB_TO_CB(R, G, B)))

#define WSRFC_WHITE_COLOR    RGB_TO_WTVCOLOR(0xff, 0xff, 0xff)
#define WSRFC_BLACK_COLOR    RGB_TO_WTVCOLOR(0x00, 0x00, 0x00)
#define WSRFC_WTVBGR_COLOR   RGB_TO_WTVCOLOR(0x22, 0x22, 0x22)
#define WSRFC_WTVTXT_COLOR   RGB_TO_WTVCOLOR(0x44, 0xcc, 0x55)
#define WSRFC_WTVHDR_COLOR   RGB_TO_WTVCOLOR(0xe7, 0xce, 0x4a)
#define WSRFC_WTVLNK_COLOR   RGB_TO_WTVCOLOR(0x18, 0x9c, 0xd6)

/// @cond
// Macro to create a texture format, combining the RDP native "fmt/size" tuple.
// This macro is used to genearte the #pixel_mode_t enums creating identifiers
// which are easy to convert back into RDP native fields.
#define PIXEL_MODE(color_quantization, color_format, pixel_bitdepth) ((color_quantization << 6) | (color_format << 3) | (pixel_bitdepth))
/// @endcond

/** @brief Extract the depth (number of bits per pixel) from a #pixel_mode_t. (eg: `FMT_RGBA16` => 16) 
 * 
 * Note that there are texture format that are 4bpp, so don't divide this by 8 to get the number of bytes
 * per pixels, but rather use #TEX_FORMAT_BYTES2PIX and #TEX_FORMAT_PIX2BYTES. */
#define TEX_FORMAT_BITDEPTH(mode)                                    (4 << ((mode) & 0x7))
/** @brief Convert the specified number of pixels to bytes. 
 * 
 * @note This macro rounds up the value. For 4bpp surfaces, this means that it returns
 *       the safe number of bytes that can hold the specified number of pixels.
 *       For instance, `TEX_FORMAT_PIX2BYTES(FMT_CI4, 3)` returns 2, as you need 2 bytes
 *       to store 3 pixels in 4bpp format (even though the last byte is only half used).
 */
#define TEX_FORMAT_PIX2BYTES(mode, pixels)                           ((((pixels) << (((mode) & 7) + 2)) + 7) >> 3)
/** @brief Convert the specified number of bytes to pixels.
  * 
  * @note This macro rounds down the value. For instance, for a 32-bpp surface,
  *       calling `TEX_FORMAT_BYTES2PIX(FMT_RGBA32, 5)` returns 1, because you can safely
  *       store at maximum 1 32bpp pixel in 5 bytes.
  */
#define TEX_FORMAT_BYTES2PIX(mode, bytes)                            (((bytes) << 1) >> ((mode) & 7))

/** @brief Valid pixel bit depths */
typedef enum
{
    DEPTH_NONE   = 0, ///< Placeholder for no pixel depth defined
    DEPTH_16_BPP = 2, ///< 16 bits per pixel
    DEPTH_32_BPP = 3  ///< 32 bits per pixel
} bitdepth_t;

/** @brief Valid color formats */
typedef enum
{
    /** @brief red (R), green (G), blue (B) and alpha/transparency (A) 
     *
     * 16-bit pixel depth color range:
     *   R  = 0 to 31
     *   G  = 0 to 31
     *   B  = 0 to 31
     *   A  = 0 to  1
     *
     * 32-bit pixel depth color range:
     *   R  = 0 to 255
     *   G  = 0 to 255
     *   B  = 0 to 255
     *   A  = 0 to 255
    */
    COLOR_RGBA = 0,
    /** @brief luminance (Y) and two chrominance (analog as UV or CbCr digitally) 
     *
     * 16-bit pixel depth color range:
     *   Y  =    0 to 219 
     *   Cr = -112 to 112
     *   Cr = -112 to 112
     *   A  = not available
     * In this format the Cr and Cb are shared across two pixels with different Y values.
     *
     * 32-bit pixel depth color range:
     *   Y  =    0 to 219 
     *   Cr = -112 to 112
     *   Cr = -112 to 112
     *   A  =    0 to 255
    */
    COLOR_YUV  = 1,
    /** @brief luminance (Y) and two chrominance (analog as UV or CbCr digitally) with a range offset
     *
     * 16-bit pixel depth color range:
     *   Y  =   16 to 235 
     *   Cr =   16 to 240
     *   Cr =   16 to 240
     *   A  = not available
     * In this format the Cr and Cb are shared across two pixels with different Y values.
     *
     * 32-bit pixel depth color range:
     *   Y  =   16 to 235 
     *   Cr =   16 to 240
     *   Cr =   16 to 240
     *   A  =    0 to 255
     */
    COLOR_YUVO = 2
} color_format;

/** @brief Valid color quantizations */
typedef enum
{
    COLORQ_SCALAR  = 0, ///< Scalar with direct representation of colors.
    COLORQ_VECTOR8 = 1, ///< 8-bit vector quantiziation with CLUT
    COLORQ_VECTOR4 = 2  ///< 4-bit vector quantiziation with CLUT
} color_quantization_t;


/**
 * @brief Pixel format enum
 * 
 * This enum defines the pixel formats that can be used for #surface_t buffers.
 * The list corresponds to the pixel formats that the RDP can use as textures.
 * 
 * @note Some of these formats can be used by RDP as framebuffer (specifically,
 * #FMT_RGBA16, #FMT_RGBA32 and #FMT_CI8). 
 * @warning the CPU-based graphics library
 * graphics.h only accepts surfaces in either #FMT_RGBA16 or #FMT_RGBA32 as 
 * target buffers, and does not assert.
 */
typedef enum
{
    FMT_NONE           = 0,                    ///< Placeholder for no format defined

    FMT_RGBA16        = PIXEL_MODE(COLORQ_SCALAR, COLOR_RGBA, DEPTH_16_BPP), ///< Format RGBA 5551 (16-bit)
    FMT_RGBA32        = PIXEL_MODE(COLORQ_SCALAR, COLOR_RGBA, DEPTH_32_BPP), ///< Format RGBA 8888 (32-bit)

    FMT_YUV16         = PIXEL_MODE(COLORQ_SCALAR, COLOR_YUV,  DEPTH_16_BPP), ///< Format YUV2 4:2:2
    FMT_YUV16_OFFSET  = PIXEL_MODE(COLORQ_SCALAR, COLOR_YUVO, DEPTH_16_BPP), ///< Format YUV2 4:2:2 with range offset
    FMT_YUV32         = PIXEL_MODE(COLORQ_SCALAR, COLOR_YUV,  DEPTH_32_BPP), ///< Format YUV2 4:4:4

    FMT_YUV16_VQ8     = PIXEL_MODE(COLORQ_VECTOR8, COLOR_YUV, DEPTH_16_BPP), ///< Format 8-bit vector with YUV2 4:2:2 code book
    FMT_YUV32_VQ8     = PIXEL_MODE(COLORQ_VECTOR8, COLOR_YUV, DEPTH_32_BPP), ///< Format 8-bit vector with YUV2 4:4:4 code book
    FMT_YUV32_VQ4     = PIXEL_MODE(COLORQ_VECTOR4, COLOR_YUV, DEPTH_32_BPP), ///< Format 4-bit vector with YUV2 4:4:4 code book
} pixel_mode_t;

/** @brief Return the name of the texture format as a string (for debugging purposes) */
const char* tex_format_name(pixel_mode_t fmt);

#define SURFACE_FLAGS_TEXFORMAT    0x001F   ///< Pixel format of the surface
#define SURFACE_FLAGS_OWNEDBUFFER  0x0020   ///< Set if the buffer must be freed
#define SURFACE_FLAGS_TEXINDEX     0x0F00   ///< Placeholder for rdpq lookup table

/**
 * @brief A surface buffer for graphics
 * 
 * This structure holds the basic information about a buffer used to hold graphics.
 * It is commonly used by graphics routines in libdragon as either a source (eg: texture)
 * or a target (eg: framebuffer). It can be used for both CPU-based drawing
 * (such as graphics.h) or RDP-basic drawing (such as rdp.h and rdpq.h).
 * 
 * Use #surface_alloc / #surface_free to allocate / free a surface. If you already have
 * a memory pointer to a graphics buffer and you just need to wrap it in a #surface_t,
 * use #surface_make.
 */
typedef struct surface_s
{
    uint16_t flags;                                  ///< Flags (including pixel format)
    uint16_t width;                                  ///< Width in pixels
    uint16_t height;                                 ///< Height in pixels
    uint16_t stride;                                 ///< Stride in bytes (length of a row)
    uint32_t length;                                 ///< Buffer length
    void *buffer;                                    ///< Buffer pointer
    void (*callback)(void *buffer, uint32_t length); ///< Callback to prepare the buffer for hardware.
} surface_t;

/**
 * @brief Initialize a surface_t structure with the provided buffer.
 * 
 * This functions initializes a surface_t structure with the provided buffer and information.
 * It is just a helper to fill the structure fields.
 * 
 * It is not necessary to call #surface_free on surfaces created by #surface_make as there
 * is nothing to free: the provided buffer will not be owned by the structure, so it is up
 * to the caller to handle its lifetime.
 * 
 * If you plan to use this format as RDP framebuffer, make sure that the provided buffer
 * respects the required alignment of 64 bytes, otherwise #rdpq_attach will fail.
 * 
 * @param[in] buffer    Pointer to the memory buffer
 * @param[in] format    Pixel format
 * @param[in] width     Width in pixels
 * @param[in] height    Height in pixels
 * @param[in] stride    Stride in bytes (length of a row)
 * @return              The initialized surface
 * 
 * @see #surface_make_linear
 */
inline surface_t surface_make(void *buffer, pixel_mode_t format, uint16_t width, uint16_t height, uint16_t stride) {
    return (surface_t){
        .flags = format,
        .width = width,
        .height = height,
        .stride = stride,
        .buffer = buffer,
    };
}

/**
 * @brief Initialize a surface_t structure with the provided linear buffer.
 *
 * This function is similar to #surface_make, but it works for images that
 * are linearly mapped with no per-line padding or extraneous data.
 * 
 * Compared to #surface_make, it does not accept a stride parameter, and
 * calculate the stride from the width and the pixel format.
 * 
 * @param[in] buffer    Pointer to the memory buffer
 * @param[in] format    Pixel format
 * @param[in] width     Width in pixels
 * @param[in] height    Height in pixels
 * @return              The initialized surface
 * 
 * @see #surface_make
 */
inline surface_t surface_make_linear(void *buffer, pixel_mode_t format, uint16_t width, uint16_t height) {
    return surface_make(buffer, format, width, height, TEX_FORMAT_PIX2BYTES(format, width));
}

/**
 * @brief Allocate a new surface in memory
 * 
 * This function allocates a new surface with the specified pixel format,
 * width and height. The surface must be freed via #surface_free when it is
 * not needed anymore.
 * 
 * A surface allocated via #surface_alloc can be used as a RDP frame buffer
 * (passed to #rdpq_attach) because it is guaranteed to have the required
 * alignment of 64 bytes, provided it is using one of the formats supported by
 * RDP as a framebuffer target (`FMT_RGBA32`, `FMT_RGBA16` or `FMT_I8`).
 *
 * @param[in]  format   Pixel format of the surface
 * @param[in]  width    Width in pixels
 * @param[in]  height   Height in pixels
 * @return              The initialized surface
 */
surface_t surface_alloc(pixel_mode_t format, uint16_t width, uint16_t height);

/**
 * @brief Initialize a surface_t structure, pointing to a rectangular portion of another
 *        surface.
 * 
 * The surface returned by this function will point to a portion of the buffer of
 * the parent surface, and will have of course the same pixel format.
 * 
 * @param[in]  parent   Parent surface that will be pointed to
 * @param[in]  x0       X coordinate of the top-left corner within the parent surface
 * @param[in]  y0       Y coordinate of the top-left corner within the parent surface
 * @param[in]  width    Width of the surface that will be returned
 * @param[in]  height   Height of the surface that will be returned
 * @return              The initialized surface
 */
surface_t surface_make_sub(surface_t *parent, 
    uint16_t x0, uint16_t y0, uint16_t width, uint16_t height);


//EMAC:
void surface_rgba16_to_yuv16(void *buffer, uint32_t length);
//EMAC:
void surface_rgba32_to_yuv16(void *buffer, uint32_t length);

/**
 * @brief Free the buffer allocated in a surface.
 * 
 * This function should be called after a surface allocated via #surface_alloc is not
 * needed anymore. 
 * 
 * Calling this function on surfaces allocated via #surface_make or #surface_make_sub
 * (that is, surfaces initialized with an existing buffer pointer) has no effect but
 * clearing the contents of the surface structure.
 * 
 * @param[in]  surface   The surface to free
 */
void surface_free(surface_t *surface);

/**
 * @brief Returns the pixel format of a surface
 * 
 * @param[in] surface   Surface
 * @return              The pixel format of the provided surface
 */
inline pixel_mode_t surface_get_format(const surface_t *surface)
{
    return (pixel_mode_t)(surface->flags & SURFACE_FLAGS_TEXFORMAT);
}

/**
 * @brief Checks whether this surface owns the buffer that it contains.
 * 
 * @param[in] surface   Surface
 * @return              True if this surface owns the buffer; false otherwise
 */
inline bool surface_has_owned_buffer(const surface_t *surface)
{
    return surface->buffer != NULL && surface->flags & SURFACE_FLAGS_OWNEDBUFFER;
}

/**
 * @brief Create a placeholder surface, that can be used during rdpq block recording.
 * 
 * When recording a rspq block (via #rspq_block_begin / #rspq_block_end) it might
 * be useful sometimes to issue draw commands that refer to a surface, but
 * allowing the actual surface to change later at any time.
 * 
 * See #rdpq_set_lookup_address for more information.
 * 
 * @note A placeholder surface holds a NULL pointer to the actual bytes. Make sure
 *       not to use it anywhere else but with rdpq.
 * 
 * @param index     Index that will be used to lookup the surface at playback time
 * @param format    Pixel format
 * @param width     Width of the surface in pixels
 * @param height    Height of the surface in pixels
 * @param stride    Stride of the surface in bytes
 * @return surface_t    The initialized placeholder surface
 * 
 * @see #surface_make_placeholder_linear
 * @see #rdpq_set_lookup_address
 */
inline surface_t surface_make_placeholder(int index, pixel_mode_t format, uint16_t width, uint16_t height, uint16_t stride) {
    return (surface_t){
        .flags = (uint16_t)(format | ((index & 0xF) << 8)),
        .width = width,
        .height = height,
        .stride = stride,
        .buffer = NULL,
    };
}

/**
 * @brief Create a linear placeholder surface, that can be used during rdpq block recording.
 * 
 * This function is similar to #surface_make_placeholder, but it creates
 * a surface that is linearly mapped with no per-line padding or extraneous data.
 * (so the stride is automatically deduced from the width).
 * 
 * @param index     Index that will be used to lookup the surface at playback time
 * @param format    Pixel format
 * @param width     Width of the surface in pixels
 * @param height    Height of the surface in pixels
 * @return surface_t    The initialized placeholder surface
 * 
 * @see #surface_make_placeholder
 */
inline surface_t surface_make_placeholder_linear(int index, pixel_mode_t format, uint16_t width, uint16_t height) {
    return surface_make_placeholder(index, format, width, height, TEX_FORMAT_PIX2BYTES(format, width));
}

/**
 * @brief Returns the lookup index of a placeholder surface
 * 
 * If the surface is a placeholder, this function returns the associated lookup
 * index that will be used to retrieve the actual surface at playback time.
 * Otherwise, if it is a normal surface, this function will return 0.
 * 
 * @param surface   Placeholder surface
 * @return int      The lookup index of the placeholder surface, or 0 if it is a normal surface
 */
inline int surface_get_placeholder_index(const surface_t *surface)
{
    return (surface->flags >> 8) & 0xF;
}


#ifdef __cplusplus
}
#endif

#endif
