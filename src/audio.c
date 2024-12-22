#include "audio.h"
#include "regs_internal.h"
#include "wtvsys.h"
#include "interrupt.h"
#include "libc.h"

/** @brief Number of buffers the audio subsytem allocates and manages */
#define DEFAULT_AUDIO_BUFCNT 2
/** @brief How many bytes an audio sample holds in one channel. */
#define BYTES_PER_SAMPLE     (sizeof(asamp))
/** @brief The amount of audio channels to keep track of. */
#define AUDIO_CHANNEL_COUNT  2
/** @brief How many samples an audio buffer holds. 512 is about 11.6ms @ 44100Hz and 10.6ms @ 48000Hz  */
#define SAMPLES_PER_BUFFER   512
/** @brief The size in bytes to allocate for a single audio buffer. */
#define AUDIO_BUFFER_SIZE    ((BYTES_PER_SAMPLE * AUDIO_CHANNEL_COUNT) * SAMPLES_PER_BUFFER)

/** @brief The audio clock frequency the box is running at */
static int _frequency            = DEFAULT_AUDIO_CLOCK;
/** @brief The number of buffers currently allocated */
static int _num_buf              = DEFAULT_AUDIO_BUFCNT;
/** @brief The buffer size in bytes for each buffer allocated */
static int _buf_size             = 0;
/** @brief Array of pointers to the allocated buffers */
static short **buffers          = NULL;
/** @brief Array of pointers to the allocated buffers (original pointers to free) */
static short **buffers_orig      = NULL;

static volatile bool _paused = false;

/** @brief Index of the current playing buffer */
static volatile int now_playing   = 0;
/** @brief Length of the playing queue (number of buffers queued for AI DMA) */
static volatile int playing_queue = 0;
/** @brief Index of the last buffer that has been emptied (after playing) */
static volatile int now_empty     = 0;
/** @brief Index pf the currently being written buffer */
static volatile int now_writing   = 0;
/** @brief Bitmask of buffers indicating which buffers are full */
static volatile int buf_full      = 0;

uint32_t __ak4532_mute(bool bit)
{
	return gpio_write(GPIO_AK5432_ENABLE, !bit);
}

uint32_t __ak4532_select(bool bit)
{
	return gpio_write(GPIO_AK5432_CS, !bit);
}

uint32_t __ak4532_set_cclk(bool bit)
{
	return gpio_write(GPIO_AK5432_CCLK, !bit);
}

uint32_t __ak4532_set_cdata(bool bit)
{
	return gpio_write(GPIO_AK5432_CDATA, !bit);
}

void __ak4532_register_write(uint8_t address, uint8_t data)
{
	__ak4532_select(0);
	__ak4532_set_cclk(0);
	__ak4532_select(1);

	uint16_t cdata = (address << 0x08) | data;

	for (int i = 0; i < 16; i++)
	{
		__ak4532_set_cdata(((cdata & 0x8000) == 0x0000));
		cdata <<= 1;

		__ak4532_set_cclk(1);
		__ak4532_set_cclk(0);
	}

	__ak4532_select(0);

	__ak4532_set_cdata(0);
}

void __audocodec_init()
{
	REGISTER_WRITE(AUD_FMT_CNTL, AUD_CODEC_AK4532);

	__ak4532_register_write(AK5432_MASTER_VOL_LCH, AK5432_D_NONE);
	__ak4532_register_write(AK5432_MASTER_VOL_RCH, AK5432_D_NONE);

	__ak4532_register_write(AK5432_VOICE_VOL_LCH, AK5432_D2_ATT2 | AK5432_D1_ATT1);
	__ak4532_register_write(AK5432_VOICE_VOL_RCH, AK5432_D2_ATT2 | AK5432_D1_ATT1);

	__ak4532_register_write(AK5432_LINE_VOL_LCH, AK5432_D2_ATT2 | AK5432_D1_ATT1);
	__ak4532_register_write(AK5432_LINE_VOL_RCH, AK5432_D2_ATT2 | AK5432_D1_ATT1);

	__ak4532_register_write(AK5432_AUX_VOL_LCH, AK5432_D2_ATT2 | AK5432_D0_ATT0);
	__ak4532_register_write(AK5432_AUX_VOL_RCH, AK5432_D2_ATT2 | AK5432_D0_ATT0);

	__ak4532_mute(false);
}

static void __audio_callback()
{
}

void audio_write_temp(void* buffer, uint32_t length)
{
	REGISTER_WRITE(AUD_OUT_NSTART, PhysicalAddr(buffer));
	REGISTER_WRITE(AUD_OUT_NSIZE, length);

	REGISTER_WRITE(AUD_OUT_DMA_CNTL, AUD_DMA_EN | AUD_NV | AUD_NVF);
}

void audio_init(int frequency, int numbuffers)
{
	if(frequency == 0) 
	{
		frequency = DEFAULT_AUDIO_CLOCK;
	}

	disable_interrupts();

	uint32_t sys_clk = AG(bus_speed);
	uint32_t clk_div = ((sys_clk / 0x100) / frequency);

	if(clk_div > 6)
	{
		clk_div = 6;
	}
	else if(clk_div < 1)
	{
		clk_div = 1;
	}

	_frequency = sys_clk / (clk_div * 0x100);

	REGISTER_WRITE(BUS_CHIP_CNTL, (REGISTER_READ(BUS_CHIP_CNTL) & 0xc3ffffff) | (clk_div << 26));

	if(is_solo_box())
	{
		__audocodec_init();
	}

	/* Make sure we don't choose too many buffers */
	if(numbuffers > (sizeof(buf_full) * 8))
	{
		/* This is a bit mask, so we can only have as many
		 * buffers as we have bits. */
		numbuffers = sizeof(buf_full) * 8;
	}

	/* Set up buffers */
	_buf_size = AUDIO_BUFFER_SIZE;
	_num_buf = (numbuffers > 1) ? numbuffers : DEFAULT_AUDIO_BUFCNT;
	buffers = malloc(sizeof(asamp *) * _num_buf);
	buffers_orig = malloc(sizeof(asamp *) * _num_buf);

	for(int i = 0; i < _num_buf; i++)
	{
		buffers_orig[i] = buffers[i] = malloc_uncached(_buf_size);

		memset(buffers[i], 0, _buf_size);
	}

	/* Set up ring buffer pointers */
	now_playing = 0;
	playing_queue = 0;
	now_empty = 0;
	now_writing = 0;
	buf_full = 0;
	_paused = false;

	REGISTER_WRITE(AUD_OUT_NCONFIG, AUD_16BIT | AUD_STEREO);

	REGISTER_WRITE(AUD_OUT_NSTART, PhysicalAddr(buffers[0]));
	REGISTER_WRITE(AUD_OUT_NSIZE, _buf_size);

	REGISTER_WRITE(AUD_OUT_DMA_CNTL, AUD_DMA_EN | AUD_NV | AUD_NVF);

	//register_AUDIO_OUT_handler(__audio_callback);
	//set_AUDIO_OUT_interrupt(true);

	// audio in interrupt would start here for solo-based boxes.

	enable_interrupts();
}

void audio_close()
{
	unregister_AUDIO_OUT_handler(__audio_callback);
	set_AUDIO_OUT_interrupt(false);
}