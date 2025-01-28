/**
 * @file regsinternal.h
 * @brief Register definitions for various hardware in the N64
 * @ingroup lowlevel
 */
#ifndef __LIBWTV_REGS_INTERNAL_H
#define __LIBWTV_REGS_INTERNAL_H

#include <stdint.h>

// MIPS memory map

#define MIPS_KSEG0_BASE_ADDRESS  0x80000000 // Cached
#define MIPS_KSEG1_BASE_ADDRESS  0xa0000000 // Uncached
#define MIPS_KSSEG_BASE_ADDRESS  0xc0000000 // TLB-mapped

// WebTV memory map

#define ROMU_BASE_ADDRESS        0x1f800000
#define ROMU_DATA_SIZE           0x800000   // 8MB

#define DIAG_SPACE_BASE_ADDRESS  0x1f400000
#define DIAG_SPACE_DATA_SIZE     0x400000   // 4MB

#define ROML_BASE_ADDRESS        0x1f000000
#define ROML_DATA_SIZE           0x400000   // 4MB

#define EXDEV7_BASE_ADDRESS      0x07800000
#define EXDEV7_DATA_SIZE         0x800000   // 8MB
#define EXDEV6_BASE_ADDRESS      0x07000000
#define EXDEV6_DATA_SIZE         0x800000   // 8MB
#define EXDEV5_BASE_ADDRESS      0x06800000
#define EXDEV5_DATA_SIZE         0x800000   // 8MB
#define EXDEV4_BASE_ADDRESS      0x06000000
#define EXDEV4_DATA_SIZE         0x800000   // 8MB
#define EXDEV3_BASE_ADDRESS      0x05800000
#define EXDEV3_DATA_SIZE         0x800000   // 8MB
#define EXDEV2_BASE_ADDRESS      0x05000000
#define EXDEV2_DATA_SIZE         0x800000   // 8MB
#define EXDEV1_BASE_ADDRESS      0x04800000
#define EXDEV1_DATA_SIZE         0x800000   // 8MB

#define CNTL_SPACE_BASE_ADDRESS  0x04000000
#define CNTL_SPACE_DATA_SIZE     0x800000   // 8MB

#define RAM_BASE_ADDRESS         0x00000000
#define RAM_DATA_SIZE            0x4000000  // 64MB

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#define ABSOLUTE_GLOBALS_BASE_ADDRESS (0x00000280 | MIPS_KSEG0_BASE_ADDRESS)

typedef struct 
{
	uint32_t ram_size;            // +0x00000000

	uint32_t cpu_speed;           // +0x00000004
	uint32_t bus_speed;           // +0x00000008
	uint32_t counts_per_microsec; // +0x0000000c
	
	uint32_t ssid_hi;             // +0x00000010
	uint32_t ssid_lo;             // +0x00000014
	
	uint32_t ag_checksum;         // +0x00000018
	
	char *heap_base;              // +0x0000001c
	uint32_t heap_size;           // +0x0000001d
	char *stack_base;             // +0x00000021

	char ir_microcode_version;    // +0x00000022

	char pad0;
	char pad1;
	char pad2;

	uint32_t *first_romfs_base;   // +0x00000025

	uint32_t test_vector;         // +0x00000029
} absolute_globals;
#define GET_AG(b, f) (((absolute_globals *)(b))->f) 
#define SET_AG(b, f, v) (((absolute_globals *)(b))->f = v)
#define AG(f) GET_AG(ABSOLUTE_GLOBALS_BASE_ADDRESS, f)
#define AGS(f, v) SET_AG(ABSOLUTE_GLOBALS_BASE_ADDRESS, f, v)

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

// WebTV control space

#define CNTL_BASE_ADDRESS    (0x04000000 | MIPS_KSEG1_BASE_ADDRESS)

#include "regs_internal/busUnit.h"
#include "regs_internal/memUnit.h"
#include "regs_internal/rioUnit.h"
#include "regs_internal/pixelUnits.h"
#include "regs_internal/audUnit.h"
#include "regs_internal/devUnit.h"
#include "regs_internal/sucUnit.h"
#include "regs_internal/divUnit.h"
#include "regs_internal/dveUnit.h"
#include "regs_internal/gfxUnit.h"
#include "regs_internal/modUnit.h"

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

#define BOOTROM_BASE_ADDRESS (0x1fc00000 | MIPS_KSEG1_BASE_ADDRESS)
#define DEFAULT_BOOTROM_SIZE 0x200000

#define APPROM_BASE_ADDRESS  (0x1f000000 | MIPS_KSEG1_BASE_ADDRESS)
#define DEFAULT_APPROM_SIZE  0x200000

#define ALTROM_BASE_ADDRESS  (0x1fe00000 | MIPS_KSEG1_BASE_ADDRESS)
#define DEFAULT_ALTROM_SIZE  0x100000

#define NO_ROMFS_FLAG 0x4e6f4653

typedef struct 
{
	uint32_t ins0;
	uint32_t ins1;

	uint32_t code_checksum;
	uint32_t code_size;
	uint32_t build_size;
	uint32_t build_version;
	uint32_t data_stored_base;
	uint32_t data_stored_size;
	uint32_t bss_size;
	uint32_t romfs_base;
	uint32_t level1_lzj_version;
	uint32_t level1_image_size;
	uint32_t build_base;
	uint32_t build_flags;
	uint32_t data_stored_compressed_size;
	uint32_t bootrom_level1_base;
} build_header;
#define GET_BF(b, f) (((build_header *)(b))->f)

#endif
