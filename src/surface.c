/**
 * @file surface.c
 * @brief Surface buffers used to draw images
 * @ingroup graphics
 */

#include "surface.h"
#include "wtvsys.h"
//EMAC:#include "debug.h"
//EMAC:#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include "libc.h"

const char* tex_format_name(pixel_mode_t fmt)
{
    switch (fmt) {
    case FMT_NONE:   return "FMT_NONE";
    case FMT_YUV16:  return "FMT_YUV16";
    case FMT_YUV32:  return "FMT_YUV32";
    default:         return "FMT_???";
    }
}

surface_t surface_alloc(pixel_mode_t format, uint16_t width, uint16_t height)
{
    // A common mistake is to call surface_alloc with the wrong argument order.
    // Try to catch it by checking that the format is not valid.
    // Do not limit ourselves to pixel_mode_t enum values, as people might want
    // to test weird RDP formats (e.g. RGBA8) to find out what happens.
    //EMAC:assertf((format & ~SURFACE_FLAGS_TEXFORMAT) == 0,
    //    "invalid surface format: 0x%x (%d)", format, format);
    return (surface_t){ 
        .flags = format | SURFACE_FLAGS_OWNEDBUFFER,
        .width = width,
        .height = height,
        .stride = TEX_FORMAT_PIX2BYTES(format, width),
        .length = TEX_FORMAT_PIX2BYTES(format, width) * height,
        .buffer = malloc_uncached_aligned(64, TEX_FORMAT_PIX2BYTES(format, width) * height),
    };
}

void surface_free(surface_t *surface)
{
    if (surface_has_owned_buffer(surface)) {
        free_uncached(surface->buffer);
    }
    memset(surface, 0, sizeof(surface_t));
}

surface_t surface_make_sub(surface_t *parent, uint16_t x0, uint16_t y0, uint16_t width, uint16_t height)
{
    //EMAC:assert(x0 + width <= parent->width);
    //EMAC:assert(y0 + height <= parent->height);

    pixel_mode_t fmt = surface_get_format(parent);
    //EMAC:assertf(TEX_FORMAT_BITDEPTH(fmt) != 4 || (x0 & 1) == 0,
    //    "cannot create a subsurface with an odd X offset (%" PRId16 ") in a 4bpp surface", x0);

    surface_t sub;
    sub.buffer = parent->buffer + y0 * parent->stride + TEX_FORMAT_PIX2BYTES(fmt, x0);
    sub.width = width;
    sub.height = height;
    sub.stride = parent->stride;
    sub.flags = parent->flags & ~SURFACE_FLAGS_OWNEDBUFFER;
    return sub;
}

//EMAC:
void surface_blank(const surface_t *surface)
{
    uint32_t color = WTVCOLOR_TO_WTV2PIXEL(WSRFC_BLACK_COLOR);

    for(int j = 0; j < surface->length; j += 4)
    {
        *((uint32_t*)(surface->buffer + j)) = color;
    }
}

//EMAC:
void surface_rgba16_to_yuv16(void *buffer, uint32_t length)
{
    return;
    length >>= 2;

    for(uint32_t i = 0; i < length; i++)
    {
        uint32_t rgba_color_2pixel = ((uint32_t*)buffer)[i];

        const uint32_t r = (rgba_color_2pixel >> 11) & 0x1f;
        const uint32_t g = (rgba_color_2pixel >> 6) & 0x1f;
        const uint32_t b = (rgba_color_2pixel >> 1) & 0x1f;

        printf("cool=%08x\x0a\x0d", (r * 0x4d));

        ((uint32_t*)buffer)[i] = 0x11;

        //uint32_t rgba_color_2pixel = *((uint32_t*)(buffer + i));

        /*const uint32_t r = (rgba_color_2pixel >> 11) & 0x1f;
        const uint32_t g = (rgba_color_2pixel >> 6) & 0x1f;
        const uint32_t b = (rgba_color_2pixel >> 1) & 0x1f;*/

        /*uint32_t y = (uint32_t)(((uint32_t)(0x4d * r) + (uint32_t)(0x96 * g) + (uint32_t)(0x1d * b) + (uint32_t)(1 << 7)) >> 8);

        uint32_t u = (UV_OFFSET + ((0x90 * (b - y)) + (1 << 7))) & 0xff;
        if(u > Y_LIGHTEST) u = Y_LIGHTEST;
        if(u < Y_DARKEST) u = Y_DARKEST;

        uint32_t v = (UV_OFFSET + ((0xb7 * (r - y)) + (1 << 7))) & 0xff;
        if(v > Y_LIGHTEST) v = Y_LIGHTEST;
        if(v < Y_DARKEST) v = Y_DARKEST;

        y = (Y_DARKEST + (((y * Y_RANGE) + (1 << 7)) >> 8)) & 0xff;
        if(y > Y_LIGHTEST) y = Y_LIGHTEST;
        if(y < Y_DARKEST) y = Y_DARKEST;

        uint32_t cool = ((y << 0x18) | (u << 0x10) | (y << 0x08) | v);*/

    }

    /*
    pixel = space.read_dword(m_vid_ccnt);

    int32_t y1 = ((pixel >> 0x18) & 0xff) - VID_Y_BLACK;
    int32_t Cb = ((pixel >> 0x10) & 0xff) - VID_UV_OFFSET;
    int32_t y2 = ((pixel >> 0x08) & 0xff) - VID_Y_BLACK;
    int32_t Cr = ((pixel) & 0xff) - VID_UV_OFFSET;

    y1 = (((y1 << 8) + VID_UV_OFFSET) / VID_Y_RANGE);
    y2 = (((y2 << 8) + VID_UV_OFFSET) / VID_Y_RANGE);

    int32_t r = ((0x166 * Cr) + VID_UV_OFFSET) >> 8;
    int32_t b = ((0x1C7 * Cb) + VID_UV_OFFSET) >> 8;
    int32_t g = ((0x32 * b) + (0x83 * r) + VID_UV_OFFSET) >> 8;

    *line++ = (
          std::clamp(y1 + r, 0x00, 0xff) << 0x10
        | std::clamp(y1 - g, 0x00, 0xff) << 0x08
        | std::clamp(y1 + b, 0x00, 0xff)
    );

    *line++ = (
          std::clamp(y2 + r, 0x00, 0xff) << 0x10
        | std::clamp(y2 - g, 0x00, 0xff) << 0x08
        | std::clamp(y2 + b, 0x00, 0xff)
    );
    */


}

/*
int ARGBToYUY2(const uint8_t* src_argb,
               int src_stride_argb,
               uint8_t* dst_yuy2,
               int dst_stride_yuy2,
               int width,
               int height) {
  int y;
  void (*ARGBToUVRow)(const uint8_t* src_argb, int src_stride_argb,
                      uint8_t* dst_u, uint8_t* dst_v, int width) =
      ARGBToUVRow_C;
  void (*ARGBToYRow)(const uint8_t* src_argb, uint8_t* dst_y, int width) =
      ARGBToYRow_C;
  void (*I422ToYUY2Row)(const uint8_t* src_y, const uint8_t* src_u,
                        const uint8_t* src_v, uint8_t* dst_yuy2, int width) =
      I422ToYUY2Row_C;

  if (!src_argb || !dst_yuy2 || width <= 0 || height == 0) {
    return -1;
  }
  // Negative height means invert the image.
  if (height < 0) {
    height = -height;
    dst_yuy2 = dst_yuy2 + (height - 1) * dst_stride_yuy2;
    dst_stride_yuy2 = -dst_stride_yuy2;
  }
  // Coalesce rows.
  if (src_stride_argb == width * 4 && dst_stride_yuy2 == width * 2) {
    width *= height;
    height = 1;
    src_stride_argb = dst_stride_yuy2 = 0;
  }
#if defined(HAS_ARGBTOYROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBToYRow = ARGBToYRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBTOUVROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ARGBToUVRow = ARGBToUVRow_Any_SSSE3;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_SSSE3;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToYRow = ARGBToYRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      ARGBToYRow = ARGBToYRow_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBTOUVROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ARGBToUVRow = ARGBToUVRow_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      ARGBToUVRow = ARGBToUVRow_AVX2;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToYRow = ARGBToYRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_NEON_DOTPROD)
  if (TestCpuFlag(kCpuHasNeonDotProd)) {
    ARGBToYRow = ARGBToYRow_Any_NEON_DotProd;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_NEON_DotProd;
    }
  }
#endif
#if defined(HAS_ARGBTOUVROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ARGBToUVRow = ARGBToUVRow_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      ARGBToUVRow = ARGBToUVRow_NEON;
    }
  }
#endif
#if defined(HAS_ARGBTOUVROW_SVE2)
  if (TestCpuFlag(kCpuHasSVE2)) {
    ARGBToUVRow = ARGBToUVRow_Any_SVE2;
    if (IS_ALIGNED(width, 2)) {
      ARGBToUVRow = ARGBToUVRow_SVE2;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_MSA) && defined(HAS_ARGBTOUVROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ARGBToYRow = ARGBToYRow_Any_MSA;
    ARGBToUVRow = ARGBToUVRow_Any_MSA;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_MSA;
    }
    if (IS_ALIGNED(width, 32)) {
      ARGBToUVRow = ARGBToUVRow_MSA;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    ARGBToYRow = ARGBToYRow_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_LSX;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_LSX) && defined(HAS_ARGBTOUVROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    ARGBToYRow = ARGBToYRow_Any_LSX;
    ARGBToUVRow = ARGBToUVRow_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      ARGBToYRow = ARGBToYRow_LSX;
      ARGBToUVRow = ARGBToUVRow_LSX;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_LASX) && defined(HAS_ARGBTOUVROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    ARGBToYRow = ARGBToYRow_Any_LASX;
    ARGBToUVRow = ARGBToUVRow_Any_LASX;
    if (IS_ALIGNED(width, 32)) {
      ARGBToYRow = ARGBToYRow_LASX;
      ARGBToUVRow = ARGBToUVRow_LASX;
    }
  }
#endif
#if defined(HAS_ARGBTOYROW_RVV)
  if (TestCpuFlag(kCpuHasRVV)) {
    ARGBToYRow = ARGBToYRow_RVV;
  }
#endif
#if defined(HAS_I422TOYUY2ROW_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_SSE2;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_SSE2;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_AVX2;
    if (IS_ALIGNED(width, 32)) {
      I422ToYUY2Row = I422ToYUY2Row_AVX2;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_NEON;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_NEON;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_MSA;
    if (IS_ALIGNED(width, 32)) {
      I422ToYUY2Row = I422ToYUY2Row_MSA;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_LSX)
  if (TestCpuFlag(kCpuHasLSX)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_LSX;
    if (IS_ALIGNED(width, 16)) {
      I422ToYUY2Row = I422ToYUY2Row_LSX;
    }
  }
#endif
#if defined(HAS_I422TOYUY2ROW_LASX)
  if (TestCpuFlag(kCpuHasLASX)) {
    I422ToYUY2Row = I422ToYUY2Row_Any_LASX;
    if (IS_ALIGNED(width, 32)) {
      I422ToYUY2Row = I422ToYUY2Row_LASX;
    }
  }
#endif

  {
    // Allocate a rows of yuv.
    align_buffer_64(row_y, ((width + 63) & ~63) * 2);
    uint8_t* row_u = row_y + ((width + 63) & ~63);
    uint8_t* row_v = row_u + ((width + 63) & ~63) / 2;
    if (!row_y)
      return 1;

    for (y = 0; y < height; ++y) {
      ARGBToUVRow(src_argb, 0, row_u, row_v, width);
      ARGBToYRow(src_argb, row_y, width);
      I422ToYUY2Row(row_y, row_u, row_v, dst_yuy2, width);
      src_argb += src_stride_argb;
      dst_yuy2 += dst_stride_yuy2;
    }

    free_aligned_buffer_64(row_y);
  }
  return 0;
}
*/

//EMAC:
void surface_rgba32_to_yuv16(void *buffer, uint32_t length)
{

}

extern inline surface_t surface_make(void *buffer, pixel_mode_t format, uint16_t width, uint16_t height, uint16_t stride);
extern inline pixel_mode_t surface_get_format(const surface_t *surface);
extern inline surface_t surface_make_linear(void *buffer, pixel_mode_t format, uint16_t width, uint16_t height);
extern inline bool surface_has_owned_buffer(const surface_t *surface);
