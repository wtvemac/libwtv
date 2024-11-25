/**
 * @file modUnit.h
 * @ingroup hardware
 * @brief WebTV's software modem unit and hardware modem interface
 */

#include <stdint.h>

#define MOD_FMT_CNTL     0xa400b040

// Softmodem audio output

#define MOD_OUT_DMA_CNTL 0xa400b01c

#define MOD_OUT_CSTART   0xa400b000
#define MOD_OUT_CSIZE    0xa400b004
#define MOD_OUT_CCONFIG  0xa400b008
#define MOD_OUT_CCNT     0xa400b00c

#define MOD_OUT_NSTART   0xa400b010
#define MOD_OUT_NSIZE    0xa400b014
#define MOD_OUT_NCONFIG  0xa400b018

// Softmodem audio input

#define MOD_IN_DMA_CNTL  0xa400b03c

#define MOD_IN_CSTART    0xa400b020
#define MOD_IN_CSIZE     0xa400b024
#define MOD_IN_CCNT      0xa400b02c

#define MOD_IN_NSTART    0xa400b030
#define MOD_IN_NSIZE     0xa400b034

// Softmodem audio GPIO

#define MOD_GPIO_CNTL    0xa400b044

// Softmodem audio DMA Control (in and out)

#define MOD_IO_DMA_CNTL  0xa400b05c

// Hardware modem on SPOT boxes (within devUnit; overlaps IR out on SOLO boxes)

#define DEV_MOD0         0xa4004040
#define DEV_MOD1         0xa4004044
#define DEV_MOD2         0xa4004048
#define DEV_MOD3         0xa400404c
#define DEV_MOD4         0xa4004050
#define DEV_MOD5         0xa4004054
#define DEV_MOD6         0xa4004058
#define DEV_MOD7         0xa400405c

// Hardware modem on SOLO boxes

#define MOD_HWMOD0       0xbe000000
#define MOD_HWMOD1       0xbe000004
#define MOD_HWMOD2       0xbe000008
#define MOD_HWMOD3       0xbe00000c
#define MOD_HWMOD4       0xbe000010
#define MOD_HWMOD5       0xbe000014
#define MOD_HWMOD6       0xbe000018
#define MOD_HWMOD7       0xbe00001c
