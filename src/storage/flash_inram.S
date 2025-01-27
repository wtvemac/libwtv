#include "../idtcpu-regs.S"

#define INRAM_FUNCTION_FAILED    0xffffffff
#define INRAM_FUNCTION_SUCCEEDED 0x00000000

	.section .text.flash_inram_functions
	.set noreorder

/* ** ** ** ** ** ** ** ** ** ** ** ** */
/* ** ** ** ** ** ** ** ** ** ** ** ** */

	.globl flash_get_device_id
	.func flash_get_device_id
flash_get_device_id:
	mfc0    t0, C0_SR
	and     t2, t0, 0xFFFFFFFE
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
	la      t3, 0x00AA00AA
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

	.globl flash_mx_erase_sector
	.func flash_mx_erase_sector
flash_mx_erase_sector:
	mfc0    t0, C0_SR
	and     t2, t0, 0xFFFFFFFE
	mtc0    t2, C0_SR
	la      t1, 0x10000
	addu    t1, a0, t1

flash_mx_erase_sector_prepare:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0x80
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+sector = 0x30
*/
	la      t3, 0x00AA00AA
	sw      t3, 0x5554(t1)
	la      t4, 0x00550055
	sw      t4, -0x5558(t1)
	la      t5, 0x00800080
	sw      t5, 0x5554(t1)
	sw      t3, 0x5554(t1)
	sw      t4, -0x5558(t1)

flash_mx_erase_sector_do:
	la      t6, 0x00300030
	sw      t6, 0x0000(a1)

flash_mx_erased_sector_wait:
	lw      t6, 0x0000(a1)
	and     t6, t5, t6
	bne     t6, t5, flash_mx_erased_sector_wait
	nop

flash_mx_erase_sector_reset:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0xf0
*/
	sw      t3, 0x5554(t1)
	sw      t4, -0x5558(t1)
	la      t5, 0x00f000f0
	sw      t5, 0x5554(t1)

flash_mx_erase_sector_exit:
	mtc0    t0, C0_SR
	la      v0, INRAM_FUNCTION_SUCCEEDED
	jr      ra
	nop

	.set reorder
	.endfunc

/* ** ** ** ** ** ** ** ** ** ** ** ** */
/* ** ** ** ** ** ** ** ** ** ** ** ** */

	.globl flash_mx_program
	.func flash_mx_program
flash_mx_program:
	mfc0    t0, C0_SR
	and     t2, t0, 0xFFFFFFFE
	mtc0    t2, C0_SR
	la      t1, 0x10000
	addu    t1, a0, t1
	move    t6, ra
	la      t7, flash_get_page_programming_time
	jalr    t7
	nop
	move    ra, t6 
	move    t8, v0

flash_mx_program_page_prepare:
/*
	*+0x5555 = 0xaa
	*+0x2aaa = 0x55
	*+0x5555 = 0xa0

	*+xxxx = byte 
*/
	la      t3, 0x00AA00AA
	sw      t3, 0x5554(t1)
	la      t4, 0x00550055
	sw      t4, -0x5558(t1)
	la      t5, 0x00a000a0
	sw      t5, 0x5554(t1)
	la      t6, 0x100      /* = 128 bytes per page x 2 stripped flash chips */

flash_mx_program_page_do:
	lw      t7, 0x0000(a2)
	sw      t7, 0x0000(a1)
	addi    a1, 4
	addi    t6, -4
	addi    a2, 4
	bnez    t6, flash_mx_program_page_do
	nop

	mfc0    t6, C0_COUNT
	mtc0    zero, C0_COUNT
flash_mx_program_wait:
	mfc0    t7, C0_COUNT
	slt     t9, t7, t8
	bnez    t9, flash_mx_program_wait
	nop
	mtc0    t6, C0_COUNT

	la      t5, 0x00800080
flash_mx_programmed_wait:
	lw      t6, 0x0000(a0)
	and     t6, t5, t6
	bne     t6, t5, flash_mx_programmed_wait
	nop

flash_mx_program_reset:
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
	bnez    t6, flash_mx_program_failed
	nop

	addi    a3, -0x100
	bnez    a3, flash_mx_program_page_prepare
	nop
	la      v0, INRAM_FUNCTION_SUCCEEDED
	b       flash_mx_program_exit

flash_mx_program_failed:
	la      v0, INRAM_FUNCTION_FAILED

flash_mx_program_exit:
	mtc0    t0, C0_SR
	jr      ra
	nop

	.set reorder
	.endfunc