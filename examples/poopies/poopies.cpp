#include <libwtv.h>
// There is no storage system right now so data is stored as pre-allocated data within the executable.
#include "fart-sound.h"
#include "poop-emoji.h"
#include "lc2nup_hsb.h"
#include "chill_jingle.h"

// NOTE: the state of libwtv is experimental. This may break over time until I get libwtv in a more stable state.
// Interacting with the SDK will end up being close to how you'd interact with libdragon (with some N64-specific calls removed and some WebTV-specific calls added)

// NOTE: Random functions will be integraded.
#define PHI 0x9e3779b9
static uint32_t Q[4096], c = 362436;
void init_rand(uint32_t x)
{
	int i;

	Q[0] = x;
	Q[1] = x + PHI;
	Q[2] = x + PHI + PHI;

	for (i = 3; i < 4096; i++)
	{
		Q[i] = Q[i - 3] ^ Q[i - 2] ^ PHI ^ i;
	}
}
uint32_t rand_cmwc()
{
	uint64_t t, a = 18782LL;
	static uint32_t i = 4095;
	uint32_t x, r = 0xfffffffe;

	i = (i + 1) & 4095;
	t = a * Q[i] + c;
	c = (t >> 32);
	x = t + c;

	if (x < c)
	{
		x++;
		c++;
    }

	return (Q[i] = r - x);
}

// Squares on all 4 corners.
void draw_fiducials()
{
	const display_context_t disp = display_get();

	// NOTE: the way colors are defined will change.

	int size = 20;

	// Top left
	graphics_draw_box(disp, 0, 0, size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVLNK_COLOR));
	// Top right
	graphics_draw_box(disp, (disp->width - size), 0, size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVHDR_COLOR));
	// Bottom left
	graphics_draw_box(disp, 0, (disp->height - size), size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVHDR_COLOR));
	// Bottom right
	graphics_draw_box(disp, (disp->width - size), (disp->height - size), size, size, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WTVLNK_COLOR));
	
	display_show(disp);
}

void draw_poopie(int x, int y)
{
	const display_context_t disp = display_get();

	// NOTE: the way colors are defined will change.

	for(int dry = 0, sy = y; dry < poop_emoji_height; dry++)
	{
		uint16_t changed = 0;

		for(int drx = 0, sx = x; drx < poop_emoji_width; drx++)
		{
			uint16_t pixel_color = poop_emoji[poop_emoji_start + ((dry * poop_emoji_width) + drx)];

			if(pixel_color != 0x2d80)
			{
				graphics_draw_pixel(disp, sx, sy, pixel_color);

				sx++;
				changed++;
			}
		}

		if(changed > 0)
		{
			sy++;
		}
	}

	display_show(disp);
}

#define kMaxFilenameLen		28
#define DIRECTORY_SEPARATOR '/'
typedef struct FSNode	FSNode;
struct FSNode
{
	FSNode *next;               /* pointer to next file's FSNode */
	FSNode *parent;             /* pointer to parent directory */
	FSNode *first;              /* 0 for object, or pointer to first FSNode in dir */
	char *data;                 /* 0 for directory, or pointer to file data */
	uint32_t dataLength;        /* size of object */
	uint32_t nodeChecksum;
	uint32_t dataChecksum;      /* 0 if data = nil */
	char name[kMaxFilenameLen];
};
typedef struct /* only used by downloader & boot code to verify ROMFS */
{
	uint32_t uint32_count; // find good name?
	uint32_t checksum;
	
} ROMFSHeader;

FSNode* walk_romfs(FSNode* node, const char* file_path)
{
	while(node != NULL)
	{
		char* cur_node_char = (char *)node->name;
		char* cur_path_char = (char *)file_path;
		while(*cur_node_char != NULL && *cur_path_char != DIRECTORY_SEPARATOR && *cur_path_char != NULL)
		{
			// This node doesn't match searched file name, bail and go onto the next (if there is a next).
			if(*cur_path_char != *cur_node_char || (cur_node_char - node->name) >= kMaxFilenameLen)
			{
				goto next_node;
			}

			cur_node_char++;
			cur_path_char++;
		}

		if(node->first != 0)
		{
			return walk_romfs(node->first, (cur_path_char + 1)); // + 1 to go beyond the DIRECTORY_SEPARATOR
		}
		else
		{
			return node;
		}

	next_node:
		node = node->next;
	}

	return NULL;
}

void romfs_tests()
{
	uint32_t* romfs_base = get_bootrom_romfs_base();

	ROMFSHeader* romfs_header = (ROMFSHeader*)((uint32_t)romfs_base - sizeof(ROMFSHeader));

	printf("---------------------------\n");
	printf("HEADER CHECKSUM=0x%08x\n", romfs_header->checksum);
	printf("HEADER SIZE=0x%08x\n", romfs_header->uint32_count);
	printf("---------------------------\n");

	uint32_t* checksum_base = (uint32_t*)((uint32_t)romfs_base - sizeof(ROMFSHeader) - (sizeof(uint32_t) * romfs_header->uint32_count));
	uint32_t checksum = 0x00000000;
	for(uint32_t i = 0; i < romfs_header->uint32_count; i++)
	{
		checksum += *checksum_base;
		checksum_base++;
	}
	printf("CALCULATED CHECKSUM=0x%08x\n", checksum);
	printf("---------------------------\n");

	FSNode* root_node = (FSNode*)((uint32_t)romfs_base - sizeof(ROMFSHeader) - sizeof(FSNode));
	
	//FSNode* found_node = walk_romfs(root_node, "ROM/GMPatches/Data");
	FSNode* found_node = walk_romfs(root_node, "ROM/Sounds/Message.mid");
	if(found_node != NULL)
	{
		printf("found_node->name=%s, found_node->data=%p .. 0x%08x\n", found_node->name, found_node->data, found_node->dataLength);
	}
	else
	{
		printf("FILE NOT FOUND!");
	}
	printf("---------------------------\n");
}

void ide_flash_tests()
{
	printf("\x0a\x0d\x0a\x0d");


	if(is_diskful_box())
	{
		printf("IDE Test:\x0a\x0d");
		
		ata_init();

		if(ata_enabled())
		{
			ata_identity_t identity = ata_get_identity();

			printf("\tsect = %d, heads = %d, cyl = %d, sect*heads*cyl = %d\x0a\x0d", identity.num_sectors_per_track, identity.num_heads, identity.num_cylinders, (identity.num_sectors_per_track*identity.num_heads*identity.num_cylinders));
			printf("\tdefault capacity in sectors = %u\x0a\x0d", (((identity.current_sector_capacity & 0xffff) << 0x10) + (identity.current_sector_capacity >> 0x10)));
			printf("\tLBA capacity in sectors = %u\x0a\x0d", (((identity.user_addressable_sectors & 0xffff) << 0x10) + (identity.user_addressable_sectors >> 0x10)));
			printf("\tMaxMultipleSectors = %d\x0a\x0d", identity.maximum_block_transfer);
			printf("\tCapabilitiesFlags = 0x%04x\x0a\x0d", (identity.capabilities_data >> 0x10));
			printf("\tPIODataXferMode = 0x%02x\x0a\x0d", (identity.pio_cycle_time_mode>>0x08));
			printf("\tMinPIOXferTimeNoIORDY = %d ns\x0a\x0d", identity.minimum_pio_cycle_time);
			printf("\tMinPIOXferTimeIORDY = %d ns\x0a\x0d", identity.minimum_pio_cycle_time_iordy);
			printf("\tFirmware Revision: %.8s\x0a\x0d", identity.firmware_revision);
			printf("\tDevice Name: %.40s\x0a\x0d", identity.model_number);
		}
		else
		{
			printf("\tATA isn't enabled. Drive probably doesn't exist?\x0a\x0d");
		}
	}
	else
	{
		printf("Flash Test:\x0a\x0d");

		flash_init();

		printf("Flash identity: %08x\x0a\x0d", flash_get_identity());
	}

}

int main()
{
	set_leds(2); // NOTE: this will change

	core_init_temp(); // NOTE: this will change

	if(is_solo_box())
	{
		uint8_t reset_cause = 0x00;
		nvram_secondary_read(0x22, &reset_cause, 1);
		if(reset_cause & 0x0c)
		{
			printf("Unplug and re-plug to end up in the bootrom...\x0a\x0d");
			return 0;
		}
	}

	init_rand(2222);

	set_leds(4); // NOTE: this will change

	printf("=============================================================\x0a\x0d");

	display_init(RESOLUTION_560x420, FMT_YUV16, 1, WSRFC_BLACK_COLOR);

	console_alloc(0, 20, 560, 220, HORIZONTAL_PADDING, VERTICAL_PADDING, TAB_WIDTH, LINE_FEED_HEIGHT, true);

	printf("COOL BEANS v1.1!\x0a\x0d");
	printf("Built using eMac's WebTV SDK...\x0a\x0d");
	printf("\x0a\x0d");
	printf("This is a custom putchar implemetation (out serial and screen)\x0a\x0d");
	printf("Using printf code from https://github.com/eyalroz/printf\x0a\x0d");
	printf("Screen font from libdragon (16-bit)\x0a\x0d");
	printf("\x0a\x0d");
	printf("CHIP ID:      0x%08x\x0a\x0d", get_chip_id());
	printf("SYSCONFIG:    0x%08x\x0a\x0d", get_sysconfig());
	printf("RAM SIZE:     %d MB\x0a\x0d", (get_memory_size() / 1024 / 1024));
	printf("SSID:         %s\x0a\x0d", get_ssid_string());
	printf("HD LOCK 0x36: %s\x0a\x0d", ata_get_userpwd_random_string());

	console_close();

	printf("HD PASSWORD:  %s\x0a\x0d", ata_get_userpwd_string());

	printf("Text to screen disabled...\x0a\x0d");

	draw_fiducials();

	printf("Audio init.\x0a\x0d");
	
	BAEMixer test = minibae_init();
	BAEBankToken bank = NULL;
	
	uint32_t* romfs_base = get_bootrom_romfs_base();
	FSNode* root_node = (FSNode*)((uint32_t)romfs_base - sizeof(ROMFSHeader) - sizeof(FSNode));
	FSNode* patchbank_node = walk_romfs(root_node, "ROM/GMPatches/Data");
	BAEResult r2 = BAEMixer_AddBankFromMemory(test, patchbank_node->data, patchbank_node->dataLength, &bank);
	printf("r2=0x%08x, bank=%p\x0a\x0d", r2, bank);

	int16_t pVersionMajor = 0;
	int16_t pVersionMinor = 0;
	int16_t pVersionSubMinor = 0;
	BAEResult r3 = BAEMixer_GetBankVersion(test, bank, &pVersionMajor, &pVersionMinor, &pVersionSubMinor);
	printf("r3=0x%08x, pVersionMajor=%08x, pVersionMinor=%08x, pVersionSubMinor=%08x\x0a\x0d", r3, pVersionMajor, pVersionMinor, pVersionSubMinor);

	BAESong song = BAESong_New(test);
	FSNode* midi_node = walk_romfs(root_node, "ROM/Cache/Music/DialingWebTV.mid");
	//BAEResult r4 = BAESong_LoadMidiFromMemory(song, midi_node->data, midi_node->dataLength, true);
	BAEResult r4 = BAESong_LoadMidiFromMemory(song, &chill_jingle_mid, chill_jingle_mid_len, true);
	printf("r4=0x%08x\x0a\x0d", r4);
	BAESong_DisplayInfo(song);

	BAEResult r5 = BAESong_Start(song, 0);
	printf("r5=0x%08x\x0a\x0d", r4);

	//audio_set_outx_buffer_callback(sound_callback);

	romfs_tests();

	ide_flash_tests();

	printf("Enabling keyboard (IR and/or PS2)... Press any key to get its key map.\x0a\x0d");
	controller_init();

	uint32_t chipval = REGISTER_READ(0xa4000004) & 0x3fffffff;

	while(1)
	{
		controller_scan();

		hid_event event_object = dequeue_hid_event(); // NOTE: this currently doesn't debounce.

		if(event_object.source != EVENT_NULL)
		{
			uint16_t keycode = GET_KEYCODE(event_object.data);

			printf("source=%04x, data=%04x, keycode=%04x\x0a\x0d", event_object.source, event_object.data, keycode);

			if(KY_IS_PRESSED(event_object.data))
			{
				switch(keycode)
				{
					case KY_F1:
					case KY_POWER:
						printf("Power cycle box...\x0a\x0d");

						set_leds(7);

						if(is_spot_box())
						{
							printf("Watchdog reset...\x0a\x0d");
							REGISTER_WRITE(0xa4000004, (chipval | 0x00000000));
							REGISTER_WRITE(0xa4000004, (chipval | 0x40000000));
							REGISTER_WRITE(0xa4000004, (chipval | 0x80000000));
							REGISTER_WRITE(0xa4000004, (chipval | 0xc0000000));
							printf("Idling around like an idiot. The watchdog is going to get us soon!\x0a\x0d");
						}
						else
						{
							printf("Setting MEM_CMD to preserve memory and BUS_RESET_CAUSE_SET to reset. This typically only works on a Derby or Solo rev >= 1.3. You may need to unplug the box if this gets stuck...\x0a\x0d");
							REGISTER_WRITE(0xa4005010, 0xd0000000);
							REGISTER_WRITE(0xa40000a8, 0x00000004);
						}

						while(1) { }
						break;

					/*case KY_F2:
						if(is_solo_box())
						{
							printf("Boot to bootrom hit...\x0a\x0d");

							set_leds(7);

							printf("Writing to IIC NVRAM (secondary NVRAM) to indicate a skip to the bootrom.\x0a\x0d");
							set_box_flag(BOX_FLAG_BOOTROM_BOOT, true);
							
							printf("Setting BUS_RESET_CAUSE_SET to reset.\x0a\x0d");
							REGISTER_WRITE(0xa4005010, 0xd0000000);
							REGISTER_WRITE(0xa40000a8, 0x00000004);
							REGISTER_WRITE(0xa0000404, 0x0000000b);
							REGISTER_WRITE(0xa00007c4, 0x00000000);
						}
						else
						{
							printf("Boot to bootrom not available for this box (yet)...\x0a\x0d");
						}
						break;

					case KY_F4:
						printf("Power cycle box via watchdog hit...\x0a\x0d");

						set_leds(7);
						
						printf("Enabling watchdog timer.\x0a\x0d");
						REGISTER_WRITE(0xa4000004, (chipval | 0x00000000));
						REGISTER_WRITE(0xa4000004, (chipval | 0x40000000));
						REGISTER_WRITE(0xa4000004, (chipval | 0x80000000));
						REGISTER_WRITE(0xa4000004, (chipval | 0xc0000000));
						
						printf("Idling around like an idiot. The watchdog is going to get us soon!\x0a\x0d");
						while(1) { }
						break;*/

					case KY_F5:
						printf("Disable screen hit...\x0a\x0d");

						set_leds(7);

						printf("Turning the screen off.\x0a\x0d");
						display_disable();
						break;

					case KY_F6:
						printf("Enable screen hit...\x0a\x0d");

						set_leds(4);

						printf("Turning the screen on.\x0a\x0d");
						display_enable();
						break;

					case KY_F7:
					{
						printf("Very square...\x0a\x0d");

						const display_context_t disp = display_get();
						graphics_draw_box(disp, 100, 100, 100, 100, WTVCOLOR_TO_WTV2PIXEL(WSRFC_WHITE_COLOR));
						display_show(disp);
						break;
					}

					default:
						BAESound sound = BAESound_New(test);
						BAEResult r6 = BAESound_LoadMemorySample(sound, &fart_sound, fart_sound_size, BAE_WAVE_TYPE);
						BAEResult r7 = BAESound_Start(sound, 1, BAE_FIXED_1, 0);
						printf("MattMan sends his condolences...\x0a\x0d");
						draw_poopie((rand_cmwc() % (560 - 48)), (260 + (rand_cmwc() % 112)));
						break;
				}
			}
		}
	}

	return 0;
}