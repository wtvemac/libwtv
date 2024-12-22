#include "audio.h"
#include "regs_internal.h"
#include "wtvsys.h"
#include "interrupt.h"
#include "libc.h"
#include "utils.h"

/** @brief The audio clock frequency the box is running at */
static int _frequency            = DEFAULT_AUDIO_CLOCK;
/** @brief The number of buffers currently allocated */
static int _num_buf              = DEFAULT_AUDIO_BUFCNT;
/** @brief The buffer size in bytes for each buffer allocated */
static int _buf_size             = 0;
/** @brief Array of pointers to the allocated buffers */
static asamp **buffers           = NULL;
/** @brief Array of pointers to the allocated buffers (original pointers to free) */
static asamp **buffers_orig      = NULL;

static audio_fill_out_buffer_callback _fill_out_buffer_callback = NULL;
static audio_fill_out_buffer_callback _orig_fill_out_buffer_callback = NULL;
static audio_fill_outx_buffer_callback _fill_outx_buffer_callback = NULL;
static audio_fill_outx_buffer_callback _orig_fill_outx_buffer_callback = NULL;

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

	for(int i = 0; i < 16; i++)
	{
		__ak4532_set_cdata(((cdata & 0x8000) == 0x0000));
		cdata <<= 1;

		__ak4532_set_cclk(1);
		__ak4532_set_cclk(0);
	}

	__ak4532_select(0);

	__ak4532_set_cdata(0);
}

void __ak4532_init()
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

void __audocodec_init()
{
	__ak4532_init();
}

/**
 * @brief Send next available chunks of audio data to the AI
 *
 * This function is called whenever internal buffers are running low.  It will
 * send as many buffers as possible to the AI until the AI is full.
 */
static void __audio_out_callback()
{

	if(_fill_outx_buffer_callback)
	{
		/* Disable interrupts so we don't get a race condition with writes */
		disable_interrupts();

		audio_callback_data data = _fill_outx_buffer_callback();

		REGISTER_WRITE(AUD_OUT_NSTART, PhysicalAddr(data.buffer));
		REGISTER_WRITE(AUD_OUT_NSIZE, data.length);

		/* Safe to enable interrupts here */
		enable_interrupts();
	}
	else if(!buffers) /* Do not copy more data if we've freed the audio system */
	{

		/* Disable interrupts so we don't get a race condition with writes */
		disable_interrupts();

		/* Check how many queued buffers were consumed, and update buf_full flags
		accordingly, to make them available for further writes. */
		if(playing_queue > 1) {
			playing_queue--;
			now_empty = (now_empty + 1) % _num_buf;
			buf_full &= ~(1<<now_empty);
		}
		if(playing_queue > 0) {
			playing_queue--;
			now_empty = (now_empty + 1) % _num_buf;
			buf_full &= ~(1<<now_empty);
		}

		/* Copy in as many buffers as can fit (up to 2) */
		while(playing_queue < 2)
		{
			/* check if next buffer is full */
			int next = (now_playing + 1) % _num_buf;
			
			if((!(buf_full & (1 << next))) && !_fill_out_buffer_callback)
			{
				break;
			}

			if(_fill_out_buffer_callback)
			{
				_fill_out_buffer_callback(buffers[next], SAMPLES_PER_BUFFER);
			}

			/* Enqueue next buffer. Don't mark it as empty right now because the
			DMA will run in background, and we need to avoid audio_write()
			to reuse it before the DMA is finished. */
			REGISTER_WRITE(AUD_OUT_NSTART, PhysicalAddr(buffers[next]));
			REGISTER_WRITE(AUD_OUT_NSIZE, _buf_size);

			/* Remember that we queued one buffer */
			playing_queue++;
			now_playing = next;
		}

		/* Safe to enable interrupts here */
		enable_interrupts();
	}

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

	if(numbuffers > -1)
	{
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

	register_AUDIO_OUT_handler(__audio_out_callback);
	set_AUDIO_OUT_interrupt(true);

	// audio in interrupt would start here for solo-based boxes.

	enable_interrupts();
}

void audio_close()
{
	unregister_AUDIO_OUT_handler(__audio_out_callback);
	set_AUDIO_OUT_interrupt(false);

	if(buffers)
	{
		for(int i = 0; i < _num_buf; i++)
		{
			/* Nuke anything that isn't freed */
			if(buffers_orig[i])
			{
				free_uncached(buffers_orig[i]);
				buffers_orig[i] = buffers[i] = 0;
			}
		}

		/* Nuke array of buffers we init'd earlier */
		free(buffers);
		buffers = 0;
		free(buffers_orig);
		buffers_orig = 0;
	}

	_frequency = 0;
	_buf_size = 0;
}

void audio_set_out_buffer_callback(audio_fill_out_buffer_callback fill_out_buffer_callback)
{
	disable_interrupts();
	_orig_fill_out_buffer_callback = fill_out_buffer_callback;
	if(!_paused)
	{
		_fill_out_buffer_callback = fill_out_buffer_callback;
	}
	enable_interrupts();
}

void audio_set_outx_buffer_callback(audio_fill_outx_buffer_callback fill_outx_buffer_callback)
{
	disable_interrupts();
	_orig_fill_outx_buffer_callback = fill_outx_buffer_callback;
	if(!_paused)
	{
		_fill_outx_buffer_callback = fill_outx_buffer_callback;
	}
	enable_interrupts();
}

static void __audio_paused_callback(asamp *buffer, size_t numsamples)
{
	memset(buffer, 0, numsamples * sizeof(asamp) * 2);
}

static audio_callback_data __audio_pausedx_callback()
{
	return (audio_callback_data) {
		.buffer = 0,
		.length = 0
	};
}

void audio_pause(bool pause)
{
	if(pause != _paused && (_fill_outx_buffer_callback || _fill_out_buffer_callback))
	{
		disable_interrupts();

		_paused = pause;
		if(_fill_outx_buffer_callback)
		{
			if (pause)
			{
				_orig_fill_outx_buffer_callback = _fill_outx_buffer_callback;
				_fill_outx_buffer_callback = __audio_pausedx_callback;
			}
			else
			{
				_fill_outx_buffer_callback = _orig_fill_outx_buffer_callback;
			}
		}
		else
		{
			if(pause)
			{
				_orig_fill_out_buffer_callback = _fill_out_buffer_callback;
				_fill_out_buffer_callback = __audio_paused_callback;
			}
			else
			{
				_fill_out_buffer_callback = _orig_fill_out_buffer_callback;
			}
		}

		enable_interrupts();
	}
}

/**
 * @brief Write a chunk of audio data
 *
 * This function takes a chunk of audio data and writes it to an internal
 * buffer which will be played back by the audio system as soon as room
 * becomes available.  The buffer should contain stereo interleaved
 * samples and be exactly #audio_get_buffer_length stereo samples long.
 * 
 * To improve performance and avoid the memory copy, use #audio_write_begin
 * and #audio_write_end instead.
 *
 * @note This function will block until there is room to write an audio sample.
 *	   If you do not want to block, check to see if there is room by calling
 *	   #audio_can_write.
 *
 * @param[in] buffer
 *			Buffer containing stereo samples to be played
 */
void audio_write(const asamp * const buffer)
{
	if(!buffers)
	{
		return;
	}

	disable_interrupts();

	/* check for empty buffer */
	int next = (now_writing + 1) % _num_buf;
	while(buf_full & (1<<next))
	{
		// buffers full
		__audio_out_callback();
		enable_interrupts();
		disable_interrupts();
	}

	/* Copy buffer into local buffers */
	buf_full |= (1<<next);
	now_writing = next;
	memcpy(buffers[now_writing], buffer, _buf_size * 2 * sizeof(asamp));
	__audio_out_callback();
	enable_interrupts();
}

asamp* audio_write_begin(void) 
{
	if(!buffers)
	{
		return NULL;
	}

	disable_interrupts();

	/* check for empty buffer */
	int next = (now_writing + 1) % _num_buf;
	while(buf_full & (1<<next))
	{
		// buffers full
		__audio_out_callback();
		enable_interrupts();
		disable_interrupts();
	}

	/* Copy buffer into local buffers */
	now_writing = next;
	enable_interrupts();

	return buffers[now_writing];
}

void audio_write_end(void)
{
	disable_interrupts();
	buf_full |= (1<<now_writing);
	__audio_out_callback();
	enable_interrupts();
}

void audio_write_silence()
{
	if(!buffers)
	{
		return;
	}

	disable_interrupts();

	/* check for empty buffer */
	int next = (now_writing + 1) % _num_buf;
	while(buf_full & (1<<next))
	{
		// buffers full
		__audio_out_callback();
		enable_interrupts();
		disable_interrupts();
	}

	/* Copy silence into local buffers */
	buf_full |= (1<<next);
	now_writing = next;
	memset(buffers[now_writing], 0, _buf_size * 2 * sizeof(asamp));
	__audio_out_callback();
	enable_interrupts();
}

volatile int audio_can_write()
{
	if(!buffers)
	{
		return 0;
	}

	/* check for empty buffer */
	int next = (now_writing + 1) % _num_buf;
	return (buf_full & (1<<next)) ? 0 : 1;
}

int audio_push(const asamp *buffer, int nsamples, bool blocking)
{
	static asamp *dst = NULL;
	static int dst_sz = 0;
	int written = 0;

	while(nsamples > 0 && (blocking || dst || audio_can_write())) {
		if(!dst)
		{
			dst = audio_write_begin();
			dst_sz = audio_get_buffer_length();
		}

		int ns = MIN(nsamples, dst_sz);
		memcpy(dst, buffer, ns*2*sizeof(asamp));

		buffer += ns*2;
		dst += ns*2;
		nsamples -= ns;
		dst_sz -= ns;
		written += ns;

		if(dst_sz == 0)
		{
			audio_write_end();
			dst = NULL;
		}
	}

	return written;
}

int audio_get_frequency()
{
	return _frequency;
}

int audio_get_buffer_length()
{
	return _buf_size;
}