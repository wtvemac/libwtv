/**
 * @file potUnit.h
 * @ingroup hardware
 * @brief WebTV's pixel processing units
 */

#include <stdint.h>

// Video DMA engine (vidUnit)

#define VID_DMA_CNTL       0xa4003014

#define VID_VDATA          0xa4003040

#define VID_CSTART         0xa4003000
#define VID_CSIZE          0xa4003004
#define VID_CCNT           0xa4003008

#define VID_NSTART         0xa400300c
#define VID_NSIZE          0xa4003010

#define VID_INT_STAT_SET   0xa4003038
#define VID_INT_STAT_CLEAR 0xa4003138

#define VID_INT_EN_SET     0xa400303c
#define VID_INT_EN_CLEAR   0xa400303c

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define VID_VSYNC_RESET     0x000000c0
#define VID_DIAG_SELECT     0x00000020
#define VID_DVE_PIXAVG      0x00000010
#define VID_INTERLACE_EN    0x00000008
#define VID_DMA_EN          0x00000004
#define VID_NV              0x00000002
#define VID_NVF             0x00000001

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

// Pixel output timing
//	included in vidUnit on SPOT boxes.
//	separated out into potUnit on SOLO boxes.

#define POT_CNTL           0xa4009094 // 0x000009c9

#define POT_VSTART         0xa4009080 // 0x00000023 (35)
#define POT_VSIZE          0xa4009084 // 0x000001e0 (480)
#define POT_HSTART         0xa400908c // 0x00000077 (119)
#define POT_HSIZE          0xa4009090 // 0x00000280 (640)

#define POT_BLNKCOL        0xa4009088 // 0x002fcffd

#define POT_CLINE          0xa40090ac

#define POT_HINTLINE       0xa4009098 // 0x00000203

#define POT_INT_EN_SET     0xa400909c
#define POT_INT_EN_CLEAR   0xa40090a4

#define POT_INT_STAT_SET   0xa40090a0
#define POT_INT_STAT_CLEAR 0xa40090a8

// SPOT boxes (this was moved to potUnit on SOLO boxes)

#define VID_FCNTL          0xa4003018

#define VID_VSTART         0xa4003028
#define VID_VSIZE          0xa400302c
#define VID_HSTART         0xa4003020
#define VID_HSIZE          0xa4003024

#define VID_BLNKCOL        0xa400301c

#define VID_CLINE          0xa4003034

#define VID_HINTLINE       0xa4003030


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define VID_NSTART_ALIGN   0x0000001f
#define VID_VSTART_ALIGN   0x0000000f
#define VID_HSTART_ALIGN   0x0000001f
#define BYTES_PER_PIXEL    0x00000002

#define POT_INT_EVSYNC     0x00000020
#define POT_INT_OVSYNC     0x00000010
#define POT_INT_HSYNC      0x00000008
#define POT_INT_SHIFT      0x00000004

#define VID_INT_FIDO       0x00000040 // Only available on SPOT boxes
#define VID_INT_EVSYNC     0x00000020 // Only available on SPOT boxes
#define VID_INT_OVSYNC     0x00000010 // Only available on SPOT boxes
#define VID_INT_HSYNC      0x00000008 // Only available on SPOT boxes
#define VID_INT_DMA        0x00000004

#define POT_USE_GFX444     0x00000800
#define POT_DVE_CSS        0x00000400
#define POT_DVE_HALF_SHIFT 0x00000200
#define POT_HFIELD_LINE    0x00000100
#define POT_VIDS_OUT_EN    0x00000080
#define POT_VID_OUT_EN     0x00000040
#define POT_HALF_SHIFT     0x00000020
#define POT_INVERT_CRCB    0x00000010
#define POT_USE_USEGFX     0x00000008
#define POT_SOFT_RESET     0x00000004
#define POT_PROGRESSIVE    0x00000002
#define POT_OUT_EN         0x00000001

#define VID_SWAP_CRCB      0x00000080
#define VID_INVERT_CRCB    0x00000040
#define VID_USE_FIDO       0x00000020
#define VID_GAMMA_CORRECT  0x00000010
#define VID_BLANKCOL_EN    0x00000008
#define VID_INTERLACE      0x00000004
#define VID_PAL            0x00000002
#define VID_OUT_EN         0x00000001

