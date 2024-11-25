#include <libwtv.h>

int main()
{
	set_leds(2);
	
	heap_init2();
	//init_memory();
	suc_serial_init();

	uint8_t reset_cause = 0x00;
	read_secnv_data(&reset_cause, 0x22, 1);
	if(reset_cause & 0x0c && is_solo_box())
	{
		printf("Unplug and re-plug to end up in the bootrom...\x0a\x0d");
		return 0;
	}

	graphics_set_default_font();

	set_leds(1);
	display_init(RESOLUTION_560x420, FMT_YUV16, 1, 0xffffffff);

	printf("\x0a\x0d");
	printf("COOL BEANS v1.0!\x0a\x0d");
	printf("Built using eMac's WebTV SDK...\x0a\x0d");
	printf("\x0a\x0d");
	printf("This is a custom putchar implemetation (out serial and screen)\x0a\x0d");
	printf("Using printf code from https://github.com/eyalroz/printf\x0a\x0d");
	printf("Screen font from libdragon (16-bit)\x0a\x0d");
	printf("\x0a\x0d");
	printf("CHIP ID:      0x%08x\x0a\x0d", *((volatile uint32_t *)(0xa4000000)));
	//printf("SYSCONFIG:    0x%08x\x0a\x0d", *((volatile uint32_t *)(RIO_ROM_SYSCONFIG)));
	//printf("RAM SIZE:     %d MB\x0a\x0d", (AG(ram_size) / 1024 / 1024));
	//printf("SSID:         %08x%08x\x0a\x0d", AG(ssid_hi), AG(ssid_lo));

	uint32_t hdrnd_hi = 0x00000000;
	uint32_t hdrnd_lo = 0x00000000;
	uint32_t ahdrnd_hi = 0x36;
	uint32_t ahdrnd_lo = 0x3a;
	for(int i = 0; i < 4; i++)
	{
		uint8_t data = 0x00;

		hdrnd_hi <<= 8;
		read_secnv_data(&data, ahdrnd_hi++, 1);
		hdrnd_hi |= data;
	}

	for(int i = 0; i < 4; i++)
	{
		uint8_t data = 0x00;

		hdrnd_lo <<= 8;
		read_secnv_data(&data, ahdrnd_lo++, 1);
		hdrnd_lo |= data;
	}
	printf("HD LOCK 0x36: %08x%08x\x0a\x0d", hdrnd_hi, hdrnd_lo);
	printf("\x0a\x0d");

	disable_text_to_screen();

	printf("Text to screen disabled...\x0a\x0d");

	display_enable();

	static display_context_t disp = 0;
	disp = display_get();
	//printf("disp->stride=%08x\x0a\x0d", disp->stride);
	graphics_draw_box(disp, 0, 0, 40, 40, graphics_make_color(255, 0, 0, 255));


	//audio_init();

	set_leds(4);

	keyboard_init();
	printf("Enabling IR... Press any IR key to get its key map.\x0a\x0d");

	//poopieit();
	keyboard_poll();

	printf("Looping forever....\x0a\x0d");
	while(1) { }

	return 0;
}