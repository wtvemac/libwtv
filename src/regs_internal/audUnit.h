/**
 * @file audUnit.h
 * @ingroup hardware
 * @brief WebTV's audio unit interface
 */
 
#include <stdint.h>

#define AUD_FMT_CNTL     0xa4002040

// Audio output

#define AUD_OUT_DMA_CNTL 0xa400201c

#define AUD_OUT_CSTART   0xa4002000
#define AUD_OUT_CSIZE    0xa4002004
#define AUD_OUT_CCONFIG  0xa4002008
#define AUD_OUT_CCNT     0xa400200c

#define AUD_OUT_NSTART   0xa4002010
#define AUD_OUT_NSIZE    0xa4002014
#define AUD_OUT_NCONFIG  0xa4002018

// Audio input

#define AUD_IN_DMA_CNTL  0xa400203c

#define AUD_IN_CSTART    0xa4002020
#define AUD_IN_CSIZE     0xa4002024
#define AUD_IN_CCNT      0xa400202c

#define AUD_IN_NSTART    0xa4002030
#define AUD_IN_NSIZE     0xa4002034

// GPIO

#define AUD_GPIO_CNTL    0xa4002044

// DMA Control (in and out)

#define AUD_IO_DMA_CNTL  0xa400205c

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define AUD_DMA_EN 0x00000004
#define AUD_NV     0x00000002
#define AUD_NVF    0x00000001

#define AUD_8BIT 0x00000002
#define AUD_MONO 0x00000001

/*
a4002000: 000256e0 00000800 00000000 00000800 ..V.............
a4002010: 000256e0 00000800 00000000 00000000 ..V.............
a4002020: 00000000 00000000 0000f96c 0000f96c ...........l...l
a4002030: 00000000 00000000 00000000 00000000 ................
a4002040: 00000001 00000000 00000001 00000800 ................
a4002050: 000256e1 00000800 00000001 00000000 ..V.............
a4002060: 00000001 00000000 0000f96d 0000f96c ...........m...l
a4002070: 00000001 00000000 00000001 00000000 ................
a4002080: 000256e0 00000800 00000000 00000800 ..V.............
a4002090: 000256e0 00000800 00000000 00000000 ..V.............
a40020a0: 00000000 00000000 0000f96c 0000f96c ...........l...l
a40020b0: 00000000 00000000 00000000 00000000 ................
a40020c0: 00000001 00000000 00000001 00000800 ................
a40020d0: 000256e1 00000800 00000001 00000000 ..V.............
a40020e0: 00000001 00000000 0000f96d 0000f96c ...........m...l
a40020f0: 00000001 00000000 00000001 00000000 ................
*/