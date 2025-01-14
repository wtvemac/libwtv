/**
 * @file regsinternal.h
 * @brief Register definitions for various hardware in the N64
 * @ingroup lowlevel
 */
#ifndef __LIBWTV_REGS_INTERNAL_H
#define __LIBWTV_REGS_INTERNAL_H

#include <stdint.h>
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

/**
 * @defgroup lowlevel Low Level Hardware Interfaces
 * @ingroup libdragon
 * @brief Low level hardware interface descriptions and functionality
 *
 * The low level hardware interfaces handle several functions in the N64 that
 * would otherwise be handled by a kernel or RTOS.  This includes the @ref dma,
 * the @ref exceptions, the @ref interrupt and the @ref n64sys.  The DMA controller
 * handles DMA requests between the cartridge and the N64 RDRAM.  Other systems
 * in the N64 have their own DMA controllers that are handled in the relevant
 * subsystems.  The exception handler traps any exceptions raised by the N64,
 * including the reset exception.  The interrupt handler sets up the MIPS
 * interface (MI) which handles low level interrupt functionality for all other
 * systems in the N64.  The N64 system interface provides the ability for code to
 * manipulate cache and boot options.
 */

/**
 * @brief Register definition for the AI interface
 * @ingroup lowlevel
 */
#define ABSOLUTE_GLOBALS_BASE_ADDRESS 0x80000280

typedef struct 
{
	uint32_t ram_size;

	uint32_t cpu_speed;
	uint32_t bus_speed;
	uint32_t counts_per_microsec;
	
	uint32_t ssid_hi;
	uint32_t ssid_lo;
	
	uint32_t ag_checksum;
	
	char *heap_base;
	uint32_t heap_size;
	char *stack_base;

	char ir_microcode_version;

	char pad0;
	char pad1;
	char pad2;

	uint32_t *first_romfs_base;

	uint32_t test_vector;
} absolute_globals;
#define GET_AG(b, f) (((absolute_globals *)(b))->f) 
#define SET_AG(b, f, v) (((absolute_globals *)(b))->f = v)
#define AG(f) GET_AG(ABSOLUTE_GLOBALS_BASE_ADDRESS, f)
#define AGS(f, v) SET_AG(ABSOLUTE_GLOBALS_BASE_ADDRESS, f, v)

#define BOOTROM_BASE_ADDRESS 0x9fc00000
#define DEFAULT_BOOTROM_SIZE 0x200000
#define NO_ROMFS_BASE 0x4e6f4653

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
