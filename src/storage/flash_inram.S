#include "../idtcpu-regs.S"

#define INRAM_FUNCTION_FAILED    0xffffffff
#define INRAM_FUNCTION_SUCCEEDED 0x00000000
#define MX_PAGE_SIZE             0x100      /* = 128 bytes per page x 2 stripped flash chips */

	.section .text.flash_inram_functions
	.set noreorder

/* ** ** ** ** ** ** ** ** ** ** ** ** */
/* ** ** ** ** ** ** ** ** ** ** ** ** */

	.globl flash_get_device_id
	.func flash_get_device_id
flash_get_device_id:
	mfc0    t0, C0_SR
	and     t2, t0, 0xfffffffe
	mtc0    t2, C0_SR
	la      t1, 0x10000
	addu    t1, a0, t1
	lw      t2, 0x0000(a0)
flash_check_maskrom_id:
/*
	*+0x0000 = 0x90
*/
	la      t5, 0x90909090
	sw      t5, 0x0000(a0)
	lw      t6, 0x0000(a0)
	bne     t2, t6, flash_found_id
	nop

flash_check_flashrom_id:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0x90
*/
	la      t3, 0x00aa00aa
	sw      t3, 0x5554(t1)
	la      t4, 0x00550055
	sw      t4, -0x5558(t1)
	la      t5, 0x00900090
	sw      t5, 0x5554(t1)
	lw      t6, 0x0000(a0)
	beq     t2, t6, flash_unknown_identity
	nop

flash_found_id:
	lw      v0, 0x0000(a0)
	sll     v0, v0, 0x10
	lw      v1, 0x0004(a0)
	and     v1, 0xffff
	or      v0, v1
/*
	*+0x0000 = 0xf0
*/
	la      t5, 0x00f000f0
	sw      t5, 0x0000(a0)
	lw      t6, 0x0000(a0)
	beq     t2, t6, flash_get_device_id_exit
	nop

alternative_reset:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0xf0
*/
	sw      t3, 0x5554(t1)
	sw      t4, -0x5558(t1)
	sw      t5, 0x5554(t1)
	b       flash_get_device_id_exit
	nop

flash_unknown_identity:
	la      v0, INRAM_FUNCTION_FAILED

flash_get_device_id_exit:
	mtc0    t0, C0_SR
	jr      ra
	nop

	.set reorder
	.endfunc

/* ** ** ** ** ** ** ** ** ** ** ** ** */
/* ** ** ** ** ** ** ** ** ** ** ** ** */

	.globl flash_jedec_erase_sector
	.func flash_jedec_erase_sector
flash_jedec_erase_sector:
	mfc0    t0, C0_SR
	and     t2, t0, 0xfffffffe
	mtc0    t2, C0_SR
	la      t1, 0x10000
	addu    t1, a0, t1

flash_jedec_erase_sector_prepare:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0x80
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+sector = 0x30
*/
	la      t3, 0x00aa00aa
	sw      t3, 0x5554(t1)
	la      t4, 0x00550055
	sw      t4, -0x5558(t1)
	la      t5, 0x00800080
	sw      t5, 0x5554(t1)
	sw      t3, 0x5554(t1)
	sw      t4, -0x5558(t1)

flash_jedec_erase_sector_do:
	la      t6, 0x00300030
	sw      t6, 0x0000(a1)

flash_jedec_erase_sector_wait:
	lw      t6, 0x0000(a1)
	and     t6, t5, t6
	bne     t6, t5, flash_jedec_erase_sector_wait
	nop

flash_jedec_erase_sector_reset:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0xf0
*/
	sw      t3, 0x5554(t1)
	sw      t4, -0x5558(t1)
	la      t5, 0x00f000f0
	sw      t5, 0x5554(t1)

flash_jedec_erase_sector_exit:
	mtc0    t0, C0_SR
	la      v0, INRAM_FUNCTION_SUCCEEDED
	jr      ra
	nop

	.set reorder
	.endfunc

/* ** ** ** ** ** ** ** ** ** ** ** ** */
/* ** ** ** ** ** ** ** ** ** ** ** ** */

	.globl flash_jedec_page_program
	.func flash_jedec_page_program
flash_jedec_page_program:
	mfc0    t0, C0_SR
	and     t2, t0, 0xfffffffe
	mtc0    t2, C0_SR
	la      t1, 0x10000
	addu    t1, a0, t1
	move    t6, ra
	la      t7, flash_get_post_page_programming_wait
	jalr    t7
	nop
	move    ra, t6 
	move    t8, v0

flash_jedec_page_program_prepare:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0xa0

	*+xxxx = byte 
*/
	la      t3, 0x00aa00aa
	sw      t3, 0x5554(t1)
	la      t4, 0x00550055
	sw      t4, -0x5558(t1)
	la      t5, 0x00a000a0
	sw      t5, 0x5554(t1)
	la      t6, MX_PAGE_SIZE

flash_jedec_page_program_do:
	lw      t7, 0x0000(a2)
	sw      t7, 0x0000(a1)
	addi    a1, 4
	addi    t6, -4
	addi    a2, 4
	bnez    t6, flash_jedec_page_program_do
	nop

	mfc0    t6, C0_COUNT
	mtc0    zero, C0_COUNT
flash_jedec_page_program_wait:
	mfc0    t7, C0_COUNT
	slt     t9, t7, t8
	bnez    t9, flash_jedec_page_program_wait
	nop
	mtc0    t6, C0_COUNT

	la      t5, 0x00800080
flash_jedec_page_programmed_wait:
	lw      t6, 0x0000(a0)
	and     t6, t5, t6
	bne     t6, t5, flash_jedec_page_programmed_wait
	nop

flash_jedec_page_program_reset:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0xf0
*/
	sw      t3, 0x5554(t1)
	sw      t4, -0x5558(t1)
	la      t5, 0x00f000f0
	sw      t5, 0x5554(t1)

	la      t5, 0x00100010
	and     t6, t5, t6
	bnez    t6, flash_jedec_page_program_failed
	nop

	addi    a3, -MX_PAGE_SIZE
	bnez    a3, flash_jedec_page_program_prepare
	nop
	la      v0, INRAM_FUNCTION_SUCCEEDED
	b       flash_jedec_page_program_exit
	nop

flash_jedec_page_program_failed:
	la      v0, INRAM_FUNCTION_FAILED

flash_jedec_page_program_exit:
	mtc0    t0, C0_SR
	jr      ra
	nop

	.set reorder
	.endfunc

/* ** ** ** ** ** ** ** ** ** ** ** ** */
/* ** ** ** ** ** ** ** ** ** ** ** ** */

	.globl flash_jedec_byte_program
	.func flash_jedec_byte_program
flash_jedec_byte_program:
	mfc0    t0, C0_SR
	and     t2, t0, 0xfffffffe
	mtc0    t2, C0_SR
	la      t1, 0x10000
	addu    t1, a0, t1
	move    t6, ra
	la      t7, flash_get_post_page_programming_wait
	jalr    t7
	nop
	move    ra, t6 
	move    t8, v0

flash_jedec_byte_program_prepare:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0xa0

	*+xxxx = byte 
*/
	la      t3, 0x00aa00aa
	sw      t3, 0x5554(t1)
	la      t4, 0x00550055
	sw      t4, -0x5558(t1)
	la      t5, 0x00a000a0
	sw      t5, 0x5554(t1)

flash_jedec_byte_program_do:
	lw      t7, 0x0000(a2)
	sw      t7, 0x0000(a1)

	la      t5, 0x00800080
flash_jedec_byte_program_wait:
	lw      t6, 0x0000(a1)

	and     t8, t5, t7
	and     t9, t5, t6
	xor     t7, t8, t9
	beqz    t7, flash_jedec_byte_program_next
	nop
	la      t5, 0x00200020
	and     t7, t5, t6
	bne     t7, t5, flash_jedec_byte_program_wait
	nop
	la      t5, 0x00800080
	lw      t6, 0x0000(a1)
	and     t7, t5, t6
	xor     t7, t8, t7
	bnez    t7, flash_jedec_byte_program_failed
	nop

flash_jedec_byte_program_next:
	addi    a1, 4
	addi    a2, 4
	addi    a3, -4
	bnez    t6, flash_jedec_byte_program_prepare
	nop
	la      v0, INRAM_FUNCTION_SUCCEEDED
	b       flash_jedec_byte_program_exit
	nop

flash_jedec_byte_program_failed:
	la      v0, INRAM_FUNCTION_FAILED

flash_jedec_byte_program_exit:
	mtc0    t0, C0_SR
	jr      ra
	nop

	.set reorder
	.endfunc