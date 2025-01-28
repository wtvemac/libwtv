/**
 * @file rioUnit.h
 * @ingroup hardware
 * @brief WebTV's RIO bus unit interface
 */

#include <stdint.h>

#define RIO_CNTL          0xa4001020

#define RIO_SYSCONFIG     0xa4001000

#define RIO_ROM_CNTL0     0xa4001004
#define RIO_ROM_CNTL1     0xa4001008

#define RIO_WTV_CNTL      0xa400101c

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


// From SPOT documentation / reverse engineering:
#define SYSCONFIG_ROM0_TYPE                0x80000000
#define SYSCONFIG_ROM0_MODE                0x40000000
#define SYSCONFIG_ROM0_SPEED               0x30000000
#define SYSCONFIG_ROM0_SPEED_BSHIFT        __builtin_clz(SYSCONFIG_ROM0_SPEED)
#define SYSCONFIG_ROM1_TYPE                0x08000000
#define SYSCONFIG_ROM1_MODE                0x04000000
#define SYSCONFIG_ROM1_SPEED               0x0c000000
#define SYSCONFIG_ROM1_SPEED_BSHIFT       __builtin_clz(SYSCONFIG_ROM1_SPEED)
#define SYSCONFIG_MEM_SPEED                0x00300000
#define SYSCONFIG_MEM_SPEED_BSHIFT         __builtin_clz(SYSCONFIG_MEM_SPEED)
#define SYSCONFIG_MEM_VENDOR               0x00300000
#define SYSCONFIG_MEM_VENDOR_BSHIFT        __builtin_clz(SYSCONFIG_MEM_SPEED)
#define SYSCONFIG_AUD_DAC_TYPE             0x000c0000
#define SYSCONFIG_AUD_DAC_TYPE_BSHIFT      __builtin_clz(SYSCONFIG_AUD_DAC_TYPE)
#define SYSCONFIG_AUD_DAC_MODE             0x00020000
#define SYSCONFIG_VID_CLK_SOURCE           0x00010000
#define SYSCONFIG_SPOT_CPU_CLK_MULT        0x0000c000
#define SYSCONFIG_SPOT_CPU_CLK_MULT_BSHIFT __builtin_clz(SYSCONFIG_CPU_CLK_MULT)
#define SYSCONFIG_CPU_OBUFFER              0x00002000
#define SYSCONFIG_IS_NTSC                  0x00000800
#define SYSCONFIG_BOARD_REV                0x000000f0
#define SYSCONFIG_BOARD_REV_BSHIFT         __builtin_clz(SYSCONFIG_BOARD_REV)
#define SYSCONFIG_BOARD_TYPE               0x0000000c
#define SYSCONFIG_BOARD_TYPE_BSHIFT        __builtin_clz(SYSCONFIG_BOARD_TYPE)

#define ROMSPD_200_100                     0x00000000
#define ROMSPD_100_050                     0x00000001
#define ROMSPD_090_045                     0x00000002
#define ROMSPD_120_060                     0x00000003

#define MEMSPD_100MHZ                      (0x00000000 << SYSCONFIG_MEM_SPEED_BSHIFT)
#define MEMSPD_066MHZ                      (0x00000001 << SYSCONFIG_MEM_SPEED_BSHIFT)
#define MEMSPD_077MHZ                      (0x00000002 << SYSCONFIG_MEM_SPEED_BSHIFT)
#define MEMSPD_083MHZ                      (0x00000003 << SYSCONFIG_MEM_SPEED_BSHIFT)

#define MEMVEND_OTHER                      (0x00000000 << SYSCONFIG_MEM_VENDOR_BSHIFT)
#define MEMVEND_SAMSUNG                    (0x00000001 << SYSCONFIG_MEM_VENDOR_BSHIFT)
#define MEMVEND_FUJITSU                    (0x00000002 << SYSCONFIG_MEM_VENDOR_BSHIFT)
#define MEMVEND_NEC                        (0x00000003 << SYSCONFIG_MEM_VENDOR_BSHIFT)

#define AUD_DAC_AKM4310                    (0x00000003 << SYSCONFIG_AUD_DAC_TYPE_BSHIFT)

#define SPOT_CPU_MULT_5X                   (0x00000000 << SYSCONFIG_SPOT_CPU_CLK_MULT_BSHIFT)
#define SPOT_CPU_MULT_4X                   (0x00000001 << SYSCONFIG_SPOT_CPU_CLK_MULT_BSHIFT)
#define SPOT_CPU_MULT_2X                   (0x00000002 << SYSCONFIG_SPOT_CPU_CLK_MULT_BSHIFT)
#define SPOT_CPU_MULT_3X                   (0x00000003 << SYSCONFIG_SPOT_CPU_CLK_MULT_BSHIFT)

#define BOARD_TYPE_TRIAL                   (0x00000002 << SYSCONFIG_BOARD_TYPE_BSHIFT)
#define BOARD_TYPE_FCS                     (0x00000003 << SYSCONFIG_BOARD_TYPE_BSHIFT)

// From SOLO documentation / reverse engineering:
#define SYSCONFIG_HAS_SMARTCARD0           0x00400000
#define SYSCONFIG_HAS_SMARTCARD1           0x00200000
#define SYSCONFIG_CPU_TYPE                 0x00100000
#define SYSCONFIG_CPU_ENDIAN               0x00080000
#define SYSCONFIG_SOLO_CPU_CLK_MULT        0x00020000
#define SYSCONFIG_SOLO_CPU_CLK_MULT_BSHIFT __builtin_clz(SYSCONFIG_CPU_CLK_MULT)
#define SYSCONFIG_CPU_DRIVE                0x00018000
#define SYSCONFIG_CPU_DRIVE_BSHIFT         __builtin_clz(SYSCONFIG_SOLO_CPU_DRIVE)
#define SYSCONFIG_NO_HD                    0x00000004

#define CPU_DRIVE_067PCT                   (0x00000000 << SYSCONFIG_CPU_DRIVE_BSHIFT)
#define CPU_DRIVE_050PCT                   (0x00000001 << SYSCONFIG_CPU_DRIVE_BSHIFT)
#define CPU_DRIVE_100PCT                   (0x00000002 << SYSCONFIG_CPU_DRIVE_BSHIFT)
#define CPU_DRIVE_083PCT                   (0x00000003 << SYSCONFIG_CPU_DRIVE_BSHIFT)

#define SOLO_CPU_MULT_3X                   (0x00000000 << SYSCONFIG_CPU_CLK_MULT_BSHIFT)
#define SOLO_CPU_MULT_2X                   (0x00000001 << SYSCONFIG_CPU_CLK_MULT_BSHIFT)

#define ROM_CNTL_PAGE_MODE                 0x80000000
#define ROM_CNTL_WRITE_PROTECT             0x40000000
#define ROM_CNTL_CE_DELAY                  0x20000000
#define ROM_CNTL_TIMING_INIT_ACCESS        0x001f0000
#define ROM_CNTL_TIMING_INIT_ACCESS_BSHIFT __builtin_clz(ROM_CNTL_TIMING_INIT_ACCESS)
#define ROM_CNTL_TIMING_PAGE_ACCESS        0x1f000000
#define ROM_CNTL_TIMING_PAGE_ACCESS_BSHIFT __builtin_clz(ROM_CNTL_TIMING_PAGE_ACCESS)
#define ROM_CNTL_TIMING_NEXT_CE            0x00007000
#define ROM_CNTL_TIMING_NEXT_CE_BSHIFT     __builtin_clz(ROM_CNTL_TIMING_NEXT_CE)
#define ROM_CNTL_TIMING_CE_TO_WE           0x00000300
#define ROM_CNTL_TIMING_CE_TO_WE_BSHIFT    __builtin_clz(ROM_CNTL_TIMING_CE_TO_WE)
#define ROM_CNTL_TIMING_WE                 0x0000000f
#define ROM_CNTL_TIMING_WE_BSHIFT          __builtin_clz(ROM_CNTL_TIMING_WE)
#define ROM_CNTL_TIMING                    (ROM_CNTL_TIMING_INIT_ACCESS | ROM_CNTL_TIMING_PAGE_ACCESS | ROM_CNTL_TIMING_NEXT_CE | ROM_CNTL_TIMING_CE_TO_WE | ROM_CNTL_TIMING_WE)
