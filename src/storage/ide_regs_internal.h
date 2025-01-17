#ifndef __LIBWTV_STORAGE_IDE_REGS_INTERNAL_H
#define __LIBWTV_STORAGE_IDE_REGS_INTERNAL_H

#include <stdint.h>

// RIO device slots 1 and 2
#define IDE_PRI_BASE       0xbe400000
// RIO device slots 5 and 6
#define IDE_SEC_BASE       0xbd400000

#define IDE_REG_PADLEN     0x00400000

typedef struct __attribute__((__packed__))
{
	uint32_t data;                    // +0x000000
	uint32_t error_or_feature;        // +0x000004
	uint32_t sector_count;            // +0x000008
	uint32_t sector_number;           // +0x00000c
	uint32_t cylinder_low;            // +0x000010
	uint32_t cylinder_high;           // +0x000014
	ide_disk_select disk_select;      // +0x000018 aka Drive/Head register
	uint32_t status_or_command;       // +0x00001c
	uint8_t _padding[IDE_REG_PADLEN];
	uint32_t altstatus_or_devcontrol; // +0x400018
	uint32_t device_address;          // +0x40001c
} ide_device_registers;
#define GET_IDE_REG(b, f) (((volatile ide_device_registers *)(b))->f)
#define SET_IDE_REG(b, f, v) (((volatile ide_device_registers *)(b))->f = v)
#define IDE_PRI(f) GET_IDE_REG(IDE_PRI_BASE, f)
#define IDE_PRI_SET(f, v) SET_IDE_REG(IDE_PRI_BASE, f, v)
#define IDE_SEC(f) GET_IDE_REG(IDE_SEC_BASE, f)
#define IDE_SEC_SET(f, v) SET_IDE_REG(IDE_SEC_BASE, f, v)

#endif