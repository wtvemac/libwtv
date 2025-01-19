#include "storage/flash.h"
#include "interrupt.h"

// Need to copy this to RAM (0x80000000), and be under 0x100 bytes
// This is because the code is usually ran from flash and it'll break if we run flash command or write to flash
// The RAM interrupt vectors also starts at 0x100 so we can't go beyond that.

#define FLASH_BASE (volatile uint32_t*)0xbf000000
#define MASK_BASE  (volatile uint32_t*)0xbfc00000

void flash_init()
{
	//
}

void flash_close()
{
	//
}

uint32_t flash_get_identity()
{
	disable_interrupts();

	uint32_t flash_identity = 0x00000000;
	
	// This is just a test to check if it works. This will be changed.

	uint32_t start_before = *((volatile uint32_t*)(FLASH_BASE) + 0);

	*(FLASH_BASE) = 0x90909090;

	if(start_before != *((volatile uint32_t*)(FLASH_BASE) + 0))
	{
		*(FLASH_BASE) = 0xffffffff;

		flash_identity = *(FLASH_BASE);
	}
	else
	{
		*((volatile uint32_t*)(FLASH_BASE) + 0x00015554) = 0x00aa00aa;
		*((volatile uint32_t*)(FLASH_BASE) + 0x0000aaa8) = 0x00550055;
		*((volatile uint32_t*)(FLASH_BASE) + 0x00015554) = 0x00900090;

		if(start_before != *((volatile uint32_t*)(FLASH_BASE + 0)))
		{
			*(FLASH_BASE) = 0x00f000f0;

			if((start_before != *((volatile uint32_t*)(FLASH_BASE) + 0)))
			{
				flash_identity = *(FLASH_BASE);
			}
		}
		else
		{
			*((volatile uint32_t*)(FLASH_BASE) + 0x00015554) = 0x00aa00aa;
			*((volatile uint32_t*)(FLASH_BASE) + 0x0000aaa8) = 0x00550055;
			*((volatile uint32_t*)(FLASH_BASE) + 0x00015554) = 0x00f000f0;

			if((start_before != *((volatile uint32_t*)(FLASH_BASE) + 0)))
			{
				flash_identity = *(FLASH_BASE);
			}
		}
	}

	enable_interrupts();

	return flash_identity;
}