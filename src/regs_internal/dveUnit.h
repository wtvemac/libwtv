/**
 * @file dveUnit.h
 * @ingroup hardware
 * @brief WebTV's digital video encoder unit interface
 */
 
#define DVE_CNTL      0xa4007000

#define DVE_CNFG      0xa4007004

#define DVE_DBG_DATA  0xa4007008
#define DVE_DBG_EN    0xa400700c

#define DVE_DAC_CNTL  0xa4007010

#define DVE_RD_FIELD  0xa4007014
#define DVE_RD_PHASE  0xa4007018

#define DVE_FILT_CNTL 0xa400701c

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

#define DVE_QUAL_INVERT   0x00000008
#define DVE_TGEN          0x00000004
#define DVE_VEN           0x00000002
#define DVE_CEN           0x00000001

#define DVE_SYNC_BYPASS   0x00400000
#define DVE_DAC_PDN       0x00200000
#define DVE_DAC_ORDER     0x00100000
#define DVE_DAC_BYPASS    0x00080000
#define DVE_DTM_EN        0x00040000
#define DVE_TPGEN         0x00020000
#define DVE_PORCH         0x00010000
#define DVE_SETUP_EN      0x00008000
#define DVE_MAT_BYPASS_EN 0x00004000
#define DVE_DRIVE_CSYNC   0x00002000
#define DVE_BLNK_DISABLE  0x00001000
#define DVE_INVERT_DAC    0x00000800
#define DVE_COLOR_KILL    0x00000400
#define DVE_DAC_DITHER_EN 0x00000200
#define DVE_RGB_DITHER_EN 0x00000100
#define DVE_FILTER_MODE   0x00000080
#define DVE_FIXED_IRE     0x00000040
#define DVE_BYPASS_EN     0x00000020
#define DVE_SYNC_LOCK     0x00000010
#define DVE_RGB_OUT       0x00000008
#define DVE_PAL           0x00000004
#define DVE_PROGRESSIVE   0x00000002
#define DVE_NONSQ         0x00000001
