#ifndef __LIBWTV_INTERRUPT_H
#define __LIBWTV_INTERRUPT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup interrupt Interrupt Controller
 * @ingroup lowlevel
 * @brief N64 interrupt registering and servicing routines.
 *
 * The N64 interrupt controller provides a software interface to
 * register for interrupts from the various systems in the N64.
 * Most interrupts on the N64 coordinate through the MIPS interface
 * (MI) to allow interrupts to be handled at one spot.  A notable
 * exception is the timer interrupt which is generated by the MIPS
 * r4300 itself and not the N64 hardware.
 *
 * The interrupt controller is automatically initialized before
 * main is called. By default, all interrupts are enabled and any
 * registered callback can be called when an interrupt occurs.
 * Each of the N64-generated interrupts is maskable using the various
 * set accessors.
 *
 * Interrupts can be enabled or disabled as a whole on the N64 using
 * #enable_interrupts and #disable_interrupts.  It is assumed that
 * once the interrupt system is activated, these will always be called
 * in pairs.  Calling #enable_interrupts without first calling
 * #disable_interrupts is considered a violation of this assumption
 * and should be avoided.  Calling #disable_interrupts when interrupts
 * are already disabled will have no effect interrupts-wise
 * (but should be paired with a #enable_interrupts regardless),
 * and in that case the paired #enable_interrupts will not enable
 * interrupts either.
 * In this manner, it is safe to nest calls to disable and enable
 * interrupts.
 *
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief State of interrupts on the system
 */
typedef enum
{
    /** @brief Interrupt controller has not been initialized */
    INTERRUPTS_UNINITIALIZED,
    /** @brief Interrupts are currently disabled */
    INTERRUPTS_DISABLED,
    /** @brief Interrupts are currently enabled */
    INTERRUPTS_ENABLED
} interrupt_state_t;

/** @} */

void register_RESET_handler( void (*callback)() );
void set_RESET_interrupt(bool active);
void unregister_RESET_handler(void (*callback)());

void register_TI_handler(void (*callback)());
void set_TI_interrupt(bool active);
void unregister_TI_handler( void (*callback)() );

void register_SOLO_TI_handler(void (*callback)());
void set_SOLO_TI_interrupt(bool active);
void unregister_SOLO_TI_handler( void (*callback)() );

void register_VIDEO_DIVUNIT_handler(void (*callback)());
void set_VIDEO_DIVUNIT_interrupt(bool active, uint32_t status);
void unregister_VIDEO_DIVUNIT_handler( void (*callback)() );

void register_VIDEO_GFXUNIT_handler(void (*callback)());
void set_VIDEO_GFXUNIT_interrupt(bool active, uint32_t status);
void unregister_VIDEO_GFXUNIT_handler( void (*callback)() );

void register_VIDEO_POTUNIT_handler(void (*callback)());
void set_VIDEO_POTUNIT_interrupt(bool active, uint32_t status);
void unregister_VIDEO_POTUNIT_handler( void (*callback)() );

void register_VIDEO_VIDUNIT_handler(void (*callback)());
void set_VIDEO_VIDUNIT_interrupt(bool active, uint32_t status);
void unregister_VIDEO_VIDUNIT_handler( void (*callback)() );

void register_AUDIO_SMODEM_IN_handler(void (*callback)());
void set_AUDIO_SMODEM_IN_interrupt(bool active);
void unregister_AUDIO_SMODEM_IN_handler( void (*callback)() );

void register_AUDIO_SMODEM_OUT_handler(void (*callback)());
void set_AUDIO_SMODEM_OUT_interrupt(bool active);
void unregister_AUDIO_SMODEM_OUT_handler( void (*callback)() );

void register_AUDIO_DIVUNIT_handler(void (*callback)());
void set_AUDIO_DIVUNIT_interrupt(bool active);
void unregister_AUDIO_DIVUNIT_handler( void (*callback)() );

void register_AUDIO_IN_handler(void (*callback)());
void set_AUDIO_IN_interrupt(bool active);
void unregister_AUDIO_IN_handler( void (*callback)() );

void register_AUDIO_OUT_handler(void (*callback)());
void set_AUDIO_OUT_interrupt(bool active);
void unregister_AUDIO_OUT_handler( void (*callback)() );

void register_RIO_SLOT0_handler(void (*callback)());
void set_RIO_SLOT0_interrupt(bool active);
void unregister_RIO_SLOT0_handler( void (*callback)() );

void register_RIO_SLOT1_handler(void (*callback)());
void set_RIO_SLOT1_interrupt(bool active);
void unregister_RIO_SLOT1_handler( void (*callback)() );

void register_RIO_SLOT2_handler(void (*callback)());
void set_RIO_SLOT2_interrupt(bool active);
void unregister_RIO_SLOT2_handler( void (*callback)() );

void register_RIO_SLOT3_handler(void (*callback)());
void set_RIO_SLOT3_interrupt(bool active);
void unregister_RIO_SLOT3_handler( void (*callback)() );

void register_DEV_GPIO_handler(void (*callback)());
void set_DEV_GPIO_interrupt(bool active);
void unregister_DEV_GPIO_handler( void (*callback)() );

void register_DEV_UART_handler(void (*callback)());
void set_DEV_UART_interrupt(bool active);
void unregister_DEV_UART_handler( void (*callback)() );

void register_DEV_SMARTCARD_handler(void (*callback)());
void set_DEV_SMARTCARD_interrupt(bool active);
void unregister_DEV_SMARTCARD_handler( void (*callback)() );

void register_DEV_PPORT_handler(void (*callback)());
void set_DEV_PPORT_interrupt(bool active);
void unregister_DEV_PPORT_handler( void (*callback)() );

void register_DEV_IR_OUT_handler(void (*callback)());
void set_DEV_IR_OUT_interrupt(bool active);
void unregister_DEV_IR_OUT_handler( void (*callback)() );

void register_DEV_IR_IN_handler(void (*callback)());
void set_DEV_IR_IN_interrupt(bool active);
void unregister_DEV_IR_IN_handler( void (*callback)() );

void register_DEV_PS2_handler(void (*callback)());
void set_DEV_PS2_interrupt(bool active);
void unregister_DEV_PS2_handler( void (*callback)() );

/** 
 * @brief Guaranteed length of the reset time.
 * 
 * This is the guaranteed length of the reset time, that is the time
 * that goes between the user pressing the reset button, and the CPU actually
 * resetting. See #exception_reset_time for more details.
 * 
 * @note The general knowledge about this is that the reset time should be
 *       500 ms. Testing on different consoles show that, while most seem to
 *       reset after 500 ms, a few EU models reset after 200ms. So we define
 *       the timer shorter for greater compatibility.
 */
#define RESET_TIME_LENGTH      TICKS_FROM_MS(200)


/** 
 * @brief Check whether the RESET button was pressed and how long we are into
 *        the reset process.
 * 
 * This function returns how many ticks have elapsed since the user has pressed
 * the RESET button, or 0 if the user has not pressed it.
 * 
 * It can be used by user code to perform actions during the RESET
 * process (see #register_RESET_handler). It is also possible to simply
 * poll this value to check at any time if the button has been pressed or not.
 * 
 * The reset process takes about 500ms between the user pressing the
 * RESET button and the CPU being actually reset, though on some consoles
 * it seems to be much less. See #RESET_TIME_LENGTH for more information.
 * For the broadest compatibility, please use #RESET_TIME_LENGTH to implement
 * the reset logic.
 * 
 * Notice also that the reset process is initiated when the user presses the
 * button, but the reset will not happen until the user releases the button.
 * So keeping the button pressed is a good way to check if the application
 * actually winds down correctly.
 * 
 * @return Ticks elapsed since RESET button was pressed, or 0 if the RESET button
 *         was not pressed.
 * 
 * @see register_RESET_handler
 * @see #RESET_TIME_LENGTH
 */
uint32_t exception_reset_time( void );

static inline __attribute__((deprecated("calling init_interrupts no longer required")))
void init_interrupts() {}

static inline __attribute__((deprecated("use register_RESET_handler instead")))
void register_reset_handler( void (*callback)() )
{
    register_RESET_handler(callback);
}


/**
 * @brief Enable interrupts systemwide
 *
 * @note If this is called inside a nested disable call, it will have no effect on the
 *       system.  Therefore it is safe to nest disable/enable calls.  After the least
 *       nested enable call, systemwide interrupts will be reenabled.
 */
void enable_interrupts();

/**
 * @brief Disable interrupts systemwide
 *
 * @note If interrupts are already disabled on the system or interrupts have not
 *       been initialized, this function will not modify the system state.
 */
void disable_interrupts();


/**
 * @brief Return the current state of interrupts
 *
 * @retval INTERRUPTS_UNINITIALIZED if the interrupt system has not been initialized yet.
 * @retval INTERRUPTS_DISABLED if interrupts have been disabled.
 * @retval INTERRUPTS_ENABLED if interrupts are currently enabled.
 */
interrupt_state_t get_interrupts_state(); 

uint32_t exception_reset_time( void );

#ifdef __cplusplus
}
#endif

#endif
