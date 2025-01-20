#include "../idtcpu-regs.S"

	.section .text.flash_inram_functions
	.set noreorder

	.globl flash_get_device_id
	.func flash_get_device_id
flash_get_device_id:
	mfc0  t0, C0_SR
	and   t1, t0, 0xFFFFFFFE
	mtc0  t1, C0_SR
	la    a1, 0x10000
	addu  a1, a0, a1
	lw    t1, 0x0000(a0)

flash_check_maskrom_id:
	la    t4, 0x90909090
	sw    t4, 0x0000(a0)
	lw    t5, 0x0000(a0)
	bne   t1, t5, flash_found_id
	nop

flash_check_flashrom_id:
	la    t2, 0x00AA00AA
	sw    t2, 0x5554(a1)
	la    t3, 0x00550055
	sw    t3, -0x5558(a1)
	la    t4, 0x00900090
	sw    t4, 0x5554(a1)
	lw    t5, 0x0000(a0)
	beq   t1, t5, flash_unknown_identity
	nop

flash_found_id:
	lw    v0, 0x0000(a0)
	sll   v0, v0, 0x10
	lw    v1, 0x0004(a0)
	and   v1, 0xffff
	or    v0, v1
	la    t4, 0x00f000f0
	sw    t4, 0x0000(a0)
	lw    t5, 0x0000(a0)
	beq   t1, t5, flash_get_device_id_exit
	nop

flash_hard_reset:
	sw    t2, 0x5554(a1)
	sw    t3, -0x5558(a1)
	sw    t4, 0x5554(a1)
	b     flash_get_device_id_exit
	nop

flash_unknown_identity:
	la    v0, 0x00000000

flash_get_device_id_exit:
	mtc0  t0, C0_SR
	jr    ra
	nop

	.set reorder
	.endfunc