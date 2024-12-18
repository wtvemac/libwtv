/**
 * @file display.c
 * @brief Display Subsystem
 * @ingroup display
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "regs_internal.h"
#include "wtvsys.h"
#include "display.h"
#include "interrupt.h"
#include "utils.h"
#include "libc.h"
//EMAC:#include "debug.h"
#include "surface.h"
#include "serial.h"

/** @brief Maximum number of video backbuffers */
#define NUM_BUFFERS         32
/** @brief Number of past frames used to evaluate FPS */
#define FPS_WINDOW          32

static surface_t *surfaces;
/** @brief Currently active bit depth */
static uint32_t __bitdepth;
/** @brief Currently active video width (or hsize; calculated) */
static uint32_t __width;
/** @brief Currently active video height (or vsize; calculated) */
static uint32_t __height;
/** @brief Currently active video hstart (calculated) */
static uint32_t __hstart;
/** @brief Currently active video vstart (calculated) */
static uint32_t __vstart;
/** @brief Number of active buffers */
static uint32_t __buffers = NUM_BUFFERS;
/** @brief Pointer to uncached 16-bit aligned version of buffers */
static void *__safe_buffer[NUM_BUFFERS];
/** @brief Currently displayed buffer */
static int now_showing = -1;
/** @brief Bitmask of surfaces that are currently being drawn */
static uint32_t locked_display_mask = 0;
/** @brief Bitmask of surfaces that are ready to be shown */
static volatile uint32_t waiting_for_show_mask = 0;
/** @brief Window of absolute times at which previous frames were shown */
static uint32_t frame_times[FPS_WINDOW];
/** @brief Current index into the frame times window */
static int frame_times_index = 0;
/** @brief Current duration of the frame window (time elapsed for FPS_WINDOW frames) */
static uint32_t frame_times_duration;

/** @brief Get the next buffer index (with wraparound) */
static inline int buffer_next(int idx) {
    idx++;
    if (idx == __buffers)
    {
        idx = 0;
    }
    return idx;
}

/**
 * @brief Interrupt handler for vertical blank
 *
 * If there is another frame to display, display the frame
 */
static void __display_callback()
{
    /* Least significant bit of the current line register indicates
       if the currently displayed field is odd or even. */
    //bool field = (*VI_V_CURRENT) & 1;
    //bool interlaced = (*VI_CTRL) & (VI_CTRL_SERRATE);
	bool interlaced = false;
	bool field = false;

    /* Check if the next buffer is ready to be displayed, otherwise just
       leave up the current frame */
    int next = buffer_next(now_showing);
    if (waiting_for_show_mask & (1 << next)) {
        now_showing = next;
        waiting_for_show_mask &= ~(1 << next);
    }

    //vi_write_dram_register(__safe_buffer[now_showing] + (interlaced && !field ? __width * __bitdepth : 0));
    REGISTER_WRITE(VID_NSTART, PhysicalAddr(__safe_buffer[now_showing]));
    REGISTER_WRITE(VID_NSIZE, surfaces[now_showing].length);

    // FIXME: PAL-M on old boards like NUS-CPU-02 requires changing V_BURST every field, otherwise
    // the image seems garbled at the top. It is probably a bug in old revisions of the VI chip,
    // since the problem doesn't exist on newer boards.
    /*if (get_tv_type() == TV_MPAL && interlaced) {
        if (field == 0) {
            *VI_V_BURST = 0x000b0202;
        } else {
            *VI_V_BURST = 0x000e0204;
        }
    }*/
}

//EMAC:
void display_disable()
{
    REGISTER_XOR_WRITE(VID_DMA_CNTL, VID_DMA_EN | VID_NV | VID_NVF);

	if(is_spot_box())
	{
        REGISTER_XOR_WRITE(VID_FCNTL, VID_OUT_EN);
	}
	else
	{
        REGISTER_XOR_WRITE(POT_CNTL, POT_OUT_EN);
	}
}

void display_enable()
{
    REGISTER_OR_WRITE(VID_DMA_CNTL, VID_DMA_EN | VID_NV | VID_NVF);

	if(is_spot_box())
	{
        REGISTER_OR_WRITE(VID_FCNTL, VID_OUT_EN);
	}
	else
	{
        REGISTER_OR_WRITE(POT_CNTL, POT_OUT_EN);
	}
}

void display_init(resolution_t res, pixel_mode_t format, uint32_t num_buffers, uint32_t border_color)
{
    /* Can't have the video interrupt happening here */
    disable_interrupts();

    /* Minimum is two buffers. */
    __buffers = MAX(1, MIN(NUM_BUFFERS, num_buffers));

    /* Calculate width and scale registers */
    //EMAC:assertf(res.width > 0, "nonpositive width");
    //EMAC:assertf(res.height > 0, "nonpositive height");
    //EMAC:assertf(res.width <= 800, "invalid width");
    //EMAC:assertf(res.height <= 720, "heights > 720 are buggy on hardware");

    uint32_t hstart = res.hstart;
    uint32_t vstart = res.vstart;
    
    if(res.pal)
    {
        if(hstart == -1)
        {
            hstart = (uint32_t)((MAX_PAL_WIDTH - res.width) / 2);
        }
        if(vstart == -1)
        {
            vstart = (uint32_t)((MAX_PAL_HEIGHT - res.height) / 2);
            
        }
    }
    else
    {
        if(hstart == -1)
        {
            hstart = (uint32_t)((MAX_NTSC_WIDTH - res.width) / 2);
        }
        if(vstart == -1)
        {
            vstart = (uint32_t)((MAX_NTSC_HEIGHT - res.height) / 2);
        }
    }

    hstart += HSTART_OFFSET;
    vstart += VSTART_OFFSET;

    
    if(is_spot_box())
	{
		REGISTER_WRITE(VID_HSIZE, res.width);
		REGISTER_WRITE(VID_HSTART, hstart);

		//REGISTER_WRITE(VID_VSIZE, res.height);
		REGISTER_WRITE(VID_VSIZE, res.height >> 1);
		REGISTER_WRITE(VID_VSTART, vstart);

		REGISTER_WRITE(VID_BLNKCOL, border_color);

        if(res.interlaced)
        {
            REGISTER_OR_WRITE(VID_FCNTL, VID_INTERLACE);
        }
	}
	else
	{
		REGISTER_WRITE(POT_HSIZE, res.width);
		REGISTER_WRITE(POT_HSTART, hstart);

		//REGISTER_WRITE(POT_VSIZE, res.height);
		REGISTER_WRITE(POT_VSIZE, res.height >> 1);
		REGISTER_WRITE(POT_VSTART, vstart);

		REGISTER_WRITE(POT_BLNKCOL, border_color);

        if(res.interlaced)
        {
            REGISTER_OR_WRITE(VID_DMA_CNTL, VID_INTERLACE_EN);
        }
	}

    /* Set up the display */
	__bitdepth = 2;
    __width = res.width;
    __height = res.height;
    __hstart = hstart;
    __vstart = vstart;

    surfaces = malloc(sizeof(surface_t) * __buffers);

    /* Initialize buffers and set parameters */
    for( int i = 0; i < __buffers; i++ )
    {
        /* Set parameters necessary for drawing */
        /* Grab a location to render to */
        surfaces[i] = surface_alloc(format, __width, __height);
        __safe_buffer[i] = surfaces[i].buffer;

        if(format == FMT_RGBA16)
        {
            surfaces[i].callback = surface_rgba16_to_yuv16;
        }
        else if(format == FMT_RGBA32)
        {
            surfaces[i].callback = surface_rgba32_to_yuv16;
        }

        //EMAC:assert(__safe_buffer[i] != NULL);

        /* Baseline is blank */
        surface_blank(&surfaces[i]);
    }

    /* Set the first buffer as the displaying buffer */
    now_showing = 0;
    locked_display_mask = 0;
    waiting_for_show_mask = 0;

    REGISTER_WRITE(VID_NSTART, PhysicalAddr(__safe_buffer[0]));
    REGISTER_WRITE(VID_NSIZE, surfaces[0].length);

    display_enable();

    /* Set which line to call back on in order to flip screens */
    register_VIDEO_VIDUNIT_handler(__display_callback);
    set_VIDEO_VIDUNIT_interrupt(1, VID_INT_DMA);

    enable_interrupts();
}

void display_close()
{
    display_disable();

    /* Can't have the video interrupt happening here */
    disable_interrupts();

    set_VIDEO_VIDUNIT_interrupt(0, VID_INT_DMA);
    unregister_VIDEO_POTUNIT_handler(__display_callback);

    now_showing = -1;
    locked_display_mask = 0;
    waiting_for_show_mask = 0;

    __width = 0;
    __height = 0;
    __hstart = 0;
    __vstart = 0;

    /* If display is active, wait for vblank before touching the registers */
    //if( vi_is_active() ) { vi_wait_for_vblank(); }

    //vi_set_blank_image();
    //vi_write_dram_register( 0 );

    if( surfaces )
    {
        for( int i = 0; i < __buffers; i++ )
        {
            /* Free framebuffer memory */
            surface_free(&surfaces[i]);
            __safe_buffer[i] = NULL;
        }
        free(surfaces);
        surfaces = NULL;
    }

    enable_interrupts();
}

surface_t* display_try_get()
{
    surface_t* retval = NULL;
    int next;

    /* Can't have the video interrupt happening here */
    disable_interrupts();

    /* Calculate index of next display context to draw on. We need
       to find the first buffer which is not being drawn upon nor
       being ready to be displayed.

       Notice that the loop is always executed once, so it also works
       in the case of a single display buffer, though it at least
       wait for that buffer to be shown. */
    next = buffer_next(now_showing);
    do {
        if (((locked_display_mask | waiting_for_show_mask) & (1 << next)) == 0 || true)  {
            retval = &surfaces[next];
            locked_display_mask |= 1 << next;
            break;
        }
        next = buffer_next(next);
    } while (next != now_showing);

    enable_interrupts();

    /* Possibility of returning nothing, or a valid display context */
    return retval;
}

surface_t* display_get()
{
    // Wait until a buffer is available. We use a RSP_WAIT_LOOP as
    // it is common for display to become ready again after RSP+RDP
    // have finished processing the previous frame's commands.
    surface_t* disp;

    while(true)
    {
        // Try grabbing an available display buffer (one that isn't locked or waiting to be shown on the display).
        disp = display_try_get();

        if (disp != NULL) {
            break;
        }

        // No display is available. Wait 8ms then try again.
        wait_ms(8);
    }

    return disp;
}

void display_render(surface_t* surf)
{
    /* They tried drawing on a bad context */
    if(surf == NULL) { return; }

    /* Can't have the video interrupt screwing this up */
    disable_interrupts();

    if(surf->callback != NULL)
    {
        //surf->callback(surf->buffer, surf->length);
    }

    /* Correct to ensure we are handling the right screen */
    int i = surf - surfaces;

    //EMAC:assertf(i >= 0 && i < __buffers, "Display context is not valid!");

    /* Check we have not unlocked this display already and is pending drawn. */
    //EMAC:assertf(!(waiting_for_show_mask & (1 << i)), "display_render called again on the same display %d (mask: %x)", i, waiting_for_show_mask);

    /* This should match, or something went awry */
    //EMAC:assertf(locked_display_mask & (1 << i), "display_render called on non-locked display %d (mask: %x)", i, locked_display_mask);

    // Unlock the display so it can be used elsewhere.
    locked_display_mask &= ~(1 << i);

    // Prevent the display from being written to before it's shown on the display.
    waiting_for_show_mask |= 1 << i;

    /* Record the time at which this frame was (asked to be) shown */
    uint32_t old_ticks = frame_times[frame_times_index];
    uint32_t now = TICKS_READ();
    if (old_ticks)
        frame_times_duration = TICKS_DISTANCE(old_ticks, now);
    frame_times[frame_times_index] = now;
    frame_times_index++;
    if (frame_times_index == FPS_WINDOW)
        frame_times_index = 0;

    enable_interrupts();
}

/**
 * @brief Force-display a previously locked buffer
 *
 * Display a valid display context to the screen right away, without waiting
 * for vblank interrupt. This function works also with interrupts disabled.
 *
 * NOTE: this is currently not part of the public API as we use it only
 * internally.
 *
 * @param[in] disp
 *            A display context retrieved using #display_get
 */
void display_render_force( display_context_t disp )
{
    /* Can't have the video interrupt screwing this up */
    disable_interrupts();
    display_render(disp);
    __display_callback();
    enable_interrupts();
}

uint32_t display_get_width(void)
{
    return __width;
}

uint32_t display_get_height(void)
{
    return __height;
}

uint32_t display_get_hstart(void)
{
    return __hstart;
}

uint32_t display_get_vstart(void)
{
    return __vstart;
}

uint32_t display_get_bitdepth(void)
{
    return __bitdepth;
}

uint32_t display_get_num_buffers(void)
{
    return __buffers;
}

float display_get_fps(void)
{
    if (!frame_times_duration) return 0;
    return (float)FPS_WINDOW * (TICKS_PER_US * 1000 * 1000) / frame_times_duration;
}