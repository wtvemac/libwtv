#ifndef __LIBWTV_STORAGE_IDE_H
#define __LIBWTV_STORAGE_IDE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IDE_DEFAULT_TIMEOUT     (20 * 1000) // 20 seconds

#define IDE_DRIVE0              0x00
#define IDE_DRIVE1              0x01

#define IDE_SECTOR_LENGTH       0x200
#define IDE_SECTOR_BSHIFT       9

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

// These async options are only async for data transfer. They will block sending the command.
typedef enum
{
	IDE_PROTO_NO_DATA       = 0x00,
	IDE_PROTO_PIO_IN        = 0x01,
	IDE_PROTO_PIO_OUT       = 0x02,
	IDE_PROTO_ASYNC_PIO_IN  = 0x03,
	IDE_PROTO_ASYNC_PIO_OUT = 0x04,
	IDE_PROTO_DMA_IN        = 0x05,
	IDE_PROTO_DMA_OUT       = 0x06,
	IDE_PROTO_ASYNC_DMA_IN  = 0x07,
	IDE_PROTO_ASYNC_DMA_OUT = 0x08
} ide_proto;

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
			unsigned selected_drive : 1;
			unsigned address : 4;
		};
	};
} ide_disk_select;

typedef struct 
{
	uint32_t feature;
	uint32_t sector_number;
	uint32_t cylinder_low;
	uint32_t cylinder_high;
	ide_disk_select disk_select;
	uint32_t device_control;
	uint32_t command;
	void* data;
	uint32_t data_length;
} ide_command_block;

bool ide_primary_hd_exists(uint8_t selected_drive, uint32_t timeout);

bool ide_primary_wait_for_status(uint8_t status_mask, uint8_t status_match, uint32_t timeout);

// Will add more reading and writing routines depending on the disk type.
bool ide_primary_read_data16(uint8_t* in_data, uint32_t data_length);

void ide_setup_command(ide_command_block* command_block, uint8_t selected_drive, uint8_t device_control, uint8_t command, uint8_t feature, uint32_t lba_address, void* data, uint32_t data_length);

uint8_t ide_primary_send_command(ide_command_block command_block);

bool ide_primary_handle_nodata_command(bool async, ide_command_block command_block);

bool ide_primary_handle_pioin_command(bool async, ide_command_block command_block);

bool ide_primary_handle_pioout_command(bool async, ide_command_block command_block);

bool ide_primary_handle_command(ide_proto protocol, ide_command_block command_block);

#ifdef __cplusplus
}
#endif

#endif


