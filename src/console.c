/**
 * @file console.c
 * @brief Console Support
 * @ingroup console
 */

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include "system.h"
#include "libwtv.h"

/* Prototypes */
static void __console_render(void);

/**
 * @brief Console properties structure
 */
typedef struct
{
    bool enabled;

	/** @brief The console buffer */
	char *console_buffer;
	int console_buffer_length;

    bool separate_display;
	/** 
	* @brief Internal state of the render mode
	* @see #RENDER_AUTOMATIC and #RENDER_MANUAL
	*/
	int render_mode;
    bool forced_render;

	int current_stored_buffer_index;
	int current_drawn_buffer_index;

	int current_character_buffer_x;
	int current_character_buffer_y;
	bool buffer_carriage_return_requested;

	int current_character_display_x;
	int current_character_display_y;
	bool display_carriage_return_requested;

    int x;
    int y;
    int width;
    int height;
	int min_console_x;
	int min_console_y;
	int max_console_x;
	int max_console_y;

	int tab_width;
	int line_feed_height;
	int available_lines;
} console_context_t;

static console_context_t console_context;

void console_set_render_mode(int mode)
{
    /* Allow manual buffering somewhat like curses */
    console_context.render_mode = mode;
}

/**
 * @brief Move the console display and buffer up by a specified number of lines
 */
static void __console_lline_trim(int lines)
{
    int line_count = 0;
    int cutoff_index = console_context.current_stored_buffer_index;

    for(; cutoff_index > 0; cutoff_index--)
    {
        // Count the number lines in the back buffer.
        if(console_context.console_buffer[cutoff_index] == '\x0a')
        {
            line_count++;
        }

        // When the number of lines equals one displayful minus the specified number of #lines to move up,
        // stop counting and use the current index to slice the console buffer from that point on.
        if(line_count >= (console_context.available_lines - (lines - 1)))
        {
            cutoff_index++;
            break;
        }
    }

    console_context.current_stored_buffer_index -= cutoff_index;

    // Shift the render buffer by the specified number of #lines
    cutoff_index *= sizeof(char);
    memmove(console_context.console_buffer, (console_context.console_buffer + cutoff_index), (console_context.console_buffer_length - cutoff_index));

    console_context.current_character_buffer_y -= (console_context.line_feed_height * CHARACTER_HEIGHT) * lines;

    // Set up parameters to redraw the entire display with the console moved up by the specified number of #lines
    // Clearing of the display happens outside this function.
    console_context.current_drawn_buffer_index = 0;
    console_context.current_character_display_x = console_context.min_console_x;
    console_context.current_character_display_y = console_context.min_console_y;
}

/**
 * @brief Move the console display and buffer back by a specified number of lines.
 */
static void __console_rline_trim(int lines)
{
    int line_count = 0;
    int cutoff_index = console_context.current_stored_buffer_index;

    for(; cutoff_index > 0; cutoff_index--)
    {
        // Count the number lines in the back buffer.
        if(console_context.console_buffer[cutoff_index] == '\x0a')
        {
            line_count++;
        }

        // When the number of lines equals one displayful minus the specified number of #lines to move up,
        // stop counting and use the current index to slice the console buffer from that point on.
        if(line_count >= lines)
        {
            cutoff_index++;
            break;
        }
    }

    console_context.current_stored_buffer_index = cutoff_index;

    // Set up parameters to start the drawing at the begining of the current line.
    // Clearing of the display happens outside this function.
    if(console_context.current_drawn_buffer_index > 0)
    {
        console_context.current_drawn_buffer_index = cutoff_index;
    }
    console_context.current_character_buffer_x = console_context.min_console_x;

    console_context.current_character_display_x = console_context.min_console_x;
    console_context.display_carriage_return_requested = true;
}

/**
 * @brief Newlib hook to allow printf/iprintf to appear on console
 *
 * @param[in] buf
 *            Pointer to data buffer containing the data to write
 * @param[in] len
 *            Length of data in bytes expected to be written
 *
 */
static void __console_write(char const character)
{
    if(!console_context.enabled) { return; }

    if(console_context.current_stored_buffer_index < console_context.console_buffer_length)
    {
        if(console_context.buffer_carriage_return_requested)
        {
            if(character != '\x0d')
            {
                if(character != '\x0a')
                {
                    __console_rline_trim(1);
                }

                console_context.buffer_carriage_return_requested = false;
            }
        }

        switch(character)
        {
            case '\x0a':
                console_context.current_character_buffer_x = console_context.min_console_x;
                console_context.current_character_buffer_y += (console_context.line_feed_height * CHARACTER_HEIGHT);

                console_context.console_buffer[console_context.current_stored_buffer_index++] = character;

                if(console_context.current_character_buffer_y >= console_context.max_console_y)
                {
                    //EMAC: else beyond the screen height.
                    __console_lline_trim(1);
                }
                break;
            case '\x0d':
                console_context.buffer_carriage_return_requested = true;
                break;
            default:
                if(character > 0x00 && console_context.current_character_buffer_x < console_context.max_console_x)
                {
                    if(character == '\t')
                    {
                        console_context.current_character_buffer_x += (console_context.tab_width * CHARACTER_WIDTH);
                    }
                    else
                    {
                        console_context.current_character_buffer_x += CHARACTER_WIDTH;
                    }

                    console_context.console_buffer[console_context.current_stored_buffer_index++] = character;
                }//EMAC: else beyond the screen width.
                break;
        }

        /* Out to display! */
        if(console_context.render_mode == RENDER_AUTOMATIC)
        {
            __console_render();
        }
    }
}

void console_alloc(int x, int y, int width, int height, int horizontal_padding, int vertical_padding, int tab_width, int line_feed_height, bool forced_render)
{
    int min_console_x = (x + horizontal_padding);
    int min_console_y = (y + vertical_padding);

    int console_width = ((width - (horizontal_padding * 2)) / CHARACTER_WIDTH);
    int console_height = ((height - (vertical_padding * 2)) / CHARACTER_HEIGHT);

    console_context = (console_context_t) {
        .enabled = true,

        .console_buffer = NULL,
        .console_buffer_length = ((sizeof(char) * console_width * console_height) + sizeof(char)),

        .separate_display = true,
        .render_mode = RENDER_AUTOMATIC,
        .forced_render = forced_render,
        
        .current_stored_buffer_index = 0,
        .current_drawn_buffer_index = 0,
        .current_character_buffer_x = min_console_x,
        .current_character_buffer_y = min_console_y,

        .buffer_carriage_return_requested = false,
        .current_character_display_x = min_console_x,
        .current_character_display_y = min_console_y,
        .display_carriage_return_requested = false,

        .x = x,
        .y = y,
        .width = width,
        .height = height,

        .min_console_x = min_console_x,
        .min_console_y = min_console_y,
        .max_console_x = (x + (width - horizontal_padding)),
        .max_console_y = (y + (height - vertical_padding)),

        .tab_width = tab_width,
        .line_feed_height = line_feed_height,
        .available_lines = (console_height / line_feed_height)
    };

    graphics_set_color(CONSOLE_FOREGROUND_COLOR, CONSOLE_BACKGROUND_COLOR);

    graphics_set_default_font();
    
    console_context.console_buffer = malloc(console_context.console_buffer_length);

    register_putchar_callback(__console_write);

    console_clear();
}

void console_init()
{
    /* In case they initialized the display already */
    display_close();

    display_init(CONSOLE_RESOLUTION, FMT_YUV16, 1, 0x0);

    console_alloc(0, 0, (MAX_CONSOLE_X + HORIZONTAL_PADDING), (MAX_CONSOLE_Y + VERTICAL_PADDING), HORIZONTAL_PADDING, VERTICAL_PADDING, TAB_WIDTH, LINE_FEED_HEIGHT, true);

    console_context.separate_display = false;
}

void console_close()
{
    unregister_putchar_callback(__console_write);

    if(!console_context.separate_display)
    {
        display_close();
    }

    if(console_context.console_buffer)
    {
        /* Nuke the console buffer */
        free(console_context.console_buffer);
        console_context.console_buffer = NULL;
    }
}

void console_clear()
{
    if(!console_context.console_buffer) { return; }

    /* Force fflush not to draw regardless */
    int render = console_context.render_mode;
    console_context.render_mode = RENDER_MANUAL;

    /* Return to original */
    console_context.render_mode = render;

    /* Remove all data */
    console_empty_buffer();

    /* Should we display? */
    if(console_context.render_mode == RENDER_AUTOMATIC)
    {
        __console_render();
    }
}

void console_empty_buffer()
{
    memset(console_context.console_buffer, 0, console_context.console_buffer_length);

    console_context.current_stored_buffer_index = 0;
    console_context.current_drawn_buffer_index = 0;
    console_context.current_character_display_x = console_context.min_console_x;
    console_context.current_character_display_y = console_context.min_console_y;
    console_context.buffer_carriage_return_requested = false;

    console_context.current_character_display_x = console_context.min_console_x;
    console_context.current_character_display_y = console_context.min_console_y;
    console_context.display_carriage_return_requested = false;
}

void console_enable()
{
    console_context.enabled = true;
    
    if(console_context.render_mode == RENDER_MANUAL)
    {
        __console_render();
    }
}

void console_disable()
{
    console_context.enabled = false;
}

/**
 * @brief Helper function to render the console
 */
static void __console_render()
{
    if(!console_context.enabled || !console_context.console_buffer) { return; }

    /* Wait until we get a valid context */
    surface_t *dc = display_get();

    // Always re-render entire screen if using more than one display buffer or render mode is manual.
    if(display_get_num_buffers() > 1 || console_context.render_mode == RENDER_MANUAL)
    {
        console_context.current_drawn_buffer_index = 0;
        console_context.current_character_display_x = console_context.min_console_x;
        console_context.current_character_display_y = console_context.min_console_y;
    }

    if(console_context.current_drawn_buffer_index == 0)
    {
        graphics_draw_box(dc, console_context.x, console_context.y, console_context.width, console_context.height, graphics_get_background_color());
    }

    while(console_context.current_stored_buffer_index > console_context.current_drawn_buffer_index && console_context.current_character_display_y <= console_context.max_console_y)
    {
        char character = console_context.console_buffer[console_context.current_drawn_buffer_index];

        if(console_context.display_carriage_return_requested)
        {
            graphics_draw_box(dc, 0, console_context.current_character_display_y, console_context.max_console_x, CHARACTER_HEIGHT, graphics_get_background_color());
            console_context.display_carriage_return_requested = false;
        }

        /* Draw to the display using the forecolor and backcolor set in the graphics subsystem */
        switch(character)
        {
            case '\x0a':
                console_context.current_character_display_x = console_context.min_console_x;
                console_context.current_character_display_y += (console_context.line_feed_height * CHARACTER_HEIGHT);
                break;
            case '\x0d':
                break;
            default:
                if(character > 0x00 && console_context.current_character_display_x < console_context.max_console_x)
                {
                    if(character == '\t')
                    {
                        console_context.current_character_display_x += (console_context.tab_width * CHARACTER_WIDTH);
                    }
                    else
                    {
                        graphics_draw_character(dc, console_context.current_character_display_x, console_context.current_character_display_y, character);
                        console_context.current_character_display_x += CHARACTER_WIDTH;
                    }
                }
                break;
        }

        console_context.current_drawn_buffer_index++;
    }

    /* If the interrupts are disabled, the console wouldn't show to the display.
     * Since the console is only used for development and emergency context,
     * it is better to force display irrespective of vblank. */
    uint32_t c0_status = C0_STATUS();
    if (console_context.forced_render || (c0_status & C0_STATUS_IE) == 0 || ((c0_status & (C0_STATUS_EXL | C0_STATUS_ERL)) != 0))
    {
        extern void display_render_force(display_context_t dc);
        display_render_force(dc);
    }
    else
    {
        display_render(dc);
    }
}

void console_render()
{
    /* Render now */
    __console_render();
}

void console_set_debug(bool debug)
{
    set_serial_output(debug);
}
