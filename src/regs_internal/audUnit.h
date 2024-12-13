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

#define AUD_DINRIGHTJUST        0x00000040
#define AUD_DOUTPOSBITCLK       0x00000020
#define AUD_DINNEGBITCLK        0x00000010
#define AUD_INVERTLRCLK         0x00000008
#define AUD_INVERTBITCLK        0x00000004
#define AUD_CODEC_AK4520        0x00000000
#define AUD_CODEC_AK4532        0x00000001
#define AUD_CODEC_TITLC320AD50C 0x00000002

#define AUD_DMA_EN              0x00000004
#define AUD_NV                  0x00000002
#define AUD_NVF                 0x00000001

#define AUD_16BIT               0x00000000
#define AUD_8BIT                0x00000002
#define AUD_STEREO              0x00000000
#define AUD_MONO                0x00000001

#define GPIO_AK5432_ENABLE       0x00000002
#define GPIO_AK5432_CS           0x00000400
#define GPIO_AK5432_CCLK         0x00001000
#define GPIO_AK5432_CDATA        0x00002000

#define AK5432_MASTER_VOL_LCH    0x00
#define AK5432_MASTER_VOL_RCH    0x01
#define AK5432_VOICE_VOL_LCH     0x02
#define AK5432_VOICE_VOL_RCH     0x03
#define AK5432_LINE_VOL_LCH      0x08
#define AK5432_LINE_VOL_RCH      0x09
#define AK5432_AUX_VOL_LCH       0x0a
#define AK5432_AUX_VOL_RCH       0x0b
#define AK5432_OUT_MIXER_SW_1    0x10
#define AK5432_OUT_MIXER_SW_2    0x11
#define AK5432_LCH_IN_MIXER_SW_1 0x12
#define AK5432_RCH_IN_MIXER_SW_1 0x13
#define AK5432_LCH_IN_MIXER_SW_2 0x14
#define AK5432_RCH_IN_MIXER_SW_2 0x15
#define AK5432_POWER             0x16
#define AK5432_MIC_AMP_GAIN      0x19

#define AK5432_D_NONE            0x00
#define AK5432_D7_MUTE           0x80
#define AK5432_D4_ATT4           0x10
#define AK5432_D3_ATT3           0x08
#define AK5432_D2_ATT2           0x04
#define AK5432_D1_ATT1           0x02
#define AK5432_D0_ATT0           0x01
#define AK5432_D5_AUXL           0x20
#define AK5432_D4_AUXR           0x10
#define AK5432_D3_VOICEL         0x08
#define AK5432_D2_VOICER         0x04
#define AK5432_D4_LINER          0x10
#define AK5432_D3_LINER          0x08
#define AK5432_D4_AUXL           0x10
#define AK5432_D3_AUXR           0x08
#define AK5432_D2_VOICEL         0x04
#define AK5432_D2_VOICER         0x04
#define AK5432_D1_POWER_DOWN     0x02
#define AK5432_D0_RESET          0x01
#define AK5432_D0_MGAIN_ENABLED  0x01


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