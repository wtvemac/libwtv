
#ifndef __LIBWTV_HID_H
#define __LIBWTV_HID_H

#include "hid/event.h"

#ifdef __cplusplus
extern "C" {
#endif

void hid_init();
void hid_scan();
hid_state hid_keys_pressed();
hid_state hid_keys_held();
hid_state hid_keys_down();
hid_state hid_keys_up();

// libdragon calls

/**
 * @brief SI Nintendo 64 controller data
 * 
 * Data structure for Joybus response to `0x01` (Read N64 controller state) command.
 */
typedef struct SI_condat
{
    /** @brief Unused padding bits */
    unsigned : 16;
    /** @brief Status of the last command */
    unsigned err : 2;
    /** @brief Unused padding bits */
    unsigned : 14;

    union
    {
        struct
        {
            /** @brief 32-bit data sent to or returned from SI */
            unsigned int data : 32;
        };
        struct
        {
            /** @brief State of the A button */
            unsigned A : 1;
            /** @brief State of the B button */
            unsigned B : 1;
            /** @brief State of the Z button */
            unsigned Z : 1;
            /** @brief State of the start button */
            unsigned start : 1;
            /** @brief State of the up button */
            unsigned up : 1;
            /** @brief State of the down button */
            unsigned down : 1;
            /** @brief State of the left button */
            unsigned left : 1;
            /** @brief State of the right button */
            unsigned right : 1;
            /** @brief Unused padding bits */
            unsigned : 2;
            /** @brief State of the L button */
            unsigned L : 1;
            /** @brief State of the R button */
            unsigned R : 1;
            /** @brief State of the C up button */
            unsigned C_up : 1;
            /** @brief State of the C down button */
            unsigned C_down : 1;
            /** @brief State of the C left button */
            unsigned C_left : 1;
            /** @brief State of the C right button */
            unsigned C_right : 1;
            /** @brief State of the analog stick (X axis) */
            signed x : 8;
            /** @brief State of the analog stick (Y axis) */
            signed y : 8;
        };
    };
} _SI_condat;

/**
 * @brief SI controller data for all controller ports.
 * 
 * When reading N64 controller state, only the `c` member array will be populated.
 * When reading GC controller state, only the `gc` member array will be populated.
 */
typedef struct controller_data
{
    /** @brief Array of N64 controller state for each controller port. */
    struct SI_condat c[4];
} SI_controllers_state_t;

#define controller_init hid_init
#define controller_scan hid_scan

void controller_read(struct controller_data* output);
struct controller_data get_keys_pressed();
struct controller_data get_keys_held();
struct controller_data get_keys_down();
struct controller_data get_keys_up();
int get_dpad_direction(int controller);

int get_accessories_present(struct controller_data * data);
int get_controllers_present();

void rumble_start(int controller);
void rumble_stop(int controller);

#ifdef __cplusplus
}
#endif

#endif