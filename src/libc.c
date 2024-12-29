#include <stdbool.h>
#include "serial.h"
#include "libc.h"
#include "graphics.h"
#include "debug.h"
#include "display.h"
#include "wtvsys.h"

static bool serial_output_enabled = true;
static bool callback_output_enabled = true;

static O1HeapInstance* heap = NULL;
extern void __init_interrupts(void);

/**
 * @brief Structure of an interrupt callback
 */
typedef struct callback_link
{
    /** @brief Callback function */
    void (*callback)(const char character);
    /** @brief Pointer to next callback */
    struct callback_link * next;
} _callback_link;

struct callback_link *putchar_callback = 0;

//EMAC: consolidate with register, call and unregister from interrupt.c???

/**
 * @brief Add a callback to a linked list of callbacks
 *
 * @param[in,out] head
 *                Pointer to the head of a linked list to add to
 * @param[in]     callback
 *                Function to call when executing callbacks in this linked list
 */
static void __register_callback(struct callback_link ** head, void (*callback)(const char character))
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
static void __call_callback(struct callback_link * head, const char character)
{
    /* Call each registered callback */
    while( head )
    {
        if(head->callback)
        {
        	head->callback(character);
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
static void __unregister_callback(struct callback_link ** head, void (*callback)(const char character))
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

#include "leds.h"

void heap_init2()
{
    serial_io_init();

	heap_init(HEAP_START_ADDR, HEAP_SIZE_BYTES);

	__init_interrupts();
}

void heap_init(void* const base, const size_t size)
{
	heap = o1heapInit(base, size);
}

bool heap_check()
{
	return o1heapDoInvariantsHold(heap);
}

O1HeapDiagnostics heap_diag(void* const base, const size_t size)
{
	return o1heapGetDiagnostics(heap);
}

void* malloc(const size_t amount)
{
	return o1heapAllocate(heap, amount);
}

void free(void* const pointer)
{
	o1heapFree(heap, pointer);
}

void* memalign(size_t alignment, const size_t amount)
{
	size_t unaligned_amount = amount + alignment;
	uint8_t* unaligned_buffer = malloc(unaligned_amount);

	uint8_t unaligned_skew = alignment - (((size_t)unaligned_buffer) % alignment);

	uint8_t* pointer = unaligned_buffer + unaligned_skew;

	// Store amount of bytes offset from the original malloced buffer to align this buffer.
	// This is important so we can free the buffer properly.
	*(pointer - 1) = unaligned_skew;

	return (void*)pointer;
}

void freealign(void* const pointer)
{
	if (pointer)
	{
		// Grab the amoutn of bytes offset to align the this buffer from memalign
		uint8_t unaligned_skew = *(((uint8_t*)pointer) - 1);

		free(((uint8_t*)pointer) - unaligned_skew);
	}
}

void putchar_(const char character)
{
    if(serial_output_enabled)
    {
        serial_put_char(character);
    }

    if(callback_output_enabled)
    {
        __call_callback(putchar_callback, character);
    }
}

void set_serial_output(bool output_enabled)
{
    serial_output_enabled = output_enabled;
}

void set_callback_output(bool output_enabled)
{
    callback_output_enabled = output_enabled;
}

void register_putchar_callback(void (*callback)(const char character))
{
    __register_callback(&putchar_callback, callback);
}

void unregister_putchar_callback(void (*callback)(const char character))
{
    __unregister_callback(&putchar_callback, callback);
}


//EMAC:
int toupper(int c)
{
	if(c >= 'a' && c <= 'z')
	{
		c -= ('a' - 'A');
	}

	return c;
}
//EMAC:
int stricmp(const char *s1, const char *s2)
{
    while(*s1 != 0 && *s2 != 0)
    {
        if(*s1 != *s2 && toupper(*s1) != toupper(*s2))
        {
            return -1;
        }
        s1++;
        s2++;
    }
    return (*s1 == 0 && *s2 == 0) ? 0 : -1;
}

int stricmp2(const char *s1, const char *s2, size_t n)
{
    while(*s1 != 0 && *s2 != 0 && n > 0)
    {
        if(*s1 != *s2 && toupper(*s1) != toupper(*s2))
        {
            return -1;
        }
        s1++;
        s2++;
		n--;
    }
    return (*s1 == 0 && *s2 == 0) ? 0 : -1;
}

void __assert_fail(const char* assertion, const char* file, unsigned int line, const char* function)
{
    printf("Error: %s\x0a\x0d\tfile %s [%s] line %d\x0a\x0d", assertion, file, function, line);
    __asm__("break");
    while(1) {}
}