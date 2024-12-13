#include "wtvsys.h"
#include "libc.h"
#include "regs_internal.h"
#include "utils.h"

static uint32_t ticks64_base_tick;
static uint32_t ticks64_base;

bool is_solo_box()
{
    
	return ((REGISTER_READ(BUS_CHIP_ID) & CHIP_ID_SOLO) == CHIP_ID_SOLO);
}

bool is_spot_box()
{
	return ((REGISTER_READ(BUS_CHIP_ID) & CHIP_ID_SOLO) == CHIP_ID_SPOT);
}

uint32_t get_chip_id()
{
	return REGISTER_READ(BUS_CHIP_ID);
}

uint32_t get_sysconfig()
{
	return REGISTER_READ(RIO_SYSCONFIG);
}

/**
 * @brief Helper macro to perform cache refresh operations
 *
 * @param[in] op
 *            Operation to perform
 * @param[in] linesize
 *            Size of a cacheline in bytes
 */
#define cache_op(op, linesize) ({ \
    if (length) { \
        void *cur = (void*)((unsigned long)addr & ~(linesize-1)); \
        int count = (int)length + (addr-cur); \
        for (int i = 0; i < count; i += linesize) \
            asm ("\tcache %0,(%1)\n"::"i" (op), "r" (cur+i)); \
    } \
})

void data_cache_hit_writeback(volatile const void * addr, unsigned long length)
{
    cache_op(0x19, 16);
}

/** @brief Underlying implementation of data_cache_hit_invalidate */
void __data_cache_hit_invalidate(volatile void * addr, unsigned long length)
{
    cache_op(0x11, 16);
}

void data_cache_hit_writeback_invalidate(volatile void * addr, unsigned long length)
{
    cache_op(0x15, 16);
}

void data_cache_index_writeback_invalidate(volatile void * addr, unsigned long length)
{
    cache_op(0x01, 16);
}

void data_cache_writeback_invalidate_all(void)
{
    // TODO: do an index op instead for better performance
    data_cache_hit_writeback_invalidate(KSEG0_START_ADDR, get_memory_size());
}

void inst_cache_hit_writeback(volatile const void * addr, unsigned long length)
{
    cache_op(0x18, 32);
}

void inst_cache_hit_invalidate(volatile void * addr, unsigned long length)
{
    cache_op(0x10, 32);
}

void inst_cache_index_invalidate(volatile void * addr, unsigned long length)
{
    cache_op(0x00, 32);
}

void inst_cache_invalidate_all(void)
{
    // TODO: do an index op instead for better performance
    inst_cache_hit_invalidate(KSEG0_START_ADDR, get_memory_size());
}

void *malloc_uncached(size_t size)
{
    return malloc_uncached_aligned(16, size);
}

void *malloc_uncached_aligned(int align, size_t size)
{
    // Since we will be accessing the buffer as uncached memory, we absolutely
    // need to prevent part of it to ever enter the data cache, even as false
    // sharing with contiguous buffers. So we want the buffer to exclusively
    // cover full cachelines (aligned to minimum 16 bytes, multiple of 16 bytes).
    if (align < 16)
        align = 16;
    size = ROUND_UP(size, 16);
    void *mem = memalign(align, size);
    if (!mem) return NULL;

    // The memory returned by the system allocator could already be partly in
    // cache (eg: it might have been previously used as a normal heap buffer
    // and recently returned to the allocator). Invalidate it so that
    // we don't risk a writeback in the short future.
    data_cache_hit_invalidate(mem, size);

    // Return the pointer as uncached memory.
    return UncachedAddr(mem);
}

uint32_t gpio_write(uint32_t value, bool clear)
{
	if(clear)
	{
		REGISTER_WRITE(DEV_GPIO_OUT_CLEAR, value);
	}
	else
	{
		REGISTER_WRITE(DEV_GPIO_OUT_SET, value);
	}

	return REGISTER_READ(DEV_GPIO_OUT_SET);
}

void free_uncached(void *buf)
{
    freealign(CachedAddr(buf));
}

int get_memory_size()
{
    return AG(ram_size);
}

/** @brief Memory location to read which determines the TV type. */
#define TV_TYPE_LOC  0x80000300

tv_type_t get_tv_type() 
{
    return *((uint32_t *) TV_TYPE_LOC);
}

void usleepX(uint32_t time)
{
	if(!is_solo_box())
	{
        //
    }
    else
    {
        uint32_t cnt = *((volatile unsigned long *)BUS_TIMER_COUNT);

        uint32_t v2 = AG(counts_per_microsec) * time;

        while((*((volatile unsigned long *)BUS_TIMER_COUNT) - cnt) < v2) { }
    }
}

uint32_t get_ticks_per_microsec()
{
    return AG(counts_per_microsec);
}

uint32_t get_cpu_speed()
{
    return AG(cpu_speed);
}

uint32_t get_bus_speed()
{
    return AG(bus_speed);
}


uint64_t get_ticks(void)
{
	uint32_t now = TICKS_READ();
	uint32_t prev = ticks64_base_tick;
	ticks64_base_tick = now;
	ticks64_base += now - prev;

	return ticks64_base;
}

uint64_t get_ticks_us(void)
{
    return TICKS_TO_US(get_ticks());
}

uint64_t get_ticks_ms(void)
{
    return TICKS_TO_MS(get_ticks());
}

void wait_ticks(unsigned long wait)
{
    unsigned int initial_tick = TICKS_READ();
    while(TICKS_READ() - initial_tick < wait);
}

void wait_s(unsigned long wait_s)
{
    wait_ticks(TICKS_FROM_MS(wait_s * 1000));
}

void wait_ms(unsigned long wait_ms)
{
    wait_ticks(TICKS_FROM_MS(wait_ms));
}

void wait_us(unsigned long wait_us)
{
    wait_ticks(TICKS_FROM_US(wait_us));
}

/**
 * @brief Initialize COP1 with default settings that prevent undesirable exceptions.
 *
 */
__attribute__((constructor)) void __init_cop1(void)
{
    /* Read initialized value from cop1 control register */
    uint32_t fcr31 = C1_FCR31();
    
    /* Disable all pending exceptions to avoid triggering one immediately.
       These can be survived from a soft reset. */
    fcr31 &= ~(C1_CAUSE_OVERFLOW | 
               C1_CAUSE_UNDERFLOW | 
               C1_CAUSE_NOT_IMPLEMENTED | 
               C1_CAUSE_DIV_BY_0 | 
               C1_CAUSE_INEXACT_OP | 
               C1_CAUSE_INVALID_OP);

#ifndef NDEBUG
    /** 
     * Enable FPU exceptions that can help programmers avoid bugs in their code. 
     * Underflow exceptions are not enabled because they are triggered whenever
     * a denormalized float is generated, *even if* the FS bit is set (see below).
     * So basically having the underflow exception enabled seems to be useless
     * unless also the underflow (and the inexact) exceptions are off.
     * Notice that underflows can happen also with library code such as
     * tanf(BITCAST_I2F(0x3f490fdb)) (0.785398185253).
     */
    fcr31 |= C1_ENABLE_OVERFLOW | 
             C1_ENABLE_DIV_BY_0 | 
             C1_ENABLE_INVALID_OP;
#endif

    /* Set FS bit to allow flashing of denormalized floats
       The FPU inside the N64 CPU does not implement denormalized floats
       and will generate an unmaskable exception if a denormalized float
       is generated by any floating point operations. In order to prevent
       this exception we set the FS bit in the fcr31 control register to
       instead "flash" and "flush" the denormalized number. To understand 
       the flashing rules please read pg. 213 of the VR4300 programmers manual */
    fcr31 |= C1_FCR31_FS;

    /* Write back updated cop1 control register */
    C1_WRITE_FCR31(fcr31);
}
