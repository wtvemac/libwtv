#ifndef __LIBWTV_DISPLAY_H
#define __LIBWTV_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "surface.h"

/**
 * @defgroup display Display Subsystem
 * @ingroup libdragon
 * @brief Video interface system for configuring video output modes and displaying rendered
 *        graphics.
 *
 * The display subsystem handles interfacing with the video interface (VI)
 * and the hardware rasterizer (RDP) to allow software and hardware graphics
 * operations.  It consists of the @ref display, the @ref graphics and the
 * @ref rdp modules.  A separate module, the @ref console, provides a rudimentary
 * console for developers.  Only the display subsystem or the console can be
 * used at the same time.  However, commands to draw console text to the display
 * subsystem are available.
 *
 * The display subsystem module is responsible for initializing the proper video
 * mode for displaying 2D, 3D and software graphics.  To set up video on the N64,
 * code should call #display_init with the appropriate options.  Once the display
 * has been set, a surface can be requested from the display subsystem using
 * #display_get.  To draw to the acquired surface, code should use functions
 * present in the @ref graphics and the @ref rdp modules.  Once drawing to a surface
 * is complete, the rendered graphic can be displayed to the screen using 
 * #display_show.  Once code has finished rendering all graphics, #display_close can 
 * be used to shut down the display subsystem.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NTSC_WIDTH       640
#define MAX_NTSC_HEIGHT      480

#define MAX_PAL_WIDTH        768
#define MAX_PAL_HEIGHT       560

///@cond
typedef struct surface_s surface_t;
///@endcond

/**
 * @addtogroup display
 * @{
 */

/**
 * @brief Video resolution structure
 *
 * You can either use one of the pre-defined constants
 * (such as #RESOLUTION_320x240) or define a custom resolution.
 */
typedef struct {
    /** @brief Screen hsize aka width  (must be between 2 and 800) */
    int32_t width;
    /** @brief Screen vsize aka height (must be between 1 and 720) */
    int32_t height;
    /** @brief True if interlaced mode enabled */
    bool interlaced;
    /** @brief True if interlaced mode enabled */
    bool pal;
    /** @brief Screen hstart (must be between 2 and 800) */
    int32_t hstart;
    /** @brief Screen vstart (must be between 1 and 720) */
    int32_t vstart;
} resolution_t;


///@cond
#define const static const /* fool doxygen to document these static members */
///@endcond
/** @brief 256x240 mode NTSC */
const resolution_t RESOLUTION_256x240 = {256, 240,  true, false, 311, 155};
/** @brief 320x240 mode NTSC */
const resolution_t RESOLUTION_320x240 = {320, 240,  true, false, 279, 155};
/** @brief 512x240 mode NTSC */
const resolution_t RESOLUTION_512x240 = {512, 240,  true, false, 183, 155};
/** @brief 560x420 mode NTSC */
const resolution_t RESOLUTION_560x420 = {560, 420,  true, false, 159,  65};
/** @brief 640x240 mode NTSC */
const resolution_t RESOLUTION_640x240 = {640, 240,  true, false, 119, 155};
/** @brief 512x480 mode NTSC */
const resolution_t RESOLUTION_512x480 = {512, 480,  true, false, 183,  35};
/** @brief 624x480 mode PAL */
const resolution_t RESOLUTION_624x480 = {624, 480,  true, true,  191,  75};
/** @brief 640x480 mode NTSC */
const resolution_t RESOLUTION_640x480 = {640, 480,  true, false, 119,  35};
/** @brief 768x560 mode PAL */
const resolution_t RESOLUTION_768x560 = {768, 560,  true, true,  119,  35};
#undef const

///@endcond

/** 
 * @brief Display context (DEPRECATED: Use #surface_t instead)
 * 
 * @see #surface_t
 */
typedef surface_t* display_context_t;

/**
 * @brief Initialize the display to a particular resolution and bit depth
 *
 * Initialize video system.  This sets up a double, triple, or multiple
 * buffered drawing surface which can be blitted or rendered to using
 * software or hardware.
 *
 * @param[in] res
 *            The requested resolution. Use either one of the pre-defined
 *            resolution (such as #RESOLUTION_320x240) or define a custom one.
 * @param[in] bit
 *            The requested bit depth (#DEPTH_16_BPP or #DEPTH_32_BPP)
 * @param[in] num_buffers
 *            Number of buffers, usually 2 or 3, but can be more. Triple buffering
 *            is recommended in case the application cannot hold a steady full framerate,
 *            so that slowdowns don't impact too much.
 * @param[in] gamma
 *            The requested gamma setting
 * @param[in] filters
 *            The requested display filtering options, see #filter_options_t
 */
void display_init(resolution_t res, pixel_mode_t format, uint32_t num_buffers, uint32_t border_color);

void display_init_direct(resolution_t res, pixel_mode_t format, uint32_t border_color);

/**
 * @brief Close the display
 *
 * Close a display and free buffer memory associated with it.
 */
void display_close();

void display_disable();
void display_enable();

/**
 * @brief Get a display buffer for rendering
 *
 * Grab a surface that is safe for drawing, spin-waiting until one is
 * available.
 * 
 * When you are done drawing on the buffer, use #display_show to schedule
 * the buffer to be displayed on the screen during next vblank.
 * 
 * It is possible to get more than a display buffer at the same time, for
 * instance to begin working on a new frame while the previous one is still
 * being rendered in parallel through RDP. It is important to notice that
 * surfaces will always be shown on the screen in the order they were gotten,
 * irrespective of the order #display_show is called.
 * 
 * @return A valid surface to render to.
 */
surface_t* display_get(void);

/**
 * @brief Try getting a display surface
 * 
 * This is similar to #display_get, but it does not block if no
 * display is available and return NULL instead.
 * 
 * @return A valid surface to render to or NULL if none is available.
 */
surface_t* display_try_get(void);

/**
 * @brief Display a buffer on the screen
 *
 * Display a surface to the screen on the next vblank. 
 * 
 * Notice that this function does not accept any arbitrary surface, but only
 * those returned by #display_get, which are owned by the display module.
 * 
 * @param[in] surf
 *            A surface to show (previously retrieved using #display_get)
 */
void display_render(surface_t* surf);


/**
 * @brief Clone of display_render to attempt some compatability with libdragon
 */
#define display_show display_render


/**
 * @brief Get the currently configured width of the display in pixels
 */
uint32_t display_get_width(void);

/**
 * @brief Get the currently configured height of the display in pixels
 */
uint32_t display_get_height(void);

/**
 * @brief Get the currently configured bitdepth of the display (in bytes per pixels)
 */
uint32_t display_get_bitdepth(void);

/**
 * @brief Get the currently configured number of buffers
 */
uint32_t display_get_num_buffers(void);

/**
 * @brief Get the current number of frames per second being rendered
 * 
 * @return float Frames per second
 */
float display_get_fps(void);


/** @cond */
__attribute__((deprecated("use display_get or display_try_get instead")))
static inline surface_t* display_lock(void) {
    return display_try_get();
}
/** @endcond */

/*void display_init(uint32_t build_base_address, uint32_t datas_base_address);

void display_disable();

void display_enable();*/

#ifdef __cplusplus
}
#endif

#endif
