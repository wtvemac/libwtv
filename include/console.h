
/**
 * @file console.h
 * @brief Console Support
 * @ingroup console
 */
#ifndef __LIBWTV_CONSOLE_H
#define __LIBWTV_CONSOLE_H

#include <stdbool.h>
#include "display.h"

/**
 * @defgroup console Console Support
 * @ingroup display
 * @brief Software console emulation for debugging and simple text output.
 *
 * Console support is provided as a poor-man's console for simple debugging on
 * the N64.  It does not respect common escape sequences and is nonstandard in
 * size.  When using the console, code should be careful to make sure that the
 * display system has not been initialized.  Similarly, if the display system
 * is needed, code should be sure that the console is not initialized.
 *
 * Code wishing to use the console should first initialize the console support in
 * libdragon with #console_init.  Once the console has been initialized, it wil
 * operate in one of two modes.  In automatic mode, every write to the console will
 * be immediately drawn on the display.  The console will be scrolled when the
 * buffer fills.  In manual mode, the console will only be displayed after calling
 * #console_render.  To set the render mode, use #console_set_render_mode.  To
 * add data to the console, use printf or iprintf.  To clear the console and reset
 * the scroll, use #console_clear.  Once the console is not needed or when the
 * code wishes to switch to the display subsystem, #console_clear should be called
 * to cleanly shut down the console support.
 *
 * @{
 */

/**
 * @name Console Render Modes
 * @brief Modes for rendering the console to the display
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


/** 
 * @brief Manual Rendering
 *
 * The user must call #console_render when the display should be updated
 */
#define RENDER_MANUAL            0

/** 
 * @brief Automatic Rendering
 *
 * The display will be updated on every console interaction
 */
#define RENDER_AUTOMATIC         1
/** @} */

/**
 * @name Console Dimensions
 * @{
 */

/**
 * @brief The console display resolution.
 */
//EMAC:
#define CONSOLE_RESOLUTION       RESOLUTION_560x420
//EMAC:
#define CONSOLE_BACKGROUND_COLOR 0x2080
//EMAC:
#define CONSOLE_FOREGROUND_COLOR 0xeb80

/**
 * @brief The console width in characters
 * @note Right padding is not enforced by code, adjust this to fit into
 * [horizontal resolution] - (HORIZONTAL_PADDING * 2)
 */
#define CONSOLE_WIDTH            68
/**
 * @brief The console height in characters
 * @note Bottom padding is not enforced by code, adjust this to fit into
 * [vertical resolution] - (VERTICAL_PADDING * 2)
 */
#define CONSOLE_HEIGHT           50

/**
 * @brief Padding from the left and right ends of the display in pixels
 *
 * @note Once the padding is subtracted from the display width, the character width should divide evenly into the remaining width.
 */
#define HORIZONTAL_PADDING       (CHARACTER_WIDTH * 1)               // 2 character widths per line (padding on both sides)

/**
 * @brief Padding from the top and bottom ends of the display in pixels
 *
 * @note Once the padding is subtracted from the display height, the character height should divide evenly into the remaining height.
 */
#define VERTICAL_PADDING         (CHARACTER_HEIGHT * 1.25)           // 3 character heights per display (padding on both sides)

/** 
 * @brief The maximum allowable X value that can be used to draw a character on the display.
 */
#define MAX_CONSOLE_X           (HORIZONTAL_PADDING + (CONSOLE_WIDTH * CHARACTER_WIDTH))

/** 
 * @brief The maximum allowable Y value that can be used to draw a character on the display.
 */
#define MAX_CONSOLE_Y            (VERTICAL_PADDING + (CONSOLE_HEIGHT * CHARACTER_HEIGHT))

/** @} */

/**
 * @name Character Dimensions
 * @{
 */

/** 
 * @brief The width of one character in pixels.
 */
#define CHARACTER_WIDTH     8

/** 
 * @brief The height of one character in pixels.
 */
#define CHARACTER_HEIGHT    8 

/** 
 * @brief Tab width in characters.
 *
 * @note This needs to divide evenly into #CONSOLE_WIDTH 
 */
#define TAB_WIDTH           (CONSOLE_WIDTH / 17)                // 17 tabs per line.

/** 
 * @brief Line feed height in characters.
 *
 * @note This needs to divide evenly into #CONSOLE_HEIGHT 
 */
#define LINE_FEED_HEIGHT    (CONSOLE_HEIGHT / 25)               // 25 lines per display.

/** @} */

//EMAC:
void console_alloc(int x, int y, int width, int height, int horizontal_padding, int vertical_padding, int tab_width, int line_feed_height, bool forced_render);
void console_enable();
void console_disable();
void console_empty_buffer();

/**
 * @brief Initialize the console
 *
 * Initialize the console system.  This will initialize the video properly, so
 * a call to the display_init() fuction is not necessary.
 */
void console_init();

/**
 * @brief Close the console
 *
 * Free the console system.  This will clean up any dynamic memory that was in
 * use.
 */
void console_close();

/**
 * @brief Send console output to debug channel
 *
 * Configure whether the console output should be redirected to the debug channel
 * as well (stderr), that can be sent over USB for development purposes. See
 * #debugf for more information.
 *
 * @param[in] debug
 *            True if console output should also be sent to the debugging channel, false otherwise
 *
 */
void console_set_debug(bool debug);

/**
 * @brief Set the console render mode
 *
 * This sets the render mode of the console.  The #RENDER_AUTOMATIC mode allows
 * console_printf to immediately be placed onto the display.  This is very similar
 * to a normal console on a unix/windows system.  The #RENDER_MANUAL mode allows
 * console_printf to be buffered, and displayed at a later date using
 * console_render().  This is to allow a rendering interface somewhat analogous
 * to curses
 *
 * @param[in] mode
 *            Render mode (#RENDER_AUTOMATIC or #RENDER_MANUAL)
 */
void console_set_render_mode(int mode);

/**
 * @brief Clear the console
 *
 * Clear the console and set the virtual cursor back to the top left.
 */
void console_clear();

/**
 * @brief Render the console
 *
 * Render the console to the display.  This should be called when in manual
 * rendering mode to display the console to the display.  In automatic mode
 * it is not necessary to call.
 *
 * The color that is used to draw the text can be set using #graphics_set_color.
 *
 * Do not call while interrupts are disabled, or it will lock the system.
 */
void console_render();

#ifdef __cplusplus
}
#endif

#endif
