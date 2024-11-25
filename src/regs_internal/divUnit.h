/**
 * @file divUnit.h
 * @ingroup hardware
 * @brief WebTV's digital video input unit interface
 */

#include <stdint.h>

#define DIV_DMA_CNTL      0xa4008004

#define DIV_AUD_CNTL      0xa4008040

#define DIV_SYNC_CNTL     0xa4008000
#define DIV_SYNC_PHASE    0xa4008060

#define DIV_NEXT_VBI_TB   0xa4008008
#define DIV_NEXT_VBI_LR   0xa400800c
#define DIV_NEXT_VBI_ADDR 0xa4008010
#define DIV_NEXT_TB       0xa4008014
#define DIV_NEXT_LR       0xa4008018
#define DIV_NEXT_CFG      0xa400801c
#define DIV_NEXT_ADDR     0xa4008020

#define DIV_NEXT_AUD_ADDR 0xa4008044
#define DIV_NEXT_AUD_LEN  0xa4008048


#define DIV_CURR_VBI_TB   0xa4008024
#define DIV_CURR_VBI_LR   0xa4008028
#define DIV_CURR_VBI_ADDR 0xa400802c
#define DIV_CURR_TB       0xa4008030
#define DIV_CURR_LR       0xa4008034
#define DIV_CURR_CFG      0xa4008038
#define DIV_CURR_ADDR     0xa400803c

#define DIV_CURR_AUD_ADDR 0xa400804c
#define DIV_CURR_AUD_LEN  0xa4008050

#define DIV_GPIO_EN       0xa4008064
#define DIV_GPIO_OUTVAL   0xa4008068
#define DIV_GPIO_INVAL    0xa400806c
