#include "idtcpu-regs.S"


	.set noreorder
	.section .text.startup.crt0
	.global _start
_start:
	b     setup_for_main
	nop

code_checksum:               .word 0x00000000
build_size:                  .word __build_dword_size
code_size:                   .word __code_dword_size
build_version:               .word 0x00000000
data_stored_base:            .word __data_stored_start
data_stored_size:            .word __data_dword_size
bss_size:                    .word __bss_dword_size
romfs_base:                  .word 0x4e6f4653 # = NoFS. This starts out without a ROMFS. It can be added later.
level1_lzj_version:          .word 0x00000000
level1_image_size:           .word 0x00000000
build_base:                  .word __build_base
build_flags:                 .word 0x00000000
data_stored_compressed_size: .word 0x00000000
bootrom_level1_base:         .word 0x00000000
reserved0:                   .word 0x00000000
reserved1:                   .word 0x00000000
reserved2:                   .word 0x00000000
reserved3:                   .word 0x00000000

setup_for_main:
	mfc0  t0, C0_PRID
	andi  t0, 0xff00
	beq   t0, (PRID_R4650 << 0x10), after_setup_mmu
	nop

after_setup_mmu:
	la    a0, 0xa4000000
	lw    t0, 0(a0)
	and   t0, 0x03000000
	beq   t0, 0x03000000, setup_solo_stack
	nop
setup_spot_stack:
	/*la    a0, 0x80000280
	lw    t0, 0x0(a0)*/
	la    t0, 0x00800000
	or    t0, 0x80000000
	move  sp, t0
	b     setup_global_pointer
	nop
setup_solo_stack:
	move  sp, a0
setup_global_pointer:
	la    gp, _gp

	lw    t0, build_flags
	andi  t0, 0x1
	bne   t0, 0x0, setup_data_section_compressed
	nop
setup_data_section_uncompressed:
	lw    t0, data_stored_base
	lw    t1, data_stored_size
	la    t2, __data_start
copy_data_section_dword:
	lw    t3, 0x0(t0)
	addu  t0, 0x4
	sw    t3, 0x0(t2)
	subu  t1, 0x1
	bne   t1, 0x0, copy_data_section_dword
	addu  t2, 0x4
	b     setup_bss_section
	nop
setup_data_section_compressed:
	nop
	nop
	nop
	nop
	nop

setup_bss_section:
	lw    t1, bss_size
	beq   t1, 0x0, setup_exception_vectors
	nop
zero_bss_section_dword:
	sw    zero, 0x0(t2)
	subu  t1, 0x1
	bne   t1, 0x0, zero_bss_section_dword
	addu  t2, 0x4

setup_exception_vectors:
	la    t0, goto_exception_handler
	la    t1, 0x0c
	la    t2, K0BASE
copy_exception_handler_dword:
	lw    t3, 0x0(t0)
	addu  t0, 0x4
	sw    t3, C_K0_OFFSET(t2)
	cache HIT_WRITEBACK_INVALIDATE_D, C_K0_OFFSET(t2)
	sw    t3, E_K0_OFFSET(t2)
	cache HIT_WRITEBACK_INVALIDATE_D, E_K0_OFFSET(t2)
	sw    t3, I_K0_OFFSET(t2)
	cache HIT_WRITEBACK_INVALIDATE_D, I_K0_OFFSET(t2)
	subu  t1, 0x1
	bne   t1, 0x0, copy_exception_handler_dword
	addi  t2, 0x4

	/* 
		     CU3 CU2 CU1 CU0 _ FE RE DL IL BEV _ SR _ CH CE DE        IM __ UX UM _ ERL EXL IE
		t3 =   0   0   1   1 0  1  0  0  0   0 0  0 0  0  0  0  11111111 00  0  0 0   0   0  0

		From the iDT MIPS reference manual:

		CU3, CU2	Bits (31:30) control the usability of “co-processors” 3 and 2,
					respectively. In the R30xx family, these might be enabled if
					software wishes to use the BrCond(3:2) input pins for polling,
					or to speed exception decoding.
		CU1 		Co-processor 1 usable: set 1 to use FPA if present, 0 to disable.
					When 0, all FPA instructions cause an interrupt exception,
					even for the kernel. It can be useful to turn off an FPA even
					when one is available; it may also be enabled in devices which
					do not include an FPA, if the intent is to use the BrCond(1) pin
					as a polled input.
		CU0 		Co-processor 0 usable: set 1 to use some nominally-privileged
					instructions in user mode (this is rarely, if ever, done). Co-
					processor 0 instructions are always usable in kernel mode,
					regardless of the setting of this bit.
		RE 			Reverse endianness in user mode. The MIPS processors can be
					configured, at reset time, with either “endianness” (byte
					ordering convention, discussed in the various CPU’s User’s
					Manuals and later in this manual). The RE bit allows
					applications with one byte ordering convention to be run on
					systems with the opposite convention, presuming OS software
					provided the necessary support.
					When RE is active, user mode software runs as if the CPU had
					been configured with the opposite endianness.
		FE			Enables additional floating-point registers
		DL			Data cache lock, a new bit in RC4650. Does not prevent refills 
					into set A when set A is invalid. Does not inhibit
					update of the D-cache on store operations.
		IL			Instruction cache lock, a new bit in RC4650. Does not prevent
					refills into set A when set A is invalid. 
		BEV 		Boot exception vectors: when BEV == 1, the CPU uses the ROM
					(kseg1) space exception entry point (described in a later
					chapter). BEV is usually set to zero in running systems; this
					relocates the exception vectors. to RAM addresses, speeding
					accesses and allowing the use of “user supplied” exception
					service routines.
		SR			Indicates a soft reset or NMI has occurred.
		CH			Hit (tag match and valid state) or miss indication for last CACHE
					Hit Invalidate, Hit Write Back Invalidate, Hit Write Back, or 
					Hit Set Virtual for a primary cache.
		CE			Contents of the ECC register set or modify the check bits of the 
					caches when CE = 1; see description of the ECC register.
		DE			Specifies that cache parity errors cannot cause exceptions.
		IM 			Interrupt mask: an 8 bit field defining which interrupt sources,
					when active, will be allowed to cause an exception. Six of the
					interrupt sources are external pins (one may be used by the
					FPA, which although it lives on the same chip is logically
					external); the other two are the software-writable interrupt bits
					in the Cause register.
					No interrupt prioritizing is provided by the CPU. This is
					described in greater detail in the chapter dealing with
					exceptions.
		UX 			Enables 64-bit virtual addressing and operations in User mode. 
					The extended-addressing TLB refill exception is used for TLB 
					misses on user addresses
		UM 			User Mode bit, a new bit in RC4650.
		ERL 		Error Level
		EXL 		Exception Level
		IE 			Interrupt Enable

		- Enable coprocessor 0 and 1.
		- Enable floating point registers.
		- Enable RAM interrupt vectors (BEV = 0).
		- Interrupt mask: enable all interrupts (0x44 at minimum)
	*/
	la    t3, 0x3400ff00
	mtc0  t3, C0_SR


goto_main:
	la   t0, main
	j    t0
	nop

goto_exception_handler:
	la   t0, exception_handler
	jr   t0
	nop
	eret
	nop
	nop
	nop
	nop

/*
.global	init_mmu
init_mmu:
.frame	sp,0,$31
	.set	noreorder
	
	mtc0    $zero, Wired     # Number of wired TLB entries
	mtc0    $zero, PageMask  # TLB Page Mask
	li      $t0, 1
	move    $t1, $zero
	mtc0    $t0, EntryLo0    # Low half of TLB entry for even virtual address (VPN)
	mtc0    $t0, EntryLo1    # Low half of TLB entry for odd virtual address (VPN)
	li      $t2, 0x2F  # '/'
	lui     $t3, 0xF

	loc_8053F5C0:
	mtc0    $t2, Index       # Programmable pointer into TLB array
	add     $t1, $t3, $t2
	sll     $t1, 13
	mtc0    $t1, EntryHi     # High half of TLB entry
	nop
	nop
	nop
	tlbwi
	bne     $zero, $t2, loc_8053F5C0
	addi    $t2, -1

	li      $a0, unk_806ECF48
	lui     $t0, 0x2000
	or      $a0, $t0
	lw      $t0, 0($a0)

	loc_8053F5FC:
	lw      $t1, 4($a0)
	mtc0    $t0, Index       # Programmable pointer into TLB array
	mtc0    $t1, PageMask    # TLB Page Mask
	ld      $t0, 8($a0)
	lw      $t1, 0x10($a0)
	dmtc0   $t0, EntryHi     # High half of TLB entry
	mtc0    $t1, EntryLo0    # Low half of TLB entry for even virtual address (VPN)
	lw      $t0, 0x14($a0)
	addiu   $a0, 0x18
	mtc0    $t0, EntryLo1    # Low half of TLB entry for odd virtual address (VPN)
	lw      $t0, 0($a0)
	nop
	tlbwi
	bgez    $t0, loc_8053F5FC
	nop

	jr      $ra
	nop	
	
	.set	reorder

.end	init_mmu
*/