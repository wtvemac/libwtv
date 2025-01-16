#ifndef __LIBWTV_STORAGE_IDE_REGS_INTERNAL_H
#define __LIBWTV_STORAGE_IDE_REGS_INTERNAL_H

#include <stdint.h>

// RIO device slots 1 and 2
#define IDE_PRI_BASE       0xbe400000
// RIO device slots 5 and 6
#define IDE_SEC_BASE       0xbd400000

#define IDE_ERROR_BBK           0x80 // Bad block
#define IDE_ERROR_UNC           0x40 // Uncorrectable data
#define IDE_ERROR_MC            0x20 // Media changed
#define IDE_ERROR_IDNF          0x10 // ID mark not found
#define IDE_ERROR_MCR           0x08 // Media change request
#define IDE_ERROR_ABRT          0x04 // Command aborted
#define IDE_ERROR_TKNOF         0x02 // Track 0 not found
#define IDE_ERROR_AMNF          0x01 // No address mark

#define IDE_STATUS_BSY          0x80 // Busy
#define IDE_STATUS_DRDY         0x40 // Drive ready
#define IDE_STATUS_DWF          0x20 // Drive write fault
#define IDE_STATUS_DSC          0x10 // Drive seek complete
#define IDE_STATUS_DRQ          0x08 // Drive request ready
#define IDE_STATUS_CORR         0x04 // Corrected data
#define IDE_STATUS_IDX          0x02 // Index
#define IDE_STATUS_ERR          0x01 // Error

typedef struct 
{
	uint32_t data;             // +0x000000
	uint32_t error_or_feature; // +0x000004
	uint32_t sector_count;     // +0x000008
	uint32_t sector_number;    // +0x00000c
	uint32_t cylinder_low;     // +0x000010
	uint32_t cylinder_high;    // +0x000014
	uint32_t disk_select;      // +0x000018 aka Drive/Head register
	uint32_t status;           // +0x00001c
	uint8_t _padding[0x00400000];
	uint32_t alternate_status; // +0x400018
	uint32_t drive_address;    // +0x40001c
} ide_device_registers;
#define GET_IDE_REG(b, f) (((ide_device_registers *)(b))->f)
#define SET_IDE_REG(b, f, v) (((ide_device_registers *)(b))->f = v)
#define IDE_PRI(f) GET_IDE_REG(IDE_PRI_BASE, f)
#define IDE_PRI_SET(f, v) SET_IDE_REG(IDE_PRI_BASE, f, v)
#define IDE_SEC(f) GET_IDE_REG(IDE_SEC_BASE, f)
#define IDE_SEC_SET(f, v) SET_IDE_REG(IDE_SEC_BASE, f, v)

typedef struct 
{
	union
	{
		struct
		{
			uint32_t data : 32;
		};
		struct
		{
			unsigned unused : 24;
			unsigned always1_a : 1;
			unsigned using_lba : 1;
			unsigned always1_b : 1;
			unsigned drive_address : 1;
			unsigned address : 4;
		};
	};
} ide_disk_select;

#endif