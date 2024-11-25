/*
 * idtcpu-regs.S -- IDT MIPS register names.
 *
 * This is the regs.S file from Cygnus Support with IDT defines added by Eric MacDonald
 *
 * regs.S is copyright (c) 1995 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

/* Standard MIPS register names: */
#define zero $0
#define z0 $0
#define _at $1
#define v0 $2
#define v1 $3
#define a0 $4
#define a1 $5
#define a2 $6
#define a3 $7
#define t0 $8
#define t1 $9
#define t2 $10
#define t3 $11
#define t4 $12
#define t5 $13
#define t6 $14
#define t7 $15
#define s0 $16
#define s1 $17
#define s2 $18
#define s3 $19
#define s4 $20
#define s5 $21
#define s6 $22
#define s7 $23
#define t8 $24
#define t9 $25
#define k0 $26 /* kernel private register 0 */
#define k1 $27 /* kernel private register 1 */
#define gp $28 /* global data pointer */
#define sp $29 /* stack-pointer */
#define fp $30 /* frame-pointer */
#define ra $31 /* return address */
#define pc $pc /* pc, used on mips16 */

#define fp0 $f0
#define fp1 $f1

/* Useful memory constants: */
#define K0BASE  0x80000000
#ifndef __mips64
#define K1BASE  0xA0000000
#define K0BASE_ADDR ((char *)K0BASE)
#define K1BASE_ADDR ((char *)K1BASE)
#else
#define K1BASE  0xFFFFFFFFA0000000LL
#define K0BASE_ADDR ((char *)0xFFFFFFFF80000000LL)
#define K1BASE_ADDR ((char *)K1BASE)
#endif

#define PHYS_TO_K1(a)   ((unsigned)(a) | K1BASE)

/* Standard Co-Processor 0 registers */
#define C0_COUNT $9  /* Timer Count Register */
#define C0_COMPARE $11  /* Timer Compare Register */
#define C0_SR  $12  /* Status Register */
#define C0_CAUSE $13  /* last exception description */
#define C0_EPC  $14  /* Exception error address */
#define C0_PRID  $15  /* Processor Revision ID */
#define C0_CONFIG $16  /* CPU configuration */
#define C0_WATCHLO $18  /* Watchpoint */

/* Standard Processor Revision ID Register field offsets */
#define PR_IMP 8

/* Standard Config Register field offsets */
#define CR_DB 4
#define CR_IB 5
#define CR_DC 6 /* NOTE v4121 semantics != 43,5xxx semantics */
#define CR_IC 9 /* NOTE v4121 semantics != 43,5xxx semantics */
#define CR_SC 17
#define CR_SS 20
#define CR_SB 22


/* Standard Status Register bitmasks: */
#define SR_CU1  0x20000000 /* Mark CP1 as usable */
#define SR_FR  0x04000000 /* Enable MIPS III FP registers */
#define SR_BEV  0x00400000 /* Controls location of exception vectors */
#define SR_PE  0x00100000 /* Mark soft reset (clear parity error) */

#define SR_KX  0x00000080 /* Kernel extended addressing enabled */
#define SR_SX  0x00000040 /* Supervisor extended addressing enabled */
#define SR_UX  0x00000020 /* User extended addressing enabled */

#define SR_ERL      0x00000004  /* Error level */
#define SR_EXL      0x00000002  /* Exception level */
#define SR_IE       0x00000001  /* Interrupts enabled */

/* Standard Cause Register bitmasks: */
#define CAUSE_EXC_MASK             (0x1F << 2)
#define CAUSE_EXC_SYSCALL          (8    << 2)
#define CAUSE_EXC_BREAKPOINT       (9    << 2)
#define CAUSE_EXC_COPROCESSOR      (11   << 2)

/* Standard (R4000) cache operations. Taken from "MIPS R4000
   Microprocessor User's Manual" 2nd edition: */

#define CACHE_I  (0) /* primary instruction */
#define CACHE_D  (1) /* primary data */
#define CACHE_SI (2) /* secondary instruction */
#define CACHE_SD (3) /* secondary data (or combined instruction/data) */

#define INDEX_INVALIDATE  (0) /* also encodes WRITEBACK if CACHE_D or CACHE_SD */
#define INDEX_LOAD_TAG   (1)
#define INDEX_STORE_TAG   (2)
#define CREATE_DIRTY_EXCLUSIVE  (3) /* CACHE_D and CACHE_SD only */
#define HIT_INVALIDATE   (4)
#define CACHE_FILL   (5) /* CACHE_I only */
#define HIT_WRITEBACK_INVALIDATE (5) /* CACHE_D and CACHE_SD only */
#define HIT_WRITEBACK   (6) /* CACHE_I, CACHE_D and CACHE_SD only */
#define HIT_SET_VIRTUAL   (7) /* CACHE_SI and CACHE_SD only */

#define BUILD_CACHE_OP(o,c)  (((o) << 2) | (c))

/* Individual cache operations: */
#define INDEX_INVALIDATE_I  BUILD_CACHE_OP(INDEX_INVALIDATE,CACHE_I)
#define INDEX_WRITEBACK_INVALIDATE_D BUILD_CACHE_OP(INDEX_INVALIDATE,CACHE_D)
#define INDEX_INVALIDATE_SI             BUILD_CACHE_OP(INDEX_INVALIDATE,CACHE_SI)
#define INDEX_WRITEBACK_INVALIDATE_SD BUILD_CACHE_OP(INDEX_INVALIDATE,CACHE_SD)

#define INDEX_LOAD_TAG_I  BUILD_CACHE_OP(INDEX_LOAD_TAG,CACHE_I)
#define INDEX_LOAD_TAG_D                BUILD_CACHE_OP(INDEX_LOAD_TAG,CACHE_D)
#define INDEX_LOAD_TAG_SI               BUILD_CACHE_OP(INDEX_LOAD_TAG,CACHE_SI)
#define INDEX_LOAD_TAG_SD               BUILD_CACHE_OP(INDEX_LOAD_TAG,CACHE_SD)

#define INDEX_STORE_TAG_I               BUILD_CACHE_OP(INDEX_STORE_TAG,CACHE_I)
#define INDEX_STORE_TAG_D               BUILD_CACHE_OP(INDEX_STORE_TAG,CACHE_D)
#define INDEX_STORE_TAG_SI              BUILD_CACHE_OP(INDEX_STORE_TAG,CACHE_SI)
#define INDEX_STORE_TAG_SD              BUILD_CACHE_OP(INDEX_STORE_TAG,CACHE_SD)

#define CREATE_DIRTY_EXCLUSIVE_D        BUILD_CACHE_OP(CREATE_DIRTY_EXCLUSIVE,CACHE_D)
#define CREATE_DIRTY_EXCLUSIVE_SD       BUILD_CACHE_OP(CREATE_DIRTY_EXCLUSIVE,CACHE_SD)

#define HIT_INVALIDATE_I                BUILD_CACHE_OP(HIT_INVALIDATE,CACHE_I)
#define HIT_INVALIDATE_D                BUILD_CACHE_OP(HIT_INVALIDATE,CACHE_D)
#define HIT_INVALIDATE_SI               BUILD_CACHE_OP(HIT_INVALIDATE,CACHE_SI)
#define HIT_INVALIDATE_SD               BUILD_CACHE_OP(HIT_INVALIDATE,CACHE_SD)

#define CACHE_FILL_I                    BUILD_CACHE_OP(CACHE_FILL,CACHE_I)
#define HIT_WRITEBACK_INVALIDATE_D      BUILD_CACHE_OP(HIT_WRITEBACK_INVALIDATE,CACHE_D)
#define HIT_WRITEBACK_INVALIDATE_SD     BUILD_CACHE_OP(HIT_WRITEBACK_INVALIDATE,CACHE_SD)

#define HIT_WRITEBACK_I                 BUILD_CACHE_OP(HIT_WRITEBACK,CACHE_I)
#define HIT_WRITEBACK_D                 BUILD_CACHE_OP(HIT_WRITEBACK,CACHE_D)
#define HIT_WRITEBACK_SD                BUILD_CACHE_OP(HIT_WRITEBACK,CACHE_SD)

#define HIT_SET_VIRTUAL_SI  BUILD_CACHE_OP(HIT_SET_VIRTUAL,CACHE_SI)
#define HIT_SET_VIRTUAL_SD              BUILD_CACHE_OP(HIT_SET_VIRTUAL,CACHE_SD)

/* OLD > EOF regs.S < OLD */


/* 
 * Added IDT MIPS processor defines by Eric MacDonald
 * Useful for WebTV embedded development
 *
 *
 * Most of these come from various r4000 header files from the mid-90s.
 * With some I've added myself.
 *
 */

/*
 * Processor ID bits
 */

#define PRID_R3000A 0x04
#define PRID_R36100 0x07
#define PRID_R4600  0x20
#define PRID_R4700  0x21
#define PRID_R4650  0x22

/*
 * Exception Vectors
 */

#define C_K0_OFFSET 0x100                /* Cache Error Exception Vector */
#define C_VEC       (K0BASE+C_K0_OFFSET)
#define E_K0_OFFSET 0x180                /* General Exception Vector */
#define E_VEC       (K0BASE+E_K0_OFFSET)
#define I_K0_OFFSET 0x200                /* Interrupt Vector (if Cause.IV = 1) */
#define I_VEC       (K0BASE+I_K0_OFFSET)
#define UT_VEC      K0BASE               /* utlbmiss vector */
#define R_VEC       (K1BASE+0x1fc00000)  /* reset vector */

/*
 * Address conversion macros
 */

#define K0_TO_K1(x)   ((unsigned)(x)|K1BASE)     /* kseg0 to kseg1 */
#define K1_TO_K0(x)   ((unsigned)(x)&0x9FFFFFFF) /* kseg1 to kseg0 */
#define K0_TO_PHYS(x) ((unsigned)(x)&0x1FFFFFFF) /* kseg0 to physical */
#define K1_TO_PHYS(x) ((unsigned)(x)&0x1FFFFFFF) /* kseg1 to physical */
#define PHYS_TO_K0(x) ((unsigned)(x)|K0BASE)     /* physical to kseg0 */

/*
 * Cache size constants
 */

#define MINCACHE 0x800   /* 2*1024     2k   */
#define MAXCACHE 0x40000 /* 256*1024   256k */ 

/*
 * TLB resourse defines
 */

#define N_TLB_ENTRIES     64
#define TLB_PGSIZE        0x1000
#define RANDBASE          8
#define TLBLO_PFNMASK     0xfffff000
#define TLBLO_PFNSHIFT    12
#define TLBLO_N           0x800      /* non-cacheable */
#define TLBLO_D           0x400      /* writeable */
#define TLBLO_V           0x200      /* valid bit */
#define TLBLO_G           0x100      /* global access bit */

#define TLBHI_VPNMASK     0xfffff000
#define TLBHI_VPNSHIFT    12
#define TLBHI_PIDMASK     0xfc0
#define TLBHI_PIDSHIFT    6
#define TLBHI_NPID        64

#define TLBINX_PROBE      0x80000000
#define TLBINX_INXMASK    0x00003f00
#define TLBINX_INXSHIFT   8

#define TLBRAND_RANDMASK  0x00003f00
#define TLBRAND_RANDSHIFT 8

#define TLBCTXT_BASEMASK  0xffe00000
#define TLBCTXT_BASESHIFT 21

#define TLBCTXT_VPNMASK   0x001ffffc
#define TLBCTXT_VPNSHIFT  2

#define SR_CUMASK         0xf0000000 /* coproc usable bits */
#define SR_CU3            0x80000000 /* Coprocessor 3 usable */
#define SR_CU2            0x40000000 /* Coprocessor 2 usable */
#define SR_CU1            0x20000000 /* Coprocessor 1 usable */
#define SR_CU0            0x10000000 /* Coprocessor 0 usable */

#define SR_BEV            0x00400000 /* use boot exception vectors */

/*
 * Cache control bits
 */

#define SR_TS  0x00200000 /* TLB shutdown */
#define SR_PE  0x00100000 /* cache parity error */
#define SR_CM  0x00080000 /* cache miss */
#define SR_PZ  0x00040000 /* cache parity zero */
#define SR_SWC 0x00020000 /* swap cache */
#define SR_ISC 0x00010000 /* Isolate data cache */

/*
 * Status register interrupt masks and bits
 */

#define SR_IMASK      0x0000ff00 /* Interrupt mask */
#define SR_IMASK8     0x00000000 /* mask level 8 */
#define SR_IMASK7     0x00008000 /* mask level 7 */
#define SR_IMASK6     0x0000c000 /* mask level 6 */
#define SR_IMASK5     0x0000e000 /* mask level 5 */
#define SR_IMASK4     0x0000f000 /* mask level 4 */
#define SR_IMASK3     0x0000f800 /* mask level 3 */
#define SR_IMASK2     0x0000fc00 /* mask level 2 */
#define SR_IMASK1     0x0000fe00 /* mask level 1 */
#define SR_IMASK0     0x0000ff00 /* mask level 0 */

#define SR_IBIT8      0x00008000 /* bit level 8 */
#define SR_IBIT7      0x00004000 /* bit level 7 */
#define SR_IBIT6      0x00002000 /* bit level 6 */
#define SR_IBIT5      0x00001000 /* bit level 5 */
#define SR_IBIT4      0x00000800 /* bit level 4 */
#define SR_IBIT3      0x00000400 /* bit level 3 */
#define SR_IBIT2      0x00000200 /* bit level 2 */
#define SR_IBIT1      0x00000100 /* bit level 1 */

#define SR_KUO        0x00000020 /* old kernel/user, 0 => k, 1 => u */
#define SR_IEO        0x00000010 /* old interrupt enable, 1 => enable */
#define SR_KUP        0x00000008 /* prev kernel/user, 0 => k, 1 => u */
#define SR_IEP        0x00000004 /* prev interrupt enable, 1 => enable */
#define SR_KUC        0x00000002 /* cur kernel/user, 0 => k, 1 => u */
#define SR_IEC        0x00000001 /* cur interrupt enable, 1 => enable */

#define SR_IMASKSHIFT 8

/*
 * Cause Register
 */

#define CAUSE_BD       0x80000000 /* Branch delay slot */
#define CAUSE_CEMASK   0x30000000 /* coprocessor error */
#define CAUSE_CESHIFT  28

#define CAUSE_IPMASK   0x0000FF00 /* Pending interrupt mask */
#define CAUSE_IPSHIFT  8

#define CAUSE_EXCMASK  0x0000003C /* Cause code bits */
#define CAUSE_EXCSHIFT 2

#define CAUSE_IV       0x00800000 /* 4640 Interrupt Vector bit */

/*
 * Coprocessor 0 registers
 */

#define C0_IBASE    $0
#define C0_IBOUND   $1
#define C0_DBASE    $2
#define C0_DBOUND   $3

#define C0_BADVADDR $8  /* bad virtual address */

#define C0_COUNT    $9
#define C0_COMPARE  $11

#define C0_SR       $12 /* status register */
#define C0_CAUSE    $13 /* exception cause */
#define C0_EPC      $14 /* exception pc */
#define C0_PRID     $15 /* revision identifier */

#define C0_CONFIG   $16

#define C0_CALG     $17

#define C0_IWATCH   $18
#define C0_DWATCH   $19

#define C0_ECC      $26

#define C0_CACHEERR $27
#define C0_TAGLO    $28

#define C0_ERROREPC $30

/* Register saving used in exceptions and interrupts */

#define	R_R1		0
#define	R_R2		1
#define	R_R3		2
#define	R_R4		3
#define	R_R5		4
#define	R_R6		5
#define	R_R7		6
#define	R_R8		7
#define	R_R9		8
#define	R_R10		9
#define	R_R11		10
#define	R_R12		11
#define	R_R13		12
#define	R_R14		13
#define	R_R15		14
#define	R_R16		15
#define	R_R17		16
#define	R_R18		17
#define	R_R19		18
#define	R_R20		19
#define	R_R21		20
#define	R_R22		21
#define	R_R23		22
#define	R_R24		23
#define	R_R25		24
#define	R_R26		25
#define	R_R27		26
#define	R_R28		27
#define	R_R29		28
#define	R_R30		29
#define	R_R31		30

#define	R_IBASE		31
#define	R_IBOUND	32
#define	R_DBASE		33
#define	R_DBOUND	34
#define	R_BADVADDR	35
#define	R_COUNT		36
#define	R_COMPARE	37
#define	R_SR		38
#define	R_CAUSE		39
#define	R_EPC		40
#define	R_PRID		41
#define	R_CONFIG	42
#define	R_CALG		43
#define	R_IWATCH	44
#define	R_DWATCH	45
#define	R_ECC		46
#define	R_CACHEERR	47
#define	R_TAGLO		48
#define	R_ERROREPC	49

#define	NREGS		50

#define	R_ZERO		R_R0
#define	R_AT		R_R1
#define	R_V0		R_R2
#define	R_V1		R_R3
#define	R_A0		R_R4
#define	R_A1		R_R5
#define	R_A2		R_R6
#define	R_A3		R_R7
#define	R_T0		R_R8
#define	R_T1		R_R9
#define	R_T2		R_R10
#define	R_T3		R_R11
#define	R_T4		R_R12
#define	R_T5		R_R13
#define	R_T6		R_R14
#define	R_T7		R_R15
#define	R_S0		R_R16
#define	R_S1		R_R17
#define	R_S2		R_R18
#define	R_S3		R_R19
#define	R_S4		R_R20
#define	R_S5		R_R21
#define	R_S6		R_R22
#define	R_S7		R_R23
#define	R_T8		R_R24
#define	R_T9		R_R25
#define	R_K0		R_R26
#define	R_K1		R_R27
#define	R_GP		R_R28
#define	R_SP		R_R29
#define	R_FP		R_R30
#define	R_RA		R_R31

#define STACK_GPR      32
#define EXCEPTION_STACK_SIZE (STACK_GPR + 280)
#define STACK_HI     (STACK_GPR + (32 * 4))
#define STACK_LO     (STACK_HI+4)
#define STACK_SR     (STACK_LO+4)
#define STACK_CR     (STACK_SR+4)
#define STACK_EPC    (STACK_CR+4)
#define STACK_FC31   (STACK_EPC+4)
#define STACK_FPR    (STACK_FC31+4)
