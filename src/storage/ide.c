#include "storage/ide.h"
#include "./ide_regs_internal.h"
#include "wtvsys.h"

#define IDE_PROBE_TIMEOUT (20 * 1000) // 20 seconds to wait for the drive to do something.

static uint8_t ide_probe_number = 0x01;

bool ide_primary_hd_exists(uint8_t drive_address)
{
	// The WebTV OS sends "Joe" and B and checks for B.
	
	// We're sending "wtv" and a incremented number, then check for that number.
	// The random number is so we can call this multiple times and see the change.

	uint64_t start = get_ticks_ms();

	ide_probe_number++;

	// There can only be two devices on an IDE bus, addressed by 0 or 1.
	drive_address &= 0x01;

	while((get_ticks_ms() - start) < IDE_PROBE_TIMEOUT)
	{
		ide_disk_select disk_select = {
			.data = IDE_PRI(disk_select)
		};

		disk_select.drive_address = drive_address;

		IDE_PRI_SET(disk_select, disk_select.data);

		if(disk_select.using_lba)
		{
			IDE_PRI_SET(sector_count,  0x77); // w
			IDE_PRI_SET(sector_number, 0x74); // t
			IDE_PRI_SET(cylinder_low,  0x76); // v
			IDE_PRI_SET(cylinder_high, ide_probe_number);

			if((IDE_PRI(cylinder_high) & 0xff) == ide_probe_number)
			{
				return true;
			}
		}
	}

	return false;
}