/**
 * @file sucUnit.h
 * @ingroup hardware
 * @brief WebTV's Smartcard and UART unit interface
 */

#include <stdint.h>

// SOLO: 5.2.15.1 UART FIFO Registers

#define SUC_TFFHR                0x00000000
#define SUC_TFFHRSRW             0x00000004

#define SUC_RFFHR                0x00000040
#define SUC_RFFHRSRW             0x00000044

// SOLO: 5.2.15.2 TFFTRG Registers

#define SUC_TFFTRG               0x00000008
#define SUC_TFFCNT               0x0000000c
#define SUC_TFFMAX               0x00000010
#define SUC_TFFCR                0x00000014
#define SUC_TFFSR                0x00000018
#define SUC_TFFGCR               0x0000001c

#define SUC_RFFTRG               0x00000048
#define SUC_RFFCNT               0x0000004c
#define SUC_RFFMAX               0x00000050
#define SUC_RFFCR                0x00000054
#define SUC_RFFSR                0x00000058
#define SUC_RFFGCR               0x0000005c

// SOLO: 5.2.15.3 Shift Register Registers

#define SUC_TSRCR                0x00000080
#define SUC_TSRSTATE             0x00000084
#define SUC_TSRBCCNT             0x00000088
#define SUC_TSRBITCNT            0x0000008c

#define SUC_RSRCR                0x000000c0
#define SUC_RSRSTATE             0x000000c4
#define SUC_RSRBCCNT             0x000000c8
#define SUC_RSRBITCNT            0x000000cc

// SOLO: 5.2.15.4 Clock Divider Registers

#define SUC_MCD                  0x00000100
#define SUC_SCDO                 0x00000104
#define SUC_SCD1                 0x00000108
#define SUC_CCR                  0x0000010c

// SOLO: 5.2.15.5 Line Control and Status Registers

#define SUC_LCR                  0x00000180
#define SUC_LSCR                 0x00000184
#define SUC_LSTPBITS             0x00000188
#define SUC_LSR                  0x0000018c

// SOLO: 5.2.15.6 Interrupt Registers

#define SUC_INT_STAT             0x00000200
#define SUC_INT_STAT_RESET       0x00000204

#define SUC_INT_EN_SET           0x00000208
#define SUC_INT_EN_CLEAR         0x0000020c
#define SUC_INT_EN_STATE         0x00000210

// SOLO: 5.2.15.7 Smart Card Deactivation Registers 

#define SUC_SDSM_CNTL            0x00000280
#define SUC_SDSM_STATE           0x00000284

// SOLO: 5.2.15.8 GPIO Registers

#define SUC_IOOD                 0x00000288
#define SUC_SPIO_CNTL            0x0000028c

#define SUC_SPIO_EN_SET          0x00000290
#define SUC_SPIO_EN_CLEAR        0x00000294
#define SUC_SPIO_EN_STATE        0x00000298

#define SUC_GPIO_DIR_SET         0x000002a0
#define SUC_GPIO_DIR_CLEAR       0x000002a4
#define SUC_GPIO_DIR_STATE       0x000002a8

#define SUC_GPIO_VAL_SET         0x000002b0
#define SUC_GPIO_VAL_CLEAR       0x000002b4
#define SUC_GPIO_VAL_STATE       0x000002b8

#define SUC_GPIOR_INT_EN_SET     0x000002c0
#define SUC_GPIOR_INT_EN_CLEAR   0x000002c4
#define SUC_GPIOR_INT_EN_STATE   0x000002c8

#define SUC_GPIO_INT_EN_SET      0x000002d0
#define SUC_GPIO_INT_EN_CLEAR    0x000002d4
#define SUC_GPIO_INT_EN_STATE    0x000002d8

#define SUC_GPIO_INT_STAT_SET    0x000002e0
#define SUC_GPIO_INT_STAT_CLEAR  0x000002e4
#define SUC_GPIO_INT_STAT_STATE  0x000002e8

#define SUC_GPIOR_INT_STAT_STATE 0x000002ec

#define SUC_GPIOF_INT_STAT_STATE 0x000002f0

// SOLO: 5.2.15.9 Diagnostic Registers

#define SUC_DIAGMODE             0x00000300

// SPOT: Smartcard control (within devUnit)

#define DEV_SMARTCARD_CNTL       0xa4004010

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define DEV_SMARTCARD_CNTL       0xa4004010
#define SUC_GPIO_VAL_DCD_N       0x00000020
#define SUC_GPIO_VAL_SCICTS_N    0x00000010
#define SUC_GPIO_VAL_SCRRTS_N    0x00000008
#define SUC_GPIO_VAL_SCPDTR_N    0x00000004
#define SUC_GPIO_VAL_SCCTXD      0x00000002
#define SUC_GPIO_VAL_SCDRXD      0x00000001

#define SUC_GPIO_DIR_DCD_N       0x00000020
#define SUC_GPIO_DIR_SCICTS_N    0x00000010
#define SUC_GPIO_DIR_SCRRTS_N    0x00000008
#define SUC_GPIO_DIR_SCPDTR_N    0x00000004
#define SUC_GPIO_DIR_SCCTXD      0x00000002
#define SUC_GPIO_DIR_SCDRXD      0x00000001

#define SUC_SPIO_SELECT_SCCTXD   0x00000002
#define SUC_SPIO_SELECT_SCDRXD   0x00000001

#define SUC_SPIO_SCRRTS_N        0x00000004
#define SUC_SPIO_SCCTXD_EN       0x00000002
#define SUC_SPIO_SCDRXD_EN       0x00000001

#define SUC_RFIFO_ERROR          0x00000080
#define SUC_TRNS_ERROR           0x00000040
#define SUC_TFIFO_EMPTY          0x00000020
#define SUC_BREAK_DETECT         0x00000010
#define SUC_FRAME_ERROR          0x00000008
#define SUC_PARITY_ERROR         0x00000004
#define SUC_OVERRUN_ERROR        0x00000002
#define SUC_DATA_READY           0x00000001

#define SUC_NOTIFY_PARITY        0x00000004
#define SUC_INVERT_BITPOL        0x00000004
#define SUC_SWAP_BITORDER        0x00000004

#define SUC_SAMPDATA_SYSCLK      0x00000004
#define SUC_SAMPCLK_EN           0x00000002
#define SUC_MASTCLK_EN           0x00000001

#define SUC_SET_BREAK            0x00000040
#define SUC_STICK_PARITY         0x00000020
#define SUC_EVEN_PARITY          0x00000010
#define SUC_PARITY_EN            0x00000008
#define SUC_8BIT_CHAR            0x00000003
#define SUC_7BIT_CHAR            0x00000002
#define SUC_6BIT_CHAR            0x00000001
#define SUC_5BIT_CHAR            0x00000000

#define SUC_FF_SW_VISABLE        0x00000004
#define SUC_FF_ENABLE            0x00000002
#define SUC_FF_RESET             0x00000001

#define SUC_IOOD_DCD_N           0x00000020
#define SUC_IOOD_SCICTS_N        0x00000010
#define SUC_IOOD_SCRRTS_N        0x00000008
#define SUC_IOOD_SCPTR_N         0x00000004
#define SUC_IOOD_SCCTXD          0x00000002
#define SUC_IOOD_SCDRXD          0x00000001

#define SUC_TSR_CTS_FOWCNTL      0x00000002
#define SUC_TSR_EN               0x00000001

#define SUC_RSR_FILT_DISABLE     0x00000002
#define SUC_RSR_EN               0x00000001

// SET0 is usually assigned to a general purpose UART interface (SUCGPU).
// SET1 is usually assigned to a SmartCard interface (SUCSC0).

#define SUC_SET0_BASE            0xa400a000
#define SUC_SET1_BASE            0xa400a800

