/**
 * @file interrupt.c
 * @brief Interrupt Controller
 * @ingroup interrupt
 */

#include "interrupt.h"
#include "libc.h"
#include "cop0.h"
#include "audio.h"
#include "regs_internal.h"
#include "wtvsys.h"
#include "libwtv.h"

/** @brief Number of nested disable interrupt calls
 *
 * This will represent the number of disable interrupt calls made on the system.
 * If this is set to 0, interrupts are enabled.  A number higher than 0 represents
 * that many disable calls that were nested, and consequently the number of
 * interrupt enable calls that need to be made to re-enable interrupts.  A negative
 * number means that the interrupt system hasn't been initialized yet.
 */
static int __interrupt_depth = -1;

/** @brief Value of the status register at the moment interrupts
 *         got disabled.
 */
static int __interrupt_sr = 0;

/**
 * @brief Structure of an interrupt callback
 */
typedef struct callback_link
{
    /** @brief Callback function */
    void (*callback)();
    /** @brief Pointer to next callback */
    struct callback_link * next;
} _callback_link;

struct callback_link *TI_callback = 0;
struct callback_link *SOLO_TI_callback = 0;
struct callback_link *VIDEO_DIVUNI_callback = 0;
struct callback_link *VIDEO_GFXUNIT_callback = 0;
struct callback_link *VIDEO_POTUNIT_callback = 0;
struct callback_link *VIDEO_VIDUNIT_callback = 0;
struct callback_link *AUDIO_SMODEM_IN_callback = 0;
struct callback_link *AUDIO_SMODEM_OUT_callback = 0;
struct callback_link *AUDIO_DIVUNIT_callback = 0;
struct callback_link *AUDIO_IN_callback = 0;
struct callback_link *AUDIO_OUT_callback = 0;
struct callback_link *RIO_SLOT0_callback = 0;
struct callback_link *RIO_SLOT1_callback = 0;
struct callback_link *RIO_SLOT2_callback = 0;
struct callback_link *RIO_SLOT3_callback = 0;
struct callback_link *DEV_GPIO_callback = 0;
struct callback_link *DEV_UART_callback = 0;
struct callback_link *DEV_SMARTCARD_callback = 0;
struct callback_link *DEV_PPORT_callback = 0;
struct callback_link *DEV_IR_OUT_callback = 0;
struct callback_link *DEV_IR_IN_callback = 0;
struct callback_link *DEV_PS2_callback = 0;

/** @brief Maximum number of reset handlers that can be registered. */
#define MAX_RESET_HANDLERS 4

/** @brief Pre-NMI exception handlers */
static void (*__prenmi_handlers[MAX_RESET_HANDLERS])(void);
/** @brief Tick at which the pre-NMI was triggered */
static uint32_t __prenmi_tick;

/**
 * @brief Initialize the interrupt controller
 */
__attribute__((constructor)) void __init_interrupts()
{
    /* Make sure that we aren't initializing interrupts when they are already enabled */
    if( __interrupt_depth < 0 )
    {
        /* Set that we are enabled */
        __interrupt_depth = 0;

        /* Clear and mask all interrupts on the system so we start with a clean slate */
		if(is_spot_box())
		{
			REGISTER_WRITE(BUS_INT_EN_CLEAR, 0xffffffff);
		}
		else
		{
			REGISTER_WRITE(BUS_INT_EN_CLEAR, 0xffffffff);
			REGISTER_WRITE(POT_INT_EN_CLEAR, 0xffffffff);
			REGISTER_WRITE(POT_INT_EN_CLEAR, 0xffffffff);
			REGISTER_WRITE(BUS_INT_VIDEO_EN_CLEAR, 0xffffffff);
			REGISTER_WRITE(BUS_INT_VIDEO_EN_CLEAR, 0xffffffff);
			REGISTER_WRITE(BUS_INT_AUDIO_EN_CLEAR, 0xffffffff);
			REGISTER_WRITE(BUS_INT_RIO_EN_CLEAR, 0xffffffff);
		}
		
		/* Enable interrupts systemwide. We set the global interrupt enable,
           and then specifically enable RCP interrupts. */
        C0_WRITE_STATUS(C0_STATUS() | C0_STATUS_IE);
    }
}

void disable_interrupts()
{
    /* Don't do anything if we haven't initialized */
    if( __interrupt_depth < 0 ) { return; }

    if( __interrupt_depth == 0 )
    {
        /* We must disable the interrupts now. */
        uint32_t sr = C0_STATUS();
        C0_WRITE_STATUS(sr & ~C0_STATUS_IE);

        /* Save the original SR value away, so that we know if
           interrupts were enabled and whether to restore them.
           NOTE: this memory write must happen now that interrupts
           are disabled, otherwise it could cause a race condition
           because an interrupt could trigger and overwrite it.
           So put an explicit barrier. */
        MEMORY_BARRIER();
        __interrupt_sr = sr;
    }

    /* Ensure that we remember nesting levels */
    __interrupt_depth++;
}

void enable_interrupts()
{
    /* Don't do anything if we aren't initialized */
    if( __interrupt_depth < 0 ) { return; }

    /* Check that we're not calling enable_interrupts() more than expected */
    //EMAC:assertf(__interrupt_depth > 0, "unbalanced enable_interrupts() call");

    /* Decrement the nesting level now that we are enabling interrupts */
    __interrupt_depth--;

    if( __interrupt_depth == 0 )
    {
        /* Restore the interrupt state that was active when interrupts got
           disabled.
           This is important to be done this way, as opposed to simply or-ing
           in the IE bit (| C0_STATUS_IE), because, within an interrupt handler,
           we don't want interrupts enabled, or we would allow reentrant
           interrupts which are not supported. */
        C0_WRITE_STATUS(C0_STATUS() | (__interrupt_sr & C0_STATUS_IE));
    }
}

interrupt_state_t get_interrupts_state()
{
    if( __interrupt_depth < 0 )
    {
        return INTERRUPTS_UNINITIALIZED;
    }
    else if( __interrupt_depth == 0 )
    {
        return INTERRUPTS_ENABLED;
    }
    else
    {
        return INTERRUPTS_DISABLED;
    }
}

uint32_t exception_reset_time( void )
{
	if (!__prenmi_tick) return 0;
	return TICKS_SINCE(__prenmi_tick);
}


/**
 * @brief Add a callback to a linked list of callbacks
 *
 * @param[in,out] head
 *                Pointer to the head of a linked list to add to
 * @param[in]     callback
 *                Function to call when executing callbacks in this linked list
 */
static void __register_callback( struct callback_link ** head, void (*callback)() )
{
    if( head )
    {
        /* Add to beginning of linked list */
        struct callback_link *next = *head;
        (*head) = malloc(sizeof(struct callback_link));

        if( *head )
        {
            (*head)->next=next;
            (*head)->callback=callback;
        }
    }
}

/** 
 * @brief Call each callback in a linked list of callbacks
 *
 * @param[in] head
 *            Pointer to the head of a callback linke list
 */
static void __call_callback( struct callback_link * head )
{
    /* Call each registered callback */
    while( head )
    {
        if( head->callback )
        {
        	head->callback();
        }

        /* Go to next */
	    head=head->next;
    }
}

/**
 * @brief Remove a callback from a linked list of callbacks
 *
 * @param[in,out] head
 *                Pointer to the head of a linked list to remove from
 * @param[in]     callback
 *                Function to search for and remove from callback list
 */
static void __unregister_callback( struct callback_link ** head, void (*callback)() )
{
    if(head)
    {
        /* Try to find callback this matches */
        struct callback_link *last = 0;
        struct callback_link *cur  = *head;

        while( cur )
        {
            if( cur->callback == callback )
            {
                /* We found it!  Try to remove it from the list */
                if( last )
                {
                    /* This is somewhere in the linked list */
                    last->next = cur->next;
                }
                else
                {
                    /* This is the first node */
                    *head = cur->next;
                }

                /* Free memory */
                free( cur );

                /* Exit early */
                break;
            }

            /* Go to next entry */
            last = cur;
            cur = cur->next;
        }
    }
}

void register_RESET_handler( void (*callback)() )
{
	for (int i=0;i<MAX_RESET_HANDLERS;i++)
	{		
		if (!__prenmi_handlers[i])
		{
			__prenmi_handlers[i] = callback;
			return;
		}
	}
	//EMAC:assertf(0, "Too many pre-NMI handlers\x0a\x0d");
}
void set_RESET_interrupt(bool active)
{
    if(active)
    {
        C0_WRITE_STATUS(C0_STATUS() | C0_INTERRUPT_PRENMI);
    }
    else
    {
        C0_WRITE_STATUS(C0_STATUS() & ~C0_INTERRUPT_PRENMI);
    }
}
void __RESET_handler()
{
	serial_put_string("__RESET_handler\x0a\x0d");

	/* This function will be called many times because there is no way
	   to acknowledge the pre-NMI interrupt. So make sure it does nothing
	   after the first call. */
	if (__prenmi_tick) return;

	/* Store the tick at which we saw the exception. Make sure
	 * we never store 0 as we use that for "no reset happened". */
	__prenmi_tick = TICKS_READ() | 1;

	/* Call the registered handlers. */
	for (int i=0;i<MAX_RESET_HANDLERS;i++)
	{
		if (__prenmi_handlers[i])
			__prenmi_handlers[i]();
	}
}
void unregister_RESET_handler(void (*callback)())
{
    for (int i=0;i<MAX_RESET_HANDLERS;i++)
    {		
        if (__prenmi_handlers[i] == callback)
        {
            __prenmi_handlers[i] = NULL;
            return;
        }
    }
    //EMAC:assertf(0, "Reset handler not found\x0a\x0d");
}


void register_TI_handler(void (*callback)())
{
	__register_callback(&TI_callback, callback);
}
void set_TI_interrupt(bool active)
{
    if(active)
    {
        C0_WRITE_STATUS(C0_STATUS() | C0_INTERRUPT_TIMER);
    }
    else
    {
        C0_WRITE_STATUS(C0_STATUS() & ~C0_INTERRUPT_TIMER);
    }
}
void __TI_handler()
{
	serial_put_string("__TI_handler\x0a\x0d");

	__call_callback(TI_callback);
}
void unregister_TI_handler( void (*callback)() )
{
	__unregister_callback(&TI_callback, callback);
}


void register_SOLO_TI_handler(void (*callback)())
{
	__register_callback(&SOLO_TI_callback, callback);
}
void set_SOLO_TI_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_TIMER_EN_SET,  BUS_INT_TIMER_SYSTEM | BUS_INT_TIMER_TIMEOUT);
		REGISTER_WRITE(BUS_INT_EN_SET,        BUS_INT_TIMER);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_TIMER_EN_CLEAR, BUS_INT_TIMER_SYSTEM | BUS_INT_TIMER_TIMEOUT);
		REGISTER_WRITE(BUS_INT_EN_CLEAR,       BUS_INT_TIMER);
	}
}
void __SOLO_TI_handler()
{
	serial_put_string("__SOLO_TI_handler\x0a\x0d");
	
	__call_callback(SOLO_TI_callback);
}
void unregister_SOLO_TI_handler( void (*callback)() )
{
	__unregister_callback(&SOLO_TI_callback, callback);
}


void register_VIDEO_DIVUNIT_handler(void (*callback)())
{
	__register_callback(&VIDEO_DIVUNI_callback, callback);
}
void set_VIDEO_DIVUNIT_interrupt(bool active, uint32_t status)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_VIDEO_EN_SET,   BUS_INT_VIDEO_DIVUNIT);
		REGISTER_WRITE(BUS_INT_EN_SET,         BUS_INT_VIDEO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_VIDEO_EN_CLEAR, BUS_INT_VIDEO_DIVUNIT);
	}
}
void __VIDEO_DIVUNIT_handler()
{
	serial_put_string("__VIDEO_DIVUNIT_handler\x0a\x0d");

	__call_callback(VIDEO_DIVUNI_callback);
}
void unregister_VIDEO_DIVUNIT_handler( void (*callback)() )
{
	__unregister_callback(&VIDEO_DIVUNI_callback, callback);
}

void register_VIDEO_GFXUNIT_handler(void (*callback)())
{
	__register_callback(&VIDEO_GFXUNIT_callback, callback);
}
void set_VIDEO_GFXUNIT_interrupt(bool active, uint32_t status)
{
	if(active)
	{
		REGISTER_WRITE(GFX_INT_EN_SET,       status);
		REGISTER_WRITE(BUS_INT_VIDEO_EN_SET, BUS_INT_VIDEO_GFXUNIT);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_VIDEO);
	}
	else
	{
		REGISTER_WRITE(GFX_INT_EN_CLEAR,     status);
	}
}
void __VIDEO_GFXUNIT_handler(uint32_t status)
{
	serial_put_string("__VIDEO_GFXUNIT_handler\x0a\x0d");
	
	__call_callback(VIDEO_GFXUNIT_callback);
}
void unregister_VIDEO_GFXUNIT_handler( void (*callback)() )
{
	__unregister_callback(&VIDEO_GFXUNIT_callback, callback);
}

void register_VIDEO_POTUNIT_handler(void (*callback)())
{
	__register_callback(&VIDEO_POTUNIT_callback, callback);
}
void set_VIDEO_POTUNIT_interrupt(bool active, uint32_t status)
{
	if(active)
	{
		REGISTER_WRITE(POT_INT_EN_SET,       status);
		REGISTER_WRITE(BUS_INT_VIDEO_EN_SET, BUS_INT_VIDEO_POTUNIT);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_VIDEO);
	}
	else
	{
		REGISTER_WRITE(POT_INT_EN_CLEAR,    status);
	}
}
void __VIDEO_POTUNIT_handler(uint32_t status)
{
	serial_put_string("__VIDEO_POTUNIT_handler\x0a\x0d");
	
	__call_callback(VIDEO_POTUNIT_callback);
}
void unregister_VIDEO_POTUNIT_handler( void (*callback)() )
{
	__unregister_callback(&VIDEO_POTUNIT_callback, callback);
}

void register_VIDEO_VIDUNIT_handler(void (*callback)())
{
	__register_callback(&VIDEO_VIDUNIT_callback, callback);
}
void set_VIDEO_VIDUNIT_interrupt(bool active, uint32_t status)
{
	if(is_spot_box())
	{
		if(active)
		{
			REGISTER_WRITE(VID_INT_EN_SET,       status);
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_SPOT_VIDEO);
		}
		else
		{
			REGISTER_WRITE(VID_INT_EN_CLEAR,     status);
		}
	}
	else
	{
		if(active)
		{
			REGISTER_WRITE(VID_INT_EN_SET,       status);
			REGISTER_WRITE(BUS_INT_VIDEO_EN_SET, BUS_INT_VIDEO_VIDUNIT);
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_VIDEO);
			
		}
		else
		{
			REGISTER_WRITE(VID_INT_EN_CLEAR,      status);
		}
	}
}
void __VIDEO_VIDUNIT_handler(uint32_t status) // SPOT
{
	//serial_put_string("__VIDEO_VIDUNIT_handler\x0a\x0d");
	
	__call_callback(VIDEO_VIDUNIT_callback);
}
void unregister_VIDEO_VIDUNIT_handler( void (*callback)() )
{
	__unregister_callback(&VIDEO_VIDUNIT_callback, callback);
}


void register_AUDIO_SMODEM_IN_handler(void (*callback)())
{
	__register_callback(&AUDIO_SMODEM_IN_callback, callback);
}
void set_AUDIO_SMODEM_IN_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_SET,   BUS_INT_AUDIO_SMODEM_IN);
		REGISTER_WRITE(BUS_INT_EN_SET,         BUS_INT_AUDIO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_CLEAR, BUS_INT_AUDIO_SMODEM_IN);
	}
}
void __AUDIO_SMODEM_IN_handler()
{
	serial_put_string("__AUDIO_SMODEMIN_handler \x0a\x0d");
	
	__call_callback(AUDIO_SMODEM_IN_callback);
}
void unregister_AUDIO_SMODEM_IN_handler( void (*callback)() )
{
	__unregister_callback(&AUDIO_SMODEM_IN_callback, callback);
}

void register_AUDIO_SMODEM_OUT_handler(void (*callback)())
{
	__register_callback(&AUDIO_SMODEM_OUT_callback, callback);
}
void set_AUDIO_SMODEM_OUT_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_SET,   BUS_INT_AUDIO_SMODEM_OUT);
		REGISTER_WRITE(BUS_INT_EN_SET,         BUS_INT_AUDIO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_CLEAR, BUS_INT_AUDIO_SMODEM_OUT);
	}
}
void __AUDIO_SMODEM_OUT_handler()
{
	serial_put_string("__AUDIO_SMODEM_OUT_handler\x0a\x0d");
	
	__call_callback(AUDIO_SMODEM_OUT_callback);
}
void unregister_AUDIO_SMODEM_OUT_handler( void (*callback)() )
{
	__unregister_callback(&AUDIO_SMODEM_OUT_callback, callback);
}

void register_AUDIO_DIVUNIT_handler(void (*callback)())
{
	__register_callback(&AUDIO_DIVUNIT_callback, callback);
}
void set_AUDIO_DIVUNIT_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_SET,     BUS_INT_AUDIO_DIVUNIT);
		REGISTER_WRITE(BUS_INT_EN_SET,         BUS_INT_AUDIO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_CLEAR, BUS_INT_AUDIO_DIVUNIT);
	}
}
void __AUDIO_DIVUNIT_handler()
{
	serial_put_string("__AUDIO_DIVUNIT_handler\x0a\x0d");
	
	__call_callback(AUDIO_DIVUNIT_callback);
}
void unregister_AUDIO_DIVUNIT_handler( void (*callback)() )
{
	__unregister_callback(&AUDIO_DIVUNIT_callback, callback);
}

void register_AUDIO_IN_handler(void (*callback)())
{
	__register_callback(&AUDIO_IN_callback, callback);
}
void set_AUDIO_IN_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_SET,   BUS_INT_AUDIO_AUDIO_IN);
		REGISTER_WRITE(BUS_INT_EN_SET,         BUS_INT_AUDIO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_AUDIO_EN_CLEAR, BUS_INT_AUDIO_AUDIO_IN);
	}
}
void __AUDIO_IN_handler()
{
	serial_put_string("__AUDIO_IN_handler\x0a\x0d");
	
	__call_callback(AUDIO_IN_callback);
}
void unregister_AUDIO_IN_handler( void (*callback)() )
{
	__unregister_callback(&AUDIO_IN_callback, callback);
}

void register_AUDIO_OUT_handler(void (*callback)())
{
	__register_callback(&AUDIO_OUT_callback, callback);
}
void set_AUDIO_OUT_interrupt(bool active)
{
	if(active)
	{
		if(is_spot_box())
		{
			if(active)
			{
				REGISTER_WRITE(BUS_INT_EN_SET,         BUS_INT_SPOT_AUDIO);
			}
			else
			{
				REGISTER_WRITE(BUS_INT_EN_CLEAR,       BUS_INT_SPOT_AUDIO);
			}
		}
		else
		{
			if(active)
			{
				REGISTER_WRITE(BUS_INT_AUDIO_EN_SET,   BUS_INT_AUDIO_AUDIO_OUT);
				REGISTER_WRITE(BUS_INT_EN_SET,         BUS_INT_AUDIO);
			}
			else
			{
				REGISTER_WRITE(BUS_INT_AUDIO_EN_CLEAR, BUS_INT_AUDIO_AUDIO_OUT);
			}
		}
	}
}
void __AUDIO_OUT_handler() // SPOT
{
	serial_put_string("__AUDIO_OUT_handler\x0a\x0d");
	//progress_audio();
	
	__call_callback(AUDIO_OUT_callback);
}
void unregister_AUDIO_OUT_handler( void (*callback)() )
{
	__unregister_callback(&AUDIO_OUT_callback, callback);
}


void register_RIO_SLOT0_handler(void (*callback)())
{
	__register_callback(&RIO_SLOT0_callback, callback);
}
void set_RIO_SLOT0_interrupt(bool active)
{
	if(is_spot_box())
	{
		if(active)
		{
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_SPOT_MODEM);
		}
		else
		{
			REGISTER_WRITE(BUS_INT_EN_CLEAR,     BUS_INT_SPOT_MODEM);
		}
	}
	else
	{
		if(active)
		{
			REGISTER_WRITE(BUS_INT_RIO_EN_SET,   BUS_INT_RIO_SLOT0);
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_RIO);
		}
		else
		{
			REGISTER_WRITE(BUS_INT_RIO_EN_CLEAR, BUS_INT_RIO_SLOT0);
		}
	}
}
void __RIO_SLOT0_handler() // modem/ethernet, SPOT modem
{
	serial_put_string("__RIODEV_SLOT0_handler\x0a\x0d");
	
	__call_callback(RIO_SLOT0_callback);
}
void unregister_RIO_SLOT0_handler( void (*callback)() )
{
	__unregister_callback(&RIO_SLOT0_callback, callback);
}

void register_RIO_SLOT1_handler(void (*callback)())
{
	__register_callback(&RIO_SLOT1_callback, callback);
}
void set_RIO_SLOT1_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_RIO_EN_SET,   BUS_INT_RIO_SLOT1);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_RIO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_RIO_EN_CLEAR, BUS_INT_RIO_SLOT1);
	}
}
void __RIO_SLOT1_handler() // IDE
{
	serial_put_string("__RIODEV_SLOT1_handler\x0a\x0d");
	
	__call_callback(RIO_SLOT1_callback);
}
void unregister_RIO_SLOT1_handler( void (*callback)() )
{
	__unregister_callback(&RIO_SLOT1_callback, callback);
}

void register_RIO_SLOT2_handler(void (*callback)())
{
	__register_callback(&RIO_SLOT2_callback, callback);
}
void set_RIO_SLOT2_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_RIO_EN_SET,   BUS_INT_RIO_SLOT2);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_RIO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_RIO_EN_CLEAR, BUS_INT_RIO_SLOT2);
	}
}
void __RIO_SLOT2_handler()
{
	serial_put_string("__RIODEV_SLOT2_handler\x0a\x0d");
	
	__call_callback(RIO_SLOT2_callback);
}
void unregister_RIO_SLOT2_handler( void (*callback)() )
{
	__unregister_callback(&RIO_SLOT2_callback, callback);
}

void register_RIO_SLOT3_handler(void (*callback)())
{
	__register_callback(&RIO_SLOT3_callback, callback);
}
void set_RIO_SLOT3_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_RIO_EN_SET,   BUS_INT_RIO_SLOT3);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_RIO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_RIO_EN_CLEAR, BUS_INT_RIO_SLOT3);
	}
}
void __RIO_SLOT3_handler()
{
	serial_put_string("__RIODEV_SLOT3_handler\x0a\x0d");
	
	__call_callback(RIO_SLOT3_callback);
}
void unregister_RIO_SLOT3_handler( void (*callback)() )
{
	__unregister_callback(&RIO_SLOT3_callback, callback);
}


void register_DEV_GPIO_handler(void (*callback)())
{
	__register_callback(&DEV_GPIO_callback, callback);
}
void set_DEV_GPIO_interrupt(bool active)
{
}
void __DEV_GPIO_handler()
{
	serial_put_string("__DEV_GPIO_handler\x0a\x0d");
	
	__call_callback(DEV_GPIO_callback);
}
void unregister_DEV_GPIO_handler( void (*callback)() )
{
	__unregister_callback(&DEV_GPIO_callback, callback);
}

void register_DEV_UART_handler(void (*callback)())
{
	__register_callback(&DEV_UART_callback, callback);
}
void set_DEV_UART_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_DEV_EN_SET,   BUS_INT_DEV_UART);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_VIDEO);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_DEV_EN_CLEAR, BUS_INT_DEV_UART);
	}
}
void __DEV_UART_handler()
{
	serial_put_string("__DEV_UART_handler\x0a\x0d");
	
	__call_callback(DEV_UART_callback);
}
void unregister_DEV_UART_handler( void (*callback)() )
{
	__unregister_callback(&DEV_UART_callback, callback);
}

void register_DEV_SMARTCARD_handler(void (*callback)())
{
	__register_callback(&DEV_SMARTCARD_callback, callback);
}
void set_DEV_SMARTCARD_interrupt(bool active)
{
	if(active)
	{
		if(is_spot_box())
		{
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_SPOT_SMARTCARD);
		}
		else
		{
			REGISTER_WRITE(BUS_INT_DEV_EN_SET,   BUS_INT_DEV_SMARTCARD);
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_DEV);
		}
	}
	else
	{
		if(is_spot_box())
		{
			REGISTER_WRITE(BUS_INT_EN_CLEAR,     BUS_INT_SPOT_SMARTCARD);
		}
		else
		{
			REGISTER_WRITE(BUS_INT_DEV_EN_CLEAR, BUS_INT_DEV_SMARTCARD);
		}
	}
}
void __DEV_SMARTCARD_handler() // Spot SmartCard
{
	serial_put_string("__DEV_SMARTCARD_handler\x0a\x0d");
	
	__call_callback(DEV_SMARTCARD_callback);
}
void unregister_DEV_SMARTCARD_handler( void (*callback)() )
{
	__unregister_callback(&DEV_SMARTCARD_callback, callback);
}

void register_DEV_PPORT_handler(void (*callback)())
{
	__register_callback(&DEV_PPORT_callback, callback);
}
void set_DEV_PPORT_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_DEV_EN_SET,   BUS_INT_DEV_PPORT);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_DEV);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_DEV_EN_CLEAR, BUS_INT_DEV_PPORT);
	}
}
void __DEV_PPORT_handler()
{
	serial_put_string("__DEV_PARPORT_handler\x0a\x0d");
	
	__call_callback(DEV_PPORT_callback);
}
void unregister_DEV_PPORT_handler( void (*callback)() )
{
	__unregister_callback(&DEV_PPORT_callback, callback);
}

void register_DEV_IR_OUT_handler(void (*callback)())
{
	__register_callback(&DEV_IR_OUT_callback, callback);
}
void set_DEV_IR_OUT_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_DEV_EN_SET,   BUS_INT_DEV_IR_OUT);
		REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_DEV);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_DEV_EN_CLEAR, BUS_INT_DEV_IR_OUT);
	}
}
void __DEV_IR_OUT_handler()
{
	serial_put_string("__DEV_IR_OUT_handler\x0a\x0d");
	
	__call_callback(DEV_IR_OUT_callback);
}
void unregister_DEV_IR_OUT_handler( void (*callback)() )
{
	__unregister_callback(&DEV_IR_OUT_callback, callback);
}

void register_DEV_IR_IN_handler(void (*callback)())
{
	__register_callback(&DEV_IR_IN_callback, callback);
}
void set_DEV_IR_IN_interrupt(bool active)
{
	if(active)
	{
		if(is_spot_box())
		{
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_SPOT_IR_IN);
		}
		else
		{
			REGISTER_WRITE(BUS_INT_DEV_EN_SET,   BUS_INT_DEV_IR_IN);
			REGISTER_WRITE(BUS_INT_EN_SET,       BUS_INT_DEV);
			REGISTER_WRITE(BUS_INT_STAT_SET,     BUS_INT_DEV); //
		}
	}
	else
	{
		if(is_spot_box())
		{
			REGISTER_WRITE(BUS_INT_EN_CLEAR,     BUS_INT_SPOT_IR_IN);
		}
		else
		{
			REGISTER_WRITE(BUS_INT_DEV_EN_CLEAR, BUS_INT_DEV_IR_IN);
		}
	}
}
void __DEV_IR_IN_handler() // Spot IR
{
	//serial_put_string("__DEV_IR_IN_handler\x0a\x0d");
	
	__call_callback(DEV_IR_IN_callback);
}
void unregister_DEV_IR_IN_handler( void (*callback)() )
{
	__unregister_callback(&DEV_IR_IN_callback, callback);
}

void register_DEV_PS2_handler(void (*callback)())
{
	__register_callback(&DEV_PS2_callback, callback);
}
void set_DEV_PS2_interrupt(bool active)
{
	if(active)
	{
		REGISTER_WRITE(BUS_INT_EN_SET,     BUS_INT_SPOT_PS2);
	}
	else
	{
		REGISTER_WRITE(BUS_INT_EN_SET,     BUS_INT_SPOT_PS2);
	}
}
void __DEV_PS2_handler() // Spot only
{
	serial_put_string("__DEV_PS2_handler\x0a\x0d");

	__call_callback(DEV_PS2_callback);
}
void unregister_DEV_PS2_handler( void (*callback)() )
{
	__unregister_callback(&DEV_PS2_callback, callback);
}
