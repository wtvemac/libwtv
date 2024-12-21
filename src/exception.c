/**
 * @file exception.c
 * @brief Exception Handler
 * @ingroup exceptions
 */
#include "exception.h"
#include "exception_internal.h"
#include "console.h"
#include "wtvsys.h"
#include "debug.h"
#include "regs_internal.h"
#include "libc.h"
#include "wtvsys.h"
#include "serial.h"
#include <string.h>
#include <stdbool.h>
#include <math.h>

/**
 * @brief Syscall exception handler entry
 */
typedef struct {
	/** @brief Exception handler */
	syscall_handler_t handler;
	/** @brief Syscall code range start */
	uint32_t first_code;
	/** @brief Syscall code range end */
	uint32_t last_code;
} syscall_handler_entry_t;

/** @brief Maximum number of syscall handlers that can be registered. */
#define MAX_SYSCALL_HANDLERS 4

/** @brief Unhandled exception handler currently registered with exception system */
static void (*__exception_handler)(exception_t*) = exception_default_handler;
/** @brief Base register offset as defined by the interrupt controller */
extern volatile reg_block_t __baseRegAddr;
/** @brief Syscall exception handlers */
static syscall_handler_entry_t __syscall_handlers[MAX_SYSCALL_HANDLERS];

exception_handler_t register_exception_handler(exception_handler_t cb)
{
	exception_handler_t old = __exception_handler;
	__exception_handler = cb;
	return old;
}


/** 
 * @brief Dump a brief recap of the exception.
 * 
 * @param[in] out File to write to
 * @param[in] ex Exception to dump
 */
void __exception_dump_header(FILE *out, exception_t* ex) {
	uint32_t cr = ex->regs->cr;
	uint32_t fcr31 = ex->regs->fc31;

	//EMAC:fprintf(out, "%s exception at PC:%08x\x0a\x0d", ex->info, (uint32_t)(ex->regs->epc + ((cr & C0_CAUSE_BD) ? 4 : 0)));
	switch (ex->code) {
		case EXCEPTION_CODE_STORE_ADDRESS_ERROR:
		case EXCEPTION_CODE_LOAD_I_ADDRESS_ERROR:
		case EXCEPTION_CODE_TLB_STORE_MISS:
		case EXCEPTION_CODE_TLB_LOAD_I_MISS:
		case EXCEPTION_CODE_I_BUS_ERROR:
		case EXCEPTION_CODE_D_BUS_ERROR:
		case EXCEPTION_CODE_TLB_MODIFICATION: {
			//EMAC:fprintf(out, "Exception address: %08x\x0a\x0d", C0_BADVADDR());
		}

		case EXCEPTION_CODE_FLOATING_POINT: {
			const char *space = "";
			//EMAC:fprintf(out, "FPU status: %08x [", C1_FCR31());
			//EMAC:if (fcr31 & C1_CAUSE_INEXACT_OP) fprintf(out, "%sINEXACT", space), space=" ";
			//EMAC:if (fcr31 & C1_CAUSE_OVERFLOW) fprintf(out, "%sOVERFLOW", space), space=" ";
			//EMAC:if (fcr31 & C1_CAUSE_UNDERFLOW) fprintf(out, "%sUNDERFLOW", space), space=" ";
			//EMAC:if (fcr31 & C1_CAUSE_DIV_BY_0) fprintf(out, "%sDIV0", space), space=" ";
			//EMAC:if (fcr31 & C1_CAUSE_INVALID_OP) fprintf(out, "%sINVALID", space), space=" ";
			//EMAC:if (fcr31 & C1_CAUSE_NOT_IMPLEMENTED) fprintf(out, "%sNOTIMPL", space), space=" ";
			//EMAC:fprintf(out, "]\x0a\x0d");
			break;
		}

		case EXCEPTION_CODE_COPROCESSOR_UNUSABLE:
			//EMAC:fprintf(out, "COP: %ld\x0a\x0d", C0_GET_CAUSE_CE(cr));
			break;

		case EXCEPTION_CODE_WATCH:
			//EMAC:fprintf(out, "Watched address: %08x\x0a\x0d", C0_WATCHLO() & ~3);
			break;

		default:
			break;
	}
}

/**
 * @brief Helper to dump the GPRs of an exception
 * 
 * @param ex 		Exception
 * @param cb 		Callback that will be called for each register
 * @param arg 		Argument to pass to the callback
 */
void __exception_dump_gpr(exception_t* ex, void (*cb)(void *arg, const char *regname, char* value), void *arg) {
	char buf[24];
	for (int i=0;i<34;i++) {
		uint32_t v = (i<32) ? ex->regs->gpr[i] : (i == 33) ? ex->regs->lo : ex->regs->hi;
		snprintf(buf, sizeof(buf), "---- ---- %04x %04x", (v >> 16) & 0xFFFF, v & 0xFFFF);
		//EMAC:cb(arg, __mips_gpr[i], buf);
	}
}

/**
 * @brief Helper to dump the FPRs of an exception
 * 
 * @param ex 		Exception
 * @param cb 		Callback that will be called for each register
 * @param arg 		Argument to pass to the callback
 */
// Make sure that -ffinite-math-only is disabled otherwise the compiler will assume that no NaN/Inf can exist
// and thus __builtin_isnan/__builtin_isinf are folded to false at compile-time.
__attribute__((optimize("no-finite-math-only"), noinline))
void __exception_dump_fpr(exception_t* ex, void (*cb)(void *arg, const char *regname, char* hexvalue, char *singlevalue), void *arg) {
	char hex[32], single[32]; char *singlep;
	for (int i = 0; i<32; i++) {
		uint32_t fpr32 = ex->regs->fpr[i];

		snprintf(hex, sizeof(hex), "%08x", fpr32);

		float f;  memcpy(&f, &fpr32, sizeof(float));

		// Check for denormal on the integer representation. Unfortunately, even
		// fpclassify() generates an unmaskable exception on denormals, so it can't be used.
		// Open GCC bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66462
		if ((fpr32 & 0x7F800000) == 0 && (fpr32 & 0x007FFFFF) != 0)
			singlep = "<Denormal>";
		else if ((fpr32 & 0x7F800000) == 0x7F800000 && (fpr32 & 0x007FFFFF) != 0)
			singlep = "<NaN>";
		else if (__builtin_isinf(f))
			singlep = (f < 0) ? "<-Inf>" : "<+Inf>";
		else
			sprintf(single, "%.12g", f), singlep = single;

		//EMAC:cb(arg, __mips_fpreg[i], hex, singlep);
	}
}

#ifndef NDEBUG
static void debug_exception(exception_t* ex) {
	//EMAC:debugf("\x0a\x0d\x0a\x0d******* CPU EXCEPTION *******\x0a\x0d");
	//EMAC:__exception_dump_header(stderr, ex);

	if (true) {
		int idx = 0;
		void cb(void *arg, const char *regname, char* value) {
			//EMAC:debugf("%s: %s%s", regname, value, ++idx % 4 ? "   " : "\x0a\x0d");
		}
		//EMAC:debugf("GPR:\x0a\x0d"); 
		__exception_dump_gpr(ex, cb, NULL);
		//EMAC:debugf("\x0a\x0d\x0a\x0d");
	}	
	
	if (ex->code == EXCEPTION_CODE_FLOATING_POINT) {
		void cb(void *arg, const char *regname, char* hex, char *singlep) {
			//EMAC:debugf("%4s: %s (%16s)\x0a\x0d", regname, hex, singlep);
		}
		//EMAC:debugf("FPR:\x0a\x0d"); 
		__exception_dump_fpr(ex, cb, NULL);
		//EMAC:debugf("\x0a\x0d");
	}
}
#endif

void exception_default_handler(exception_t* ex) {
	#ifndef NDEBUG
	static bool backtrace_exception = false;

	// Write immediately as much data as we can to the debug spew. This is the
	// "safe" path, because it doesn't involve touching the console drawing code.
	debug_exception(ex);

	// Show a backtrace (starting from just before the exception handler)
	// Avoid recursive exceptions during backtrace printing
	//EMAC:if (backtrace_exception) abort();
	backtrace_exception = true;
	//EMAC:extern void __debug_backtrace(FILE *out, bool skip_exception);
	//EMAC:__debug_backtrace(stderr, true);
	backtrace_exception = false;

	// Run the inspector
	//EMAC:__inspector_exception(ex);
	#endif

	//EMAC:abort();
}

/**
 * @brief Fetch the string name of the exception
 *
 * @param[in] cr
 *            Cause register's value
 *
 * @return String representation of the exception
 */
static const char* __get_exception_name(exception_t *ex)
{
	static const char* exceptionMap[] =
	{
		"Interrupt",								// 0
		"TLB Modification",							// 1
		"TLB Miss (load/instruction fetch)",		// 2
		"TLB Miss (store)",							// 3
		"Address Error (load/instruction fetch)",	// 4
		"Address Error (store)",					// 5
		"Bus Error (instruction fetch)",			// 6
		"Bus Error (data reference: load/store)",	// 7
		"Syscall",									// 8
		"Breakpoint",								// 9
		"Reserved Instruction",						// 10
		"Coprocessor Unusable",						// 11
		"Arithmetic Overflow",						// 12
		"Trap",										// 13
		"Reserved",									// 13
		"Floating-Point",							// 15
		"Reserved",									// 16
		"Reserved",									// 17
		"Reserved",									// 18
		"Reserved",									// 19
		"Reserved",									// 20
		"Reserved",									// 21
		"Reserved",									// 22
		"Watch",									// 23
		"Reserved",									// 24
		"Reserved",									// 25
		"Reserved",									// 26
		"Reserved",									// 27
		"Reserved",									// 28
		"Reserved",									// 29
		"Reserved",									// 30
		"Reserved",									// 31
	};


	// When possible, by peeking into the exception state and COP0 registers
	// we can provide a more detailed exception name.
	uint32_t epc = ex->regs->epc + (ex->regs->cr & C0_CAUSE_BD ? 4 : 0);
	uint32_t badvaddr = C0_BADVADDR();

	switch (ex->code) {
	case EXCEPTION_CODE_FLOATING_POINT:
		if (ex->regs->fc31 & C1_CAUSE_DIV_BY_0) {
			return "Floating point divide by zero";
		} else if (ex->regs->fc31 & C1_CAUSE_INVALID_OP) {
			return "Floating point invalid operation";
		} else if (ex->regs->fc31 & C1_CAUSE_OVERFLOW) {
			return "Floating point overflow";
		} else if (ex->regs->fc31 & C1_CAUSE_UNDERFLOW) {
			return "Floating point underflow";
		} else if (ex->regs->fc31 & C1_CAUSE_INEXACT_OP) {
			return "Floating point inexact operation";
		} else {
			return "Generic floating point";
		}
	case EXCEPTION_CODE_TLB_LOAD_I_MISS:
		if (epc == badvaddr) {
			return "Invalid program counter address";
		} else if (badvaddr < 128) {
			// This is probably a NULL pointer dereference, though it can go through a structure or an array,
			// so leave some margin to the actual faulting address.
			return "NULL pointer dereference (read)";
		} else {
			return "Read from invalid memory address";
		}
	case EXCEPTION_CODE_TLB_STORE_MISS:
		if (badvaddr < 128) {
			return "NULL pointer dereference (write)";
		} else {
			return "Write to invalid memory address";
		}
	case EXCEPTION_CODE_TLB_MODIFICATION:
		return "Write to read-only memory";
	case EXCEPTION_CODE_LOAD_I_ADDRESS_ERROR:
		if (epc == badvaddr) {
			return "Misaligned program counter address";
		} else {
			return "Misaligned read from memory";
		}
	case EXCEPTION_CODE_STORE_ADDRESS_ERROR:
		return "Misaligned write to memory";
	case EXCEPTION_CODE_SYS_CALL:
		return "Unhandled syscall";
	case EXCEPTION_CODE_TRAP: {
		// GCC MIPS generate a trap with code 7 to guard divisions
		uint32_t code = (*(uint32_t*)epc >> 6) & 0x3FF;
		if (code == 7)
			return "Integer divide by zero";
	}	return exceptionMap[ex->code];
	default:
		return exceptionMap[ex->code];
	}
}

/**
 * @brief Fetch relevant registers
 *
 * @param[out] e
 *             Pointer to structure describing the exception
 * @param[in]  type
 *             Exception type.  Either #EXCEPTION_TYPE_CRITICAL or 
 *             #EXCEPTION_TYPE_RESET
 * @param[in]  regs
 *             CPU register status at exception time
 */
static void __fetch_regs(exception_t* e, int32_t type, reg_block_t *regs)
{
	e->regs = regs;
	e->type = type;
	e->code = C0_GET_CAUSE_EXC_CODE(e->regs->cr);
	e->info = __get_exception_name(e);
}

/**
 * @brief Respond to a critical exception
 */
void __onCriticalException(reg_block_t* regs)
{
	// EMAC TODO: need to continue beyond the while(1) to draw a death screen. Possibly a debug prompt like on WebTV's OS.
	if(is_spot_box())
	{
		REGISTER_WRITE(VID_BLNKCOL, 0x2fcffd);
	}
	else
	{
		REGISTER_WRITE(POT_BLNKCOL, 0x2fcffd);
	}

	exception_t e;
	__fetch_regs(&e, EXCEPTION_TYPE_CRITICAL, regs);

	printf("====> OH SHIT, I DIED <====\x0a\x0d");
	printf("ERROR [%08x]: %s\x0a\x0d\x0a\x0d", e.code, e.info);
	printf("Dumppage:\x0a\x0d");
	printf("\tSR:  0x%08x <----\x0a\x0d", e.regs->sr);
	printf("\tCR:  0x%08x <----\x0a\x0d", e.regs->cr);
	printf("\tEPC: 0x%08x <----\x0a\x0d", e.regs->epc);
	printf("\tHI:  0x%08x\x0a\x0d", e.regs->hi);
	printf("\tLO:  0x%08x\x0a\x0d", e.regs->lo);
	printf("\tat:  0x%08x\x0a\x0d", e.regs->gpr[0]);
	printf("\tv0:  0x%08x\x0a\x0d", e.regs->gpr[1]);
	printf("\tv1:  0x%08x\x0a\x0d", e.regs->gpr[2]);
	printf("\ta0:  0x%08x\x0a\x0d", e.regs->gpr[3]);
	printf("\ta1:  0x%08x\x0a\x0d", e.regs->gpr[4]);
	printf("\ta2:  0x%08x\x0a\x0d", e.regs->gpr[5]);
	printf("\ta3:  0x%08x\x0a\x0d", e.regs->gpr[6]);
	printf("\tt0:  0x%08x\x0a\x0d", e.regs->gpr[7]);
	printf("\tt1:  0x%08x\x0a\x0d", e.regs->gpr[8]);
	printf("\tt2:  0x%08x\x0a\x0d", e.regs->gpr[9]);
	printf("\tt3:  0x%08x\x0a\x0d", e.regs->gpr[10]);
	printf("\tt4:  0x%08x\x0a\x0d", e.regs->gpr[11]);
	printf("\tt5:  0x%08x\x0a\x0d", e.regs->gpr[12]);
	printf("\tt6:  0x%08x\x0a\x0d", e.regs->gpr[13]);
	printf("\tt7:  0x%08x\x0a\x0d", e.regs->gpr[14]);
	printf("\tt8:  0x%08x\x0a\x0d", e.regs->gpr[23]);
	printf("\tt9:  0x%08x\x0a\x0d", e.regs->gpr[24]);
	printf("\ts0:  0x%08x\x0a\x0d", e.regs->gpr[15]);
	printf("\ts1:  0x%08x\x0a\x0d", e.regs->gpr[16]);
	printf("\ts2:  0x%08x\x0a\x0d", e.regs->gpr[17]);
	printf("\ts3:  0x%08x\x0a\x0d", e.regs->gpr[18]);
	printf("\ts4:  0x%08x\x0a\x0d", e.regs->gpr[19]);
	printf("\ts5:  0x%08x\x0a\x0d", e.regs->gpr[20]);
	printf("\ts6:  0x%08x\x0a\x0d", e.regs->gpr[21]);
	printf("\ts7:  0x%08x\x0a\x0d", e.regs->gpr[22]);
	printf("\tk0:  0x%08x\x0a\x0d", e.regs->gpr[25]);
	printf("\tk1:  0x%08x\x0a\x0d", e.regs->gpr[26]);
	printf("\tgp:  0x%08x\x0a\x0d", e.regs->gpr[27]);
	printf("\tsp:  0x%08x\x0a\x0d", e.regs->gpr[28]);
	printf("\tfp:  0x%08x\x0a\x0d", e.regs->gpr[29]);
	printf("\tra:  0x%08x <----\x0a\x0d", e.regs->gpr[30]);
	
	debug_backtrace();

	while(1) {}

	if(!__exception_handler) { return; }

	__fetch_regs(&e, EXCEPTION_TYPE_CRITICAL, regs);
	__exception_handler(&e);
}

void register_syscall_handler(syscall_handler_t handler, uint32_t first_code, uint32_t last_code)
{
	//EMAC:assertf(first_code <= 0xFFFFF, "The maximum allowed syscall code is 0xFFFFF (requested: %05x)\x0a\x0d", first_code);
	//EMAC:assertf(last_code <= 0xFFFFF, "The maximum allowed syscall code is 0xFFFFF (requested: %05x)\x0a\x0d", first_code);
	//EMAC:assertf(first_code <= last_code, "Invalid range for syscall handler (first: %05x, last: %05x)\x0a\x0d", first_code, last_code);

	for (int i=0;i<MAX_SYSCALL_HANDLERS;i++)
	{
		if (!__syscall_handlers[i].handler)
		{
			__syscall_handlers[i].first_code = first_code;
			__syscall_handlers[i].last_code = last_code;
			__syscall_handlers[i].handler = handler;
			return;
		}
		else if (__syscall_handlers[i].first_code <= last_code && __syscall_handlers[i].last_code >= first_code)
		{
			//EMAC:assertf(0, "Syscall handler %p already registered for code range %05x - %05x", 
			//	__syscall_handlers[i].handler, __syscall_handlers[i].first_code, __syscall_handlers[i].last_code);
		}
	}
	//EMAC:assertf(0, "Too many syscall handlers\x0a\x0d");
}


/**
 * @brief Respond to a syscall exception.
 * 
 * Calls the handlers registered by #register_syscall_handler.
 */
void __onSyscallException(reg_block_t* regs)
{
	serial_put_string("__onSyscallException\x0a\x0d");
	return;

	exception_t e;

	if(!__exception_handler) { return; }

	__fetch_regs(&e, EXCEPTION_TYPE_SYSCALL, regs);

	// Fetch the syscall code from the opcode
	uint32_t epc = e.regs->epc;
	uint32_t opcode = *(uint32_t*)epc;
	uint32_t code = (opcode >> 6) & 0xfffff;

	bool called = false;
	for (int i=0; i<MAX_SYSCALL_HANDLERS; i++)
	{
		if (__syscall_handlers[i].handler && 
		    __syscall_handlers[i].first_code <= code &&
			__syscall_handlers[i].last_code >= code)
		{
			__syscall_handlers[i].handler(&e, code);
			called = true;
		}
	}

	if (!called)  {
		__onCriticalException(regs);
		return;
	}

	// Skip syscall opcode to continue execution
	e.regs->epc += 4;
}


// Fence (error) interrupt
void __FENCE_handler()
{
	serial_put_string("__FENCE_handler\x0a\x0d");

	while(true) { }
}

/*
void __onCriticalException(reg_block_t* regs)
{
	printf("__onCriticalException regs->cr=%08x,  regs->sr=%08x,  regs->epc=%08x\x0a\x0d", regs->cr, regs->sr, regs->epc);

	while(true) { }
}

void __onSyscallException(reg_block_t* regs)
{
	printf("__onSyscallException\x0a\x0d");

	while(true) { }
}
*/