#ifndef __LIBWTV_STORAGE_IDE_H
#define __LIBWTV_STORAGE_IDE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IDE_DEFAULT_TIMEOUT     (20 * 1000)

#define IDE_DRIVE0              0x00
#define IDE_DRIVE1              0x01

#define IDE_SECTOR_LENGTH       0x200
#define IDE_SECTOR_OF_MASK      (IDE_SECTOR_LENGTH - 1)
#define IDE_SECTOR_OF_BSHIFT    __builtin_popcount(IDE_SECTOR_OF_MASK)

#define IDE_ERROR_NONE          0x00 // No error bits
#define IDE_ERROR_BBK           0x80 // Bad block
#define IDE_ERROR_UNC           0x40 // Uncorrectable data
#define IDE_ERROR_MC            0x20 // Media changed
#define IDE_ERROR_IDNF          0x10 // ID mark not found
#define IDE_ERROR_MCR           0x08 // Media change request
#define IDE_ERROR_ABRT          0x04 // Command aborted
#define IDE_ERROR_TKNOF         0x02 // Track 0 not found
#define IDE_ERROR_AMNF          0x01 // No address mark

#define IDE_STATUS_NONE         0x00 // No status bits
#define IDE_STATUS_BSY          0x80 // Busy
#define IDE_STATUS_DRDY         0x40 // Drive ready
#define IDE_STATUS_DWF          0x20 // Drive write fault
#define IDE_STATUS_DSC          0x10 // Drive seek complete
#define IDE_STATUS_DRQ          0x08 // Drive request ready
#define IDE_STATUS_CORR         0x04 // Corrected data
#define IDE_STATUS_IDX          0x02 // Index
#define IDE_STATUS_ERR          0x01 // Error

#define IDE_REG_PADLEN     0x003ffff8

// These async options are only async for data transfer. They will block sending the command.
typedef enum
{
	IDE_PROTO_NO_DATA       = 0x00,
	IDE_PROTO_SYNC_PIO_IN   = 0x01,
	IDE_PROTO_SYNC_PIO_OUT  = 0x02,
	IDE_PROTO_PIO_IN        = 0x03,
	IDE_PROTO_PIO_OUT       = 0x04,
	IDE_PROTO_SYNC_DMA_IN   = 0x05,
	IDE_PROTO_SYNC_DMA_OUT  = 0x06,
	IDE_PROTO_DMA_IN        = 0x07,
	IDE_PROTO_DMA_OUT       = 0x08
} ide_proto;

typedef struct
{
	union
	{
		struct
		{
			uint32_t data;
		};
		struct
		{
			unsigned unused : 24;
			unsigned always1_a : 1;
			unsigned using_lba : 1;
			unsigned always1_b : 1;
			unsigned selected_drive : 1;
			unsigned address : 4;
		};
	};
} ide_disk_select_t;

typedef struct __attribute__((__packed__))
{
	uint32_t data;                    // +0x000000
	uint32_t error_or_feature;        // +0x000004
	uint32_t sector_count;            // +0x000008
	uint32_t sector_number;           // +0x00000c
	uint32_t cylinder_low;            // +0x000010
	uint32_t cylinder_high;           // +0x000014
	ide_disk_select_t disk_select;    // +0x000018 aka Drive/Head register
	uint32_t status_or_command;       // +0x00001c
	uint8_t _padding[IDE_REG_PADLEN];
	uint32_t altstatus_or_devcontrol; // +0x400018
	uint32_t device_address;          // +0x40001c
} ide_device_registers_t;

typedef struct 
{
	ide_disk_select_t disk_select;
	uint32_t sector_number;
	uint32_t cylinder_low;
	uint32_t cylinder_high;
	uint32_t device_control;
	uint32_t feature;
	uint32_t command;
	uint16_t data_skip;
	void* data;
	uint32_t data_length;
	uint32_t sector_count;
} ide_command_block_t;

void ide_init();

bool ide_probe(uint32_t timeout);

volatile ide_device_registers_t* ide_get_base(uint8_t selected_bus);

bool ide_wait_for_status(uint8_t status_mask, uint8_t status_match, uint32_t timeout);

bool ide_check_for_status(uint8_t status_mask, uint8_t status_match);

bool ide_read_data16(uint16_t data_skip, void* in_data, uint32_t data_length, uint8_t sector_count);

bool ide_write_data16(void* out_data, uint32_t data_length, uint8_t sector_count);

void ide_setup_command(ide_command_block_t* command_block, uint8_t selected_drive, uint8_t device_control, uint8_t command, uint8_t feature, uint64_t data_offset, void* data, uint32_t data_length);

uint8_t ide_send_command(ide_command_block_t* command_block);

bool ide_handle_simple_command(bool async, ide_command_block_t* command_block);

bool ide_handle_pio_in_command(bool async, ide_command_block_t* command_block);

bool ide_handle_pio_out_command(bool async, ide_command_block_t* command_block);

bool ide_handle_command(ide_proto protocol, ide_command_block_t* command_block);

#ifdef __cplusplus
}
#endif

#endif


