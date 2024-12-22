#include "idtcpu-regs.S"

#define saved_cause t8

	.global exception_handler
	.global exception_handler_end

	.func exception_handler
exception_handler:
	.set noreorder
	.set noat

	addiu sp, -EXCEPTION_STACK_SIZE

	sw    _at, (STACK_GPR + (R_AT * 4))(sp) # AT
	.set at
	sw    v0, (STACK_GPR + (R_V0 * 4))(sp) # V0
	sw    v1, (STACK_GPR + (R_V1 * 4))(sp) # V1
	sw    a0, (STACK_GPR + (R_A0 * 4))(sp) # A0
	sw    a1, (STACK_GPR + (R_A1 * 4))(sp) # A1
	sw    a2, (STACK_GPR + (R_A2 * 4))(sp) # A2
	sw    a3, (STACK_GPR + (R_A3 * 4))(sp) # A3
	sw    t0, (STACK_GPR + (R_T0 * 4))(sp) # T0
	sw    t1, (STACK_GPR + (R_T1 * 4))(sp) # T1
	sw    t2, (STACK_GPR + (R_T2 * 4))(sp) # T2
	sw    t3, (STACK_GPR + (R_T3 * 4))(sp) # T3
	sw    t4, (STACK_GPR + (R_T4 * 4))(sp) # T4
	sw    t5, (STACK_GPR + (R_T5 * 4))(sp) # T5
	sw    t6, (STACK_GPR + (R_T6 * 4))(sp) # T6
	sw    t7, (STACK_GPR + (R_T7 * 4))(sp) # T7
	sw    t8, (STACK_GPR + (R_T8 * 4))(sp) # T8
	sw    t9, (STACK_GPR + (R_T9 * 4))(sp) # T9
	sw    ra, (STACK_GPR + (R_RA * 4))(sp) # RA

	mflo  k0
	mfhi  k1
	sw    k0, (STACK_LO)(sp)
	sw    k1, (STACK_HI)(sp)

	mfc0  k0, C0_EPC
	mfc0  k1, C0_SR
	sw    k0, (STACK_EPC)(sp)
	sw    k1, (STACK_SR)(sp)

	and   k1, ~(SR_IE | SR_EXL)
	mtc0  k1, C0_SR

	mfc0  saved_cause, C0_CAUSE
	sw    t8, (STACK_CR)(sp)
	andi  t0, t8, 0xff

	beqz  t0, interrupt_exception
	nop

noninterrupt_exception:
	sw    s0, (STACK_GPR + (R_S0 * 4))(sp) # S0
	sw    s1, (STACK_GPR + (R_S1 * 4))(sp) # S1
	sw    s2, (STACK_GPR + (R_S2 * 4))(sp) # S2
	sw    s3, (STACK_GPR + (R_S3 * 4))(sp) # S3
	sw    s4, (STACK_GPR + (R_S4 * 4))(sp) # S4
	sw    s5, (STACK_GPR + (R_S5 * 4))(sp) # S5
	sw    s6, (STACK_GPR + (R_S6 * 4))(sp) # S6
	sw    s7, (STACK_GPR + (R_S7 * 4))(sp) # S7
	sw    k0, (STACK_GPR + (R_K0 * 4))(sp) # K0
	sw    k1, (STACK_GPR + (R_K1 * 4))(sp) # K1
	sw    gp, (STACK_GPR + (R_GP * 4))(sp) # GP
	sw    sp, (STACK_GPR + (R_SP * 4))(sp) # SP
	sw    fp, (STACK_GPR + (R_FP * 4))(sp) # FP
	jal   __onCriticalException
	addiu a0, sp, 32
	j     return_from_exception
	nop

interrupt_exception:
	mfc0  t0, C0_SR
	and   t0, ~SR_CU1
	mtc0  t0, C0_SR
	sw    sp, interrupt_exception_frame
	and   t0, saved_cause, 0x8000
	beqz  t0, check_system_asic_interrupt

cpu_timer_interrupt:
	mfc0  t0,C0_COMPARE
	mtc0  t0,C0_COMPARE
	jal   __TI_handler
	nop
	j     return_from_exception
	nop

check_system_asic_interrupt:
	la    a0, 0xa4000000
	lw    t0, 0(a0)
	and   t0, t0, 0x03000000
	beq   t0, 0x03000000, check_solo_interrupt
	nop

check_spot_interrupt:
	lw    v1, 0xa4000008
	and   v1, v1, 0xfc
	la    v0, dispatch_spot_interrupt_table
	addu  v0, v0, v1
	lw    v0, 0(v0)
	nop
	jr    v0
	nop

check_solo_interrupt:
	lw    v1, 0xa4000008
recheck_solo_interrupt:
	and   v1, v1, 0xfc
	la    v0, root_solo_interrupt_table
	addu  v0, v0, v1
	lw    v0, 0(v0)
	nop
	jr    v0
	nop

solo_fence_interrupt:
	la    a0, 0x00000004
	la    a1, __FENCE_handler
	j     dispatch_bus_interrupt
	nop

solo_timer_interrupt:
	la    a0, 0x00000008
	la    a1, __SOLO_TI_handler
	j     dispatch_bus_interrupt
	nop

solo_rdev_interrupt:
	lw    a1, 0xa400008c
	and   a2, a1, 0x4
	beq   a2, 0x4, solo_rdev_slot0_interrupt
	nop
	and   v1, v1, 0x5c
	bne   v1, 0x0, recheck_solo_interrupt
	nop
	la    v0, dispatch_solo_rdev_interrupt_table
	addu  v0, v0, a1
	lw    v0, 0(v0)
	nop
	jr    v0
	nop

solo_rdev_slot0_interrupt:
	la    a0, 0x00000004
	la    a1, 0xa400018c
	la    a2, __RIO_SLOT0_handler
	j     dispatch_bus_subinterrupt
	nop

solo_rdev_slot1_interrupt:
	la    a0, 0x00000008
	la    a1, 0xa400018c
	la    a2, __RIO_SLOT1_handler
	j     dispatch_bus_subinterrupt
	nop

solo_rdev_slot2_interrupt:
	la    a0, 0x00000010
	la    a1, 0xa400018c
	la    a2, __RIO_SLOT2_handler
	j     dispatch_bus_subinterrupt
	nop


solo_rdev_slot3_interrupt:
	la    a0, 0x00000020
	la    a1, 0xa400018c
	la    a2, __RIO_SLOT3_handler
	j     dispatch_bus_subinterrupt
	nop

solo_audio_interrupt:
	lw    a1, 0xa4000068
	and   a2, a1, 0x7c
	la    v0, dispatch_solo_audio_interrupt_table
	addu  v0, v0, a1
	lw    v0, 0(v0)
	nop
	jr    v0
	nop

solo_audio_smodem_in_interrupt:
	la    a0, 0x00000040
	la    a1, 0xa4000168
	la    a2, __AUDIO_SMODEM_IN_handler
	j     dispatch_bus_subinterrupt
	nop


solo_audio_smodem_out_interrupt:
	la    a0, 0x00000020
	la    a1, 0xa4000168
	la    a2, __AUDIO_SMODEM_OUT_handler
	j     dispatch_bus_subinterrupt
	nop

solo_audio_divunit_interrupt:
	la    a0, 0x00000010
	la    a1, 0xa4000168
	la    a2, __AUDIO_DIVUNIT_handler
	j     dispatch_bus_subinterrupt
	nop


solo_audio_in_interrupt:
	la    a0, 0x00000008
	la    a1, 0xa4000168
	la    a2, __AUDIO_IN_handler
	j     dispatch_bus_subinterrupt
	nop


solo_audio_out_interrupt:
	la    a0, 0x00000004
	la    a1, 0xa4000168
	la    a2, __AUDIO_OUT_handler
	j     dispatch_bus_subinterrupt
	nop

solo_sdev_interrupt:
	lw    a1, 0xa4000074
	and   a2, a1, 0xfc
	la    v0, dispatch_solo_sdev_interrupt_table
	addu  v0, v0, a1
	lw    v0, 0(v0)
	nop
	jr    v0
	nop

solo_sdev_gpio_interrupt:
	la    a0, 0x00000080
	la    a1, 0xa4000174
	la    a2, __DEV_GPIO_handler
	j     dispatch_bus_subinterrupt
	nop

solo_sdev_uart_interrupt:
	la    a0, 0x00000040
	la    a1, 0xa4000174
	la    a2, __DEV_UART_handler
	j     dispatch_bus_subinterrupt
	nop

solo_sdev_smartcard_interrupt:
	la    a0, 0x00000020
	la    a1, 0xa4000174
	la    a2, __DEV_SMARTCARD_handler
	j     dispatch_bus_subinterrupt
	nop

solo_sdev_pport_interrupt:
	la    a0, 0x00000010
	la    a1, 0xa4000174
	la    a2, __DEV_PPORT_handler
	j     dispatch_bus_subinterrupt
	nop

solo_sdev_ir_out_interrupt:
	la    a0, 0x00000008
	la    a1, 0xa4000174
	la    a2, __DEV_IR_OUT_handler
	j     dispatch_bus_subinterrupt
	nop


solo_sdev_ir_in_interrupt:
	la    a0, 0x00000004
	la    a1, 0xa4000174
	la    a2, __DEV_IR_IN_handler
	j     dispatch_bus_subinterrupt
	nop

solo_video_interrupt:
	lw    a1, 0xa4000080
	and   a2, a1, 0x3c
	la    v0, dispatch_solo_video_interrupt_table
	addu  v0, v0, a1
	lw    v0, 0(v0)
	nop
	jr    v0
	nop

solo_video_divunit_interrupt:
	la    a0, 0x00000020
	la    a1, 0xa4000180
	la    a2, __VIDEO_DIVUNIT_handler
	j     dispatch_bus_subinterrupt
	nop

solo_video_gfxunit_interrupt:
	la    a0, 0x00000010
	la    a1, 0xa4000180
	la    a2, __VIDEO_GFXUNIT_handler
	lw    t3, 0xa4006068
	la    t4, 0xa400606c
	j     dispatch_bus_subinterrupt_with_status
	nop

solo_video_potunit_interrupt:
	la    a0, 0x00000008
	la    a1, 0xa4000180
	la    a2, __VIDEO_POTUNIT_handler
	lw    t3, 0xa40090a0
	la    t4, 0xa40090a8
	j     dispatch_bus_subinterrupt_with_status
	nop

solo_video_vidunit_interrupt:
	la    a0, 0x00000004
	la    a1, 0xa4000180
	la    a2, __VIDEO_VIDUNIT_handler
	lw    t3, 0xa4003038
	la    t4, 0xa4003138
	j     dispatch_bus_subinterrupt_with_status
	nop

spot_hwmodem_interrupt:
	la    a0, 0x00000020
	la    a1, __RIO_SLOT0_handler
	j     dispatch_bus_interrupt
	nop

spot_audio_out_interrupt:
	la    a0, 0x00000004
	la    a1, __AUDIO_OUT_handler
	j     dispatch_bus_interrupt
	nop

spot_ir_in_interrupt:
	la    a0, 0x00000010
	la    a1, __DEV_IR_IN_handler
	j     dispatch_bus_interrupt
	nop

spot_ps2_interrupt:
	la    a0, 0x00000040
	la    a1, __DEV_PS2_handler
	j     dispatch_bus_interrupt
	nop

spot_smartcard_interrupt:
	la    a0, 0x00000008
	la    a1, __DEV_SMARTCARD_handler
	j     dispatch_bus_interrupt
	nop

spot_video_interrupt:
	la    a0, 0x00000080
	la    a1, __VIDEO_VIDUNIT_handler
	j     dispatch_bus_interrupt
	nop

dispatch_bus_interrupt:
	sw    a0, 0xa4000108
	jal   a1
	nop
	j     return_from_exception
	nop

dispatch_bus_subinterrupt:
	sw    a0, 0x0(a1)
	jal   a2
	nop
	j     return_from_exception
	nop

dispatch_bus_subinterrupt_with_status:
	sw    a0, 0x0(a1)
	sw    t3, 0x0(t4)
	jal   a2
	move  a0, t3
	j     return_from_exception
	nop

solo_non_wtv_interrupt:
spot_non_wtv_interrupt:
	and   t0, saved_cause, 0x1800000 /* DW or IW exceptions are ignored right now */
	bnez  t0, interrupt_return
	nop
unknown_interrupt:
	break 0x71
	nop
interrupt_return:	
	j     return_from_exception
	nop

return_from_exception:
	.set noat

	lw    t0, (STACK_SR)(sp)
	mtc0  t0, C0_SR

	lw    t0, (STACK_LO)(sp)
	lw    t1, (STACK_HI)(sp)
	mtlo  t0
	mthi  t1

	lw    t2, (STACK_EPC)(sp)
	mtc0  t2, C0_EPC

	lw    t0, (STACK_SR)(sp)
	lw    _at, (STACK_GPR + (R_AT * 4))(sp) # AT
	lw    v0, (STACK_GPR + (R_V0 * 4))(sp) # V0
	lw    v1, (STACK_GPR + (R_V1 * 4))(sp) # V1
	lw    a0, (STACK_GPR + (R_A0 * 4))(sp) # A0
	lw    a1, (STACK_GPR + (R_A1 * 4))(sp) # A1
	lw    a2, (STACK_GPR + (R_A2 * 4))(sp) # A2
	lw    a3, (STACK_GPR + (R_A3 * 4))(sp) # A3
	lw    t0, (STACK_GPR + (R_T0 * 4))(sp) # T0
	lw    t1, (STACK_GPR + (R_T1 * 4))(sp) # T1
	lw    t2, (STACK_GPR + (R_T2 * 4))(sp) # T2
	lw    t3, (STACK_GPR + (R_T3 * 4))(sp) # T3 
	lw    t4, (STACK_GPR + (R_T4 * 4))(sp) # T4
	lw    t5, (STACK_GPR + (R_T5 * 4))(sp) # T5
	lw    t6, (STACK_GPR + (R_T6 * 4))(sp) # T6
	lw    t7, (STACK_GPR + (R_T7 * 4))(sp) # T7
	lw    t8, (STACK_GPR + (R_T8 * 4))(sp) # T8
	lw    t9, (STACK_GPR + (R_T9 * 4))(sp) # T9
	lw    ra, (STACK_GPR + (R_RA * 4))(sp) # RA

	addiu sp, EXCEPTION_STACK_SIZE

	eret

root_solo_interrupt_table:
	/*
		This is a quick lookup table for SOLO-based interrupts.

		Priority:
			Fence error
			Solo timer interrupt (system timer and timeout errors)
			RIO device 0 only (assumed as the hardware modem)
			Audio interrupts
				Softmodem in
				Softmodem out
				divUnit
				Audio output
				Audio input
			SOLO Devices
				IR input
				Parallel port
				SmartCard
				UART
				GPIO
				IR output
			Other RIO devices
				Device 1
				Device 2
				Device 3
			Video interrupts
				gfxUnit
				divUnit
				potUnit
				vidUnit
	*/
	.word solo_non_wtv_interrupt /* 0b000000 xx:                                             */
	.word solo_fence_interrupt   /* 0b000001 xx:                                     + Fence */
	.word solo_timer_interrupt   /* 0b000010 xx:                               Timer         */
	.word solo_fence_interrupt   /* 0b000011 xx:                               Timer + Fence */
	.word solo_sdev_interrupt    /* 0b000100 xx:                        SDev                 */
	.word solo_fence_interrupt   /* 0b000101 xx:                        SDev         + Fence */
	.word solo_timer_interrupt   /* 0b000110 xx:                        SDev + Timer         */
	.word solo_fence_interrupt   /* 0b000111 xx:                        SDev + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b001000 xx:                 RDev                        */
	.word solo_fence_interrupt   /* 0b001001 xx:                 RDev                + Fence */
	.word solo_timer_interrupt   /* 0b001010 xx:                 RDev        + Timer         */
	.word solo_fence_interrupt   /* 0b001011 xx:                 RDev        + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b001100 xx:                 RDev + SDev                 */
	.word solo_fence_interrupt   /* 0b001101 xx:                 RDev + SDev         + Fence */
	.word solo_timer_interrupt   /* 0b001110 xx:                 RDev + SDev + Timer         */
	.word solo_fence_interrupt   /* 0b001111 xx:                 RDev + SDev + Timer + Fence */
	.word solo_audio_interrupt   /* 0b010000 xx:         Audio                               */
	.word solo_fence_interrupt   /* 0b010001 xx:         Audio                       + Fence */
	.word solo_timer_interrupt   /* 0b010010 xx:         Audio               + Timer         */
	.word solo_fence_interrupt   /* 0b010011 xx:         Audio               + Timer + Fence */
	.word solo_audio_interrupt   /* 0b010100 xx:         Audio        + SDev                 */
	.word solo_fence_interrupt   /* 0b010101 xx:         Audio        + SDev         + Fence */
	.word solo_timer_interrupt   /* 0b010110 xx:         Audio        + SDev + Timer         */
	.word solo_fence_interrupt   /* 0b010111 xx:         Audio        + SDev + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b011000 xx:         Audio + RDev                        */
	.word solo_fence_interrupt   /* 0b011001 xx:         Audio + RDev                + Fence */
	.word solo_timer_interrupt   /* 0b011010 xx:         Audio + RDev        + Timer         */
	.word solo_fence_interrupt   /* 0b011011 xx:         Audio + RDev        + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b011100 xx:         Audio + RDev + SDev                 */
	.word solo_fence_interrupt   /* 0b011101 xx:         Audio + RDev + SDev         + Fence */
	.word solo_timer_interrupt   /* 0b011110 xx:         Audio + RDev + SDev + Timer         */
	.word solo_fence_interrupt   /* 0b011111 xx:         Audio + RDev + SDev + Timer + Fence */
	.word solo_video_interrupt   /* 0b100000 xx: Video                                       */
	.word solo_fence_interrupt   /* 0b100001 xx: Video                               + Fence */
	.word solo_timer_interrupt   /* 0b100010 xx: Video                       + Timer         */
	.word solo_fence_interrupt   /* 0b100011 xx: Video                       + Timer + Fence */
	.word solo_sdev_interrupt    /* 0b100100 xx: Video                + SDev                 */
	.word solo_fence_interrupt   /* 0b100101 xx: Video                + SDev         + Fence */
	.word solo_timer_interrupt   /* 0b100110 xx: Video                + SDev + Timer         */
	.word solo_fence_interrupt   /* 0b100111 xx: Video                + SDev + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b101000 xx: Video         + RDev                        */
	.word solo_fence_interrupt   /* 0b101001 xx: Video         + RDev                + Fence */
	.word solo_timer_interrupt   /* 0b101010 xx: Video         + RDev        + Timer         */
	.word solo_fence_interrupt   /* 0b101011 xx: Video         + RDev        + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b101100 xx: Video         + RDev + SDev                 */
	.word solo_fence_interrupt   /* 0b101101 xx: Video         + RDev + SDev         + Fence */
	.word solo_timer_interrupt   /* 0b101110 xx: Video         + RDev + SDev + Timer         */
	.word solo_fence_interrupt   /* 0b101111 xx: Video         + RDev + SDev + Timer + Fence */
	.word solo_audio_interrupt   /* 0b110000 xx: Video + Audio                               */
	.word solo_fence_interrupt   /* 0b110001 xx: Video + Audio                       + Fence */
	.word solo_timer_interrupt   /* 0b110010 xx: Video + Audio               + Timer         */
	.word solo_fence_interrupt   /* 0b110011 xx: Video + Audio               + Timer + Fence */
	.word solo_audio_interrupt   /* 0b110100 xx: Video + Audio        + SDev                 */
	.word solo_fence_interrupt   /* 0b110101 xx: Video + Audio        + SDev         + Fence */
	.word solo_timer_interrupt   /* 0b110110 xx: Video + Audio        + SDev + Timer         */
	.word solo_fence_interrupt   /* 0b110111 xx: Video + Audio        + SDev + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b111000 xx: Video + Audio + RDev                        */
	.word solo_fence_interrupt   /* 0b111001 xx: Video + Audio + RDev                + Fence */
	.word solo_timer_interrupt   /* 0b111010 xx: Video + Audio + RDev        + Timer         */
	.word solo_fence_interrupt   /* 0b111011 xx: Video + Audio + RDev        + Timer + Fence */
	.word solo_rdev_interrupt    /* 0b111100 xx: Video + Audio + RDev + SDev                 */
	.word solo_fence_interrupt   /* 0b111101 xx: Video + Audio + RDev + SDev         + Fence */
	.word solo_timer_interrupt   /* 0b111110 xx: Video + Audio + RDev + SDev + Timer         */
	.word solo_fence_interrupt   /* 0b111111 xx: Video + Audio + RDev + SDev + Timer + Fence */

dispatch_solo_rdev_interrupt_table:
	.word solo_non_wtv_interrupt    /* 0b0000 xx */
	.word solo_rdev_slot0_interrupt /* 0b0001 xx */
	.word solo_rdev_slot1_interrupt /* 0b0010 xx */
	.word solo_rdev_slot0_interrupt /* 0b0011 xx */
	.word solo_rdev_slot2_interrupt /* 0b0100 xx */
	.word solo_rdev_slot0_interrupt /* 0b0101 xx */
	.word solo_rdev_slot1_interrupt /* 0b0110 xx */
	.word solo_rdev_slot0_interrupt /* 0b0111 xx */
	.word solo_rdev_slot3_interrupt /* 0b1000 xx */
	.word solo_rdev_slot0_interrupt /* 0b1001 xx */
	.word solo_rdev_slot1_interrupt /* 0b1010 xx */
	.word solo_rdev_slot0_interrupt /* 0b1011 xx */
	.word solo_rdev_slot2_interrupt /* 0b1100 xx */
	.word solo_rdev_slot0_interrupt /* 0b1101 xx */
	.word solo_rdev_slot1_interrupt /* 0b1110 xx */
	.word solo_rdev_slot0_interrupt /* 0b1111 xx */

dispatch_solo_audio_interrupt_table:
	.word solo_non_wtv_interrupt          /* 0b00000 xx */
	.word solo_audio_out_interrupt        /* 0b00001 xx */
	.word solo_audio_in_interrupt         /* 0b00010 xx */
	.word solo_audio_out_interrupt        /* 0b00011 xx */
	.word solo_audio_divunit_interrupt    /* 0b00100 xx */
	.word solo_audio_divunit_interrupt    /* 0b00101 xx */
	.word solo_audio_divunit_interrupt    /* 0b00110 xx */
	.word solo_audio_divunit_interrupt    /* 0b00111 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01000 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01001 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01010 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01011 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01100 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01101 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01110 xx */
	.word solo_audio_smodem_out_interrupt /* 0b01111 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10000 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10001 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10010 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10011 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10100 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10101 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10110 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b10111 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11000 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11001 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11010 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11011 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11100 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11101 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11110 xx */
	.word solo_audio_smodem_in_interrupt  /* 0b11111 xx */

dispatch_solo_sdev_interrupt_table:
	.word solo_non_wtv_interrupt        /* 0b000000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b000001 xx */
	.word solo_sdev_ir_out_interrupt    /* 0b000010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b000011 xx */
	.word solo_sdev_pport_interrupt     /* 0b000100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b000101 xx */
	.word solo_sdev_pport_interrupt     /* 0b000110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b000111 xx */
	.word solo_sdev_smartcard_interrupt /* 0b001000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b001001 xx */
	.word solo_sdev_smartcard_interrupt /* 0b001010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b001011 xx */
	.word solo_sdev_pport_interrupt     /* 0b001100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b001101 xx */
	.word solo_sdev_pport_interrupt     /* 0b001110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b001111 xx */
	.word solo_sdev_uart_interrupt      /* 0b010000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b010001 xx */
	.word solo_sdev_uart_interrupt      /* 0b010010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b010011 xx */
	.word solo_sdev_pport_interrupt     /* 0b010100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b010101 xx */
	.word solo_sdev_pport_interrupt     /* 0b010110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b010111 xx */
	.word solo_sdev_smartcard_interrupt /* 0b011000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b011001 xx */
	.word solo_sdev_smartcard_interrupt /* 0b011010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b011011 xx */
	.word solo_sdev_pport_interrupt     /* 0b011100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b011101 xx */
	.word solo_sdev_pport_interrupt     /* 0b011110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b011111 xx */
	.word solo_sdev_gpio_interrupt      /* 0b100000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b100001 xx */
	.word solo_sdev_gpio_interrupt      /* 0b100010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b100011 xx */
	.word solo_sdev_pport_interrupt     /* 0b100100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b100101 xx */
	.word solo_sdev_pport_interrupt     /* 0b100110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b100111 xx */
	.word solo_sdev_smartcard_interrupt /* 0b101000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b101001 xx */
	.word solo_sdev_smartcard_interrupt /* 0b101010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b101011 xx */
	.word solo_sdev_pport_interrupt     /* 0b101100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b101101 xx */
	.word solo_sdev_pport_interrupt     /* 0b101110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b101111 xx */
	.word solo_sdev_uart_interrupt      /* 0b110000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b110001 xx */
	.word solo_sdev_uart_interrupt      /* 0b110010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b110011 xx */
	.word solo_sdev_pport_interrupt     /* 0b110100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b110101 xx */
	.word solo_sdev_pport_interrupt     /* 0b110110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b110111 xx */
	.word solo_sdev_smartcard_interrupt /* 0b111000 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b111001 xx */
	.word solo_sdev_smartcard_interrupt /* 0b111010 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b111011 xx */
	.word solo_sdev_pport_interrupt     /* 0b111100 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b111101 xx */
	.word solo_sdev_pport_interrupt     /* 0b111110 xx */
	.word solo_sdev_ir_in_interrupt     /* 0b111111 xx */

dispatch_solo_video_interrupt_table:
	.word solo_non_wtv_interrupt       /* 0b0000 xx */
	.word solo_video_vidunit_interrupt /* 0b0001 xx */
	.word solo_video_potunit_interrupt /* 0b0010 xx */
	.word solo_video_potunit_interrupt /* 0b0011 xx */
	.word solo_video_gfxunit_interrupt /* 0b0100 xx */
	.word solo_video_gfxunit_interrupt /* 0b0101 xx */
	.word solo_video_gfxunit_interrupt /* 0b0110 xx */
	.word solo_video_gfxunit_interrupt /* 0b0111 xx */
	.word solo_video_divunit_interrupt /* 0b1000 xx */
	.word solo_video_divunit_interrupt /* 0b1001 xx */
	.word solo_video_divunit_interrupt /* 0b1010 xx */
	.word solo_video_divunit_interrupt /* 0b1011 xx */
	.word solo_video_gfxunit_interrupt /* 0b1100 xx */
	.word solo_video_gfxunit_interrupt /* 0b1101 xx */
	.word solo_video_gfxunit_interrupt /* 0b1110 xx */
	.word solo_video_gfxunit_interrupt /* 0b1111 xx */

dispatch_spot_interrupt_table:
	/*
		This is a quick lookup table for SPOT-based interrupts.

		Priority:
			Modem
			Audio
			IR in
			PS/2 device (typically a keyboard)
			SmartCard
			Video
	*/
	.word spot_non_wtv_interrupt   /* 0b000000 xx:                                              */
	.word spot_audio_out_interrupt /* 0b000001 xx:                                      + Audio */
	.word spot_smartcard_interrupt /* 0b000010 xx:                            SmartCard         */
	.word spot_audio_out_interrupt /* 0b000011 xx:                            SmartCard + Audio */
	.word spot_ir_in_interrupt     /* 0b000100 xx:                       IR                     */
	.word spot_audio_out_interrupt /* 0b000101 xx:                       IR             + Audio */
	.word spot_ir_in_interrupt     /* 0b000110 xx:                       IR + SmartCard         */
	.word spot_audio_out_interrupt /* 0b000111 xx:                       IR + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b001000 xx:               Modem                          */
	.word spot_hwmodem_interrupt   /* 0b001001 xx:               Modem                  + Audio */
	.word spot_hwmodem_interrupt   /* 0b001010 xx:               Modem      + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b001011 xx:               Modem      + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b001100 xx:               Modem + IR                     */
	.word spot_hwmodem_interrupt   /* 0b001101 xx:               Modem + IR             + Audio */
	.word spot_hwmodem_interrupt   /* 0b001110 xx:               Modem + IR + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b001111 xx:               Modem + IR + SmartCard + Audio */
	.word spot_ps2_interrupt       /* 0b010000 xx:         PS2                                  */
	.word spot_audio_out_interrupt /* 0b010001 xx:         PS2                          + Audio */
	.word spot_ps2_interrupt       /* 0b010010 xx:         PS2              + SmartCard         */
	.word spot_audio_out_interrupt /* 0b010011 xx:         PS2              + SmartCard + Audio */
	.word spot_ir_in_interrupt     /* 0b010100 xx:         PS2         + IR                     */
	.word spot_audio_out_interrupt /* 0b010101 xx:         PS2         + IR             + Audio */
	.word spot_ir_in_interrupt     /* 0b010110 xx:         PS2         + IR + SmartCard         */
	.word spot_audio_out_interrupt /* 0b010111 xx:         PS2         + IR + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b011000 xx:         PS2 + Modem                          */
	.word spot_hwmodem_interrupt   /* 0b011001 xx:         PS2 + Modem                  + Audio */
	.word spot_hwmodem_interrupt   /* 0b011010 xx:         PS2 + Modem      + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b011011 xx:         PS2 + Modem      + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b011100 xx:         PS2 + Modem + IR                     */
	.word spot_hwmodem_interrupt   /* 0b011101 xx:         PS2 + Modem + IR             + Audio */
	.word spot_hwmodem_interrupt   /* 0b011110 xx:         PS2 + Modem + IR + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b011111 xx:         PS2 + Modem + IR + SmartCard + Audio */
	.word spot_video_interrupt     /* 0b100000 xx: Video                                        */
	.word spot_audio_out_interrupt /* 0b100001 xx: Video                                + Audio */
	.word spot_smartcard_interrupt /* 0b100010 xx: Video                    + SmartCard         */
	.word spot_audio_out_interrupt /* 0b100011 xx: Video                    + SmartCard + Audio */
	.word spot_ir_in_interrupt     /* 0b100100 xx: Video               + IR                     */
	.word spot_audio_out_interrupt /* 0b100101 xx: Video               + IR             + Audio */
	.word spot_ir_in_interrupt     /* 0b100110 xx: Video               + IR + SmartCard         */
	.word spot_audio_out_interrupt /* 0b100111 xx: Video               + IR + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b101000 xx: Video       + Modem                          */
	.word spot_hwmodem_interrupt   /* 0b101001 xx: Video       + Modem                  + Audio */
	.word spot_hwmodem_interrupt   /* 0b101010 xx: Video       + Modem      + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b101011 xx: Video       + Modem      + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b101100 xx: Video       + Modem + IR                     */
	.word spot_hwmodem_interrupt   /* 0b101101 xx: Video       + Modem + IR             + Audio */
	.word spot_hwmodem_interrupt   /* 0b101110 xx: Video       + Modem + IR + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b101111 xx: Video       + Modem + IR + SmartCard + Audio */
	.word spot_ps2_interrupt       /* 0b110000 xx: Video + PS2                                  */
	.word spot_audio_out_interrupt /* 0b110001 xx: Video + PS2                          + Audio */
	.word spot_ps2_interrupt       /* 0b110010 xx: Video + PS2              + SmartCard         */
	.word spot_audio_out_interrupt /* 0b110011 xx: Video + PS2              + SmartCard + Audio */
	.word spot_ir_in_interrupt     /* 0b110100 xx: Video + PS2         + IR                     */
	.word spot_audio_out_interrupt /* 0b110101 xx: Video + PS2         + IR             + Audio */
	.word spot_ir_in_interrupt     /* 0b110110 xx: Video + PS2         + IR + SmartCard         */
	.word spot_audio_out_interrupt /* 0b110111 xx: Video + PS2         + IR + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b111000 xx: Video + PS2 + Modem                          */
	.word spot_hwmodem_interrupt   /* 0b111001 xx: Video + PS2 + Modem                  + Audio */
	.word spot_hwmodem_interrupt   /* 0b111010 xx: Video + PS2 + Modem      + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b111011 xx: Video + PS2 + Modem      + SmartCard + Audio */
	.word spot_hwmodem_interrupt   /* 0b111100 xx: Video + PS2 + Modem + IR                     */
	.word spot_hwmodem_interrupt   /* 0b111101 xx: Video + PS2 + Modem + IR             + Audio */
	.word spot_hwmodem_interrupt   /* 0b111110 xx: Video + PS2 + Modem + IR + SmartCard         */
	.word spot_hwmodem_interrupt   /* 0b111111 xx: Video + PS2 + Modem + IR + SmartCard + Audio */


exception_handler_end:
	.endfunc

	.section .bss
	.p2align 2
	.lcomm interrupt_exception_frame, 4
