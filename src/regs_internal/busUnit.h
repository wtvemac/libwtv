/**
 * @file busUnit.h
 * @ingroup hardware
 * @brief WebTV's CPU bus unit interface
 */

#include <stdint.h>

#define BUS_CHIP_ID                0xa4000000

#define BUS_CHIP_CNTL              0xa4000004

#define BUS_WATCHDOG_VALUE         0xa4000030
#define BUS_WATCHDOG_CLEAR         0xa4000118
//#define BUS_WATCHDOG_CLEAR         0xa4001018

#define BUS_TOP_OF_RAM             0xa40000c0

#define BUS_RESET_CAUSE_SET        0xa40000a8
#define BUS_RESET_CAUSE_CLEAR      0xa40000ac
#define BUS_ONLY_POWER_ON_SOLO     0xa40000d0 // = 0x0a from EnablePowerOnSoloUnits()

#define BUS_BOOT_MODE              0xa40000c8
#define BUS_USE_BOOT_MODE          0xa40000cc

#define BUS_ERR_EN_SET             0xa4000014
#define BUS_ERR_EN_CLEAR           0xa4000114
#define BUS_ERR_EN_ADDR            0xa4000018
#define BUS_ERR_STAT_SET           0xa4000054
#define BUS_ERR_STAT_CLEAR         0xa4000110
#define BUS_ERR_STAT_STATUS        0xa4000010

#define BUS_FENCE_CNTL             0xa40000c4

// Used in SPOT boxes, not mentioned in SOLO doc
#define BUS_FENCE1_ADDR            0xa400001c
#define BUS_FENCE1_MASK            0xa4000020
#define BUS_FENCE2_ADDR            0xa4000024
#define BUS_FENCE2_MASK            0xa4000028

#define BUS_FENCE_JAVA1_LOW_ADDR   0xa40000b0
#define BUS_FENCE_JAVA1_HIGH_ADDR  0xa40000b4
#define BUS_FENCE_JAVA2_LOW_ADDR   0xa40000b8
#define BUS_FENCE_JAVA2_HIGH_ADDR  0xa40000bc

#define BUS_LOW_READ_ADDR          0xa4000034
#define BUS_LOW_READ_MASK          0xa4000038

#define BUS_LOW_WRITE_ADDR         0xa400003c
#define BUS_LOW_WRITE_MASK         0xa4000040

#define BUS_TIMER_COUNT            0xa4000048
#define BUS_TIMER_COMPARE          0xa400004c

#define BUS_INT_EN_SET             0xa400000c
#define BUS_INT_EN_CLEAR           0xa400010c
#define BUS_INT_STAT_STATUS        0xa4000008
#define BUS_INT_STAT_CLEAR         0xa4000108
#define BUS_INT_STAT_SET           0xa4000050

#define BUS_INT_GPIO_POL           0xa4000064
#define BUS_INT_GPIO_EN_SET        0xa400005c
#define BUS_INT_GPIO_EN_CLEAR      0xa400015c
#define BUS_INT_GPIO_STAT_SET      0xa4000060
#define BUS_INT_GPIO_STAT_CLEAR    0xa4000158
#define BUS_INT_GPIO_STAT_STATUS   0xa4000058

#define BUS_INT_AUDIO_EN_SET       0xa4000070
#define BUS_INT_AUDIO_EN_CLEAR     0xa4000170
#define BUS_INT_AUDIO_STAT_SET     0xa400006c
#define BUS_INT_AUDIO_STAT_CLEAR   0xa4000168
#define BUS_INT_AUDIO_STAT_STATUS  0xa4000068

#define BUS_INT_DEV_EN_SET         0xa400007c
#define BUS_INT_DEV_EN_CLEAR       0xa400017c
#define BUS_INT_DEV_STAT_SET       0xa4000078
#define BUS_INT_DEV_STAT_CLEAR     0xa4000174
#define BUS_INT_DEV_STAT_STATUS    0xa4000074

#define BUS_INT_VIDEO_EN_SET       0xa4000088
#define BUS_INT_VIDEO_EN_CLEAR     0xa4000188
#define BUS_INT_VIDEO_STAT_SET     0xa4000084
#define BUS_INT_VIDEO_STAT_CLEAR   0xa4000180
#define BUS_INT_VIDEO_STAT_STATUS  0xa4000080

#define BUS_INT_RIO_POL            0xa4000094
#define BUS_INT_RIO_EN_SET         0xa4000098
#define BUS_INT_RIO_EN_CLEAR       0xa4000198
#define BUS_INT_RIO_STAT_SET       0xa4000090
#define BUS_INT_RIO_STAT_CLEAR     0xa400018c
#define BUS_INT_RIO_STAT_STATUS    0xa400008c

#define BUS_INT_TIMER_EN_SET       0xa40000a4
#define BUS_INT_TIMER_EN_CLEAR     0xa40001a4
#define BUS_INT_TIMER_STAT_SET     0xa40000a0
#define BUS_INT_TIMER_STAT_CLEAR   0xa400019c
#define BUS_INT_TIMER_STAT_STATUS  0xa400009c

////

#define CHIP_TYPE                  0xff000000
#define CHIP_TYPE_BSHIFT           __builtin_clz(CHIP_TYPE)
#define CHIP_REV                   0x00f00000
#define CHIP_REV_BSHIFT            __builtin_clz(CHIP_REV)
#define CHIP_FAB                   0x000f0000
#define CHIP_FAB_BSHIFT            __builtin_clz(CHIP_FAB)

#define CHIP_TYPE_SPOT0            0x00000000
#define CHIP_TYPE_SPOT1            0x00000001 // Also FIDO
#define CHIP_TYPE_SOLO1            0x00000002
#define CHIP_TYPE_SOLO2            0x00000003
#define CHIP_TYPE_SOLO2p1          0x00000004
#define CHIP_TYPE_DINKY            0x00000005

#define CHIP_VERSION_UNKNOWN       0x00000000
#define CHIP_VERSION1              0x00000001 // 0.1 SPOT 1
#define CHIP_VERSION2              0x00000002 // 0.2 SPOT 2 this was skipped
#define CHIP_VERSION3              0x00000003 // 0.3 SPOT 3 or pre-alpha stage chip
#define CHIP_VERSION4              0x00000004 // 0.4 SPOT 4 or FIDO
#define CHIP_VERSION5              0x00000005 // 1.0 SOLO 1
#define CHIP_VERSION6              0x00000006 // 1.2 SOLO 2
#define CHIP_VERSION7              0x00000007 // 1.3 SOLO 2.1
#define CHIP_VERSION8              0x00000008 // 1.4 SOLO 3 or DINKY

#define CHIP_FAB_CHIP_EXPRESS      0x00000000
#define CHIP_FAB_NEC               0x00000001
#define CHIP_FAB_TOSHIBA           0x00000002

#define CHIP_CNTL_WATCHDOG         0xc0000000
#define CHIP_CNTL_WATCHDOG_BSHIFT  __builtin_popcount(~CHIP_CNTL_WATCHDOG)

#define BUS_INT_SPOT_VIDEO         0x00000080
#define BUS_INT_SPOT_PS2           0x00000040
#define BUS_INT_SPOT_MODEM         0x00000020
#define BUS_INT_SPOT_IR_IN         0x00000010
#define BUS_INT_SPOT_SMARTCARD     0x00000008
#define BUS_INT_SPOT_AUDIO         0x00000004

#define BUS_INT_GPIO_BIT15         0x00020000
#define BUS_INT_GPIO_BIT14         0x00010000
#define BUS_INT_GPIO_BIT13         0x00008000
#define BUS_INT_GPIO_BIT12         0x00004000
#define BUS_INT_GPIO_BIT11         0x00002000
#define BUS_INT_GPIO_BIT10         0x00001000
#define BUS_INT_GPIO_BIT9          0x00000800
#define BUS_INT_GPIO_BIT8          0x00000400
#define BUS_INT_GPIO_BIT7          0x00000200
#define BUS_INT_GPIO_BIT6          0x00000100
#define BUS_INT_GPIO_BIT5          0x00000080
#define BUS_INT_GPIO_BIT4          0x00000040
#define BUS_INT_GPIO_BIT3          0x00000020
#define BUS_INT_GPIO_BIT2          0x00000010
#define BUS_INT_GPIO_BIT1          0x00000008
#define BUS_INT_GPIO_BIT0          0x00000004

#define BUS_INT_VIDEO              0x00000080
#define BUS_INT_VIDEO_DIVUNIT      0x00000020
#define BUS_INT_VIDEO_GFXUNIT      0x00000010
#define BUS_INT_VIDEO_POTUNIT      0x00000008
#define BUS_INT_VIDEO_VIDUNIT      0x00000004

#define BUS_INT_AUDIO              0x00000040
#define BUS_INT_AUDIO_SMODEM_OUT   0x00000040
#define BUS_INT_AUDIO_SMODEM_IN    0x00000020
#define BUS_INT_AUDIO_DIVUNIT      0x00000010
#define BUS_INT_AUDIO_AUDIO_IN     0x00000008
#define BUS_INT_AUDIO_AUDIO_OUT    0x00000004

#define BUS_INT_RIO                0x00000020
#define BUS_INT_RIO_SLOT3          0x00000020
#define BUS_INT_RIO_SLOT2          0x00000010
#define BUS_INT_RIO_SLOT1          0x00000008
#define BUS_INT_RIO_SLOT0          0x00000004

#define BUS_INT_DEV                0x00000010
#define BUS_INT_DEV_GPIO           0x00000080
#define BUS_INT_DEV_UART           0x00000040
#define BUS_INT_DEV_SMARTCARD      0x00000020
#define BUS_INT_DEV_PPORT          0x00000010
#define BUS_INT_DEV_IR_OUT         0x00000008
#define BUS_INT_DEV_IR_IN          0x00000004

#define BUS_INT_TIMER              0x00000008
#define BUS_INT_TIMER_SYSTEM       0x00000008
#define BUS_INT_TIMER_TIMEOUT      0x00000004

#define BUS_INT_FENCE              0x00000004

