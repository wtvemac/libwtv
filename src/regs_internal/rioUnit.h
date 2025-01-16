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

#define SYSCONFIG_NO_HD   0x04
