/**
 * @file memUnit.h
 * @ingroup hardware
 * @brief WebTV's main memory unit interface
 */

#include <stdint.h>

// SOLO boxes

#define MEM_CNTL          0xa4005004

#define MEM_CMD           0xa4005010
#define MEM_REF_CNTL      0xa400500c

#define MEM_TIMING        0xa4005000
#define MEM_BURP          0xa4005008

// SPOT boxes

#define MEM_SPOT_CNTL     0xa4005000

#define MEM_SPOT_CMD      0xa400500c
#define MEM_SPOT_REF_CNTL 0xa4005004

#define MEM_SPOT_TIMING   0xa4005010

#define MEM_SPOT_DATA     0xa4005008
