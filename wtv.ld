SEARCH_DIR("=/lib");
SEARCH_DIR("=/lib/mips-linux-gnu");
SEARCH_DIR("=/usr/lib");
SEARCH_DIR("=/usr/local/lib");
SEARCH_DIR("=/usr/mips-linux-gnu/lib");
SEARCH_DIR("=/usr/lib/mips-linux-gnu");
SEARCH_DIR("=/usr/local/lib/mips-linux-gnu");

OUTPUT_FORMAT(
	"elf32-tradbigmips", 
	"elf32-tradbigmips", "elf32-tradlittlemips"
)
OUTPUT_ARCH(mips)
ENTRY(_start)

EXTERN(_start)

MEMORY
{
	memory0 : ORIGIN = 0x80000000, LENGTH = 64M
	control : ORIGIN = 0xa4000000, LENGTH = 8M
	expand0 : ORIGIN = 0x84800000, LENGTH = 8M
	expand1 : ORIGIN = 0x85000000, LENGTH = 8M
	expand2 : ORIGIN = 0x85800000, LENGTH = 8M
	expand3 : ORIGIN = 0x86000000, LENGTH = 8M
	expand4 : ORIGIN = 0x86800000, LENGTH = 8M
	expand5 : ORIGIN = 0x87000000, LENGTH = 8M
	expand6 : ORIGIN = 0x87800000, LENGTH = 8M
	rombnk0 : ORIGIN = 0x9f000000, LENGTH = 4M
	diagspc : ORIGIN = 0x9f400000, LENGTH = 4M
	rombnk1 : ORIGIN = 0x9f800000, LENGTH = 8M
}

__build_system_asic = BOX_SYSTEM_ASIC;
__build_for_solo = (__build_system_asic == box_asic_solo);
__build_for_spot = (__build_system_asic == box_asic_spot);

__build_storage_type = BUILD_STORAGE_TYPE;
__build_size_align_by = (__build_storage_type == box_storage_rom || __build_storage_type == box_storage_flash) ? 0x200000 : 0x200;

__build_base = BUILD_BASE_ADDRESS;
__build_size = (((__build_end - __build_base) + (__build_size_align_by - 1)) & ~(__build_size_align_by - 1));
__build_dword_size = (__build_size >> 2);
__code_size = (__code_end - __code_start);
__code_dword_size = (__code_size >> 2);

__data_start = DATAS_BASE_ADDRESS;
__data_size = (__data_end - __data_start);
__data_dword_size = (__data_size >> 2);
__bss_size = (__bss_end - __bss_start);
__bss_dword_size = (__bss_size >> 2);


/* A bit of a ternary mess BUT this tries to select the defined memory region based on the 
 * supplied base address.
 */
__build_region_start = (__build_base >= ORIGIN(rombnk1)) ? ORIGIN(rombnk1) : ((__build_base >= ORIGIN(rombnk0)) ? ORIGIN(rombnk0) : ((__build_base >= ORIGIN(memory0)) ? ORIGIN(memory0) : 0x00000000));
__build_region_length = (__build_base >= ORIGIN(rombnk1)) ? LENGTH(rombnk1) : ((__build_base >= ORIGIN(rombnk0)) ? LENGTH(rombnk0) : ((__build_base >= ORIGIN(memory0)) ? LENGTH(memory0) : 0x00000000));

__data_region_start = (__data_start >= ORIGIN(rombnk1)) ? ORIGIN(rombnk1) : ((__data_start >= ORIGIN(rombnk0)) ? ORIGIN(rombnk0) : ((__data_start >= ORIGIN(memory0)) ? ORIGIN(memory0) : 0x00000000));
__data_region_length = (__data_start >= ORIGIN(rombnk1)) ? LENGTH(rombnk1) : ((__data_start >= ORIGIN(rombnk0)) ? LENGTH(rombnk0) : ((__data_start >= ORIGIN(memory0)) ? LENGTH(memory0) : 0x00000000));

/* 
 * Caculate the memory region end. Sometimes we supply a memory region length so use that if
 * it's available. The reason why we might supply a memeory region length is because not all
 * WebTV boxes have the same amount of RAM or ROM even though the chipset might support more.
 */
__build_region_end = __build_base + ((BUILD_REGION_LENGTH > 0) ? BUILD_REGION_LENGTH : __build_region_length);
/* Clamp end to the end of the memory region */
__build_region_end = (__build_region_end <= (__build_region_start + __build_region_length)) ? __build_region_end : (__build_region_start + __build_region_length);
ASSERT(__build_end < __build_region_end, "Build image is too large. Try to simplify your code or reduce the amount of space needed for initialized static variables.");

__data_region_end = __data_start + ((DATAS_REGION_LENGTH > 0) ? DATAS_REGION_LENGTH : __data_region_length);
/* Clamp end to the end of the memory region */
__data_region_end = (__data_region_end <= (__data_region_start + __data_region_length)) ? __data_region_end : (__data_region_start + __data_region_length);
ASSERT(__bss_end < __data_region_end, "Combined data sections are too large. Try to reduce space needed for variables.");

__heap_size = (__data_region_end - __heap_start);

/*
  * WebTV's original build object format defines three sections:
 * 
 * CODE: 
 *   The section with .text and .rodata that is checksummed and doesn't change.
 *   This can be executed directly from ROM/Flash or copied into RAM then executed.
 *
 * DATA:
 *   This section typically contains all the .data and similar (.sdata etc...) sections.
 *   This is stored below the code section and is copied into RAM when a build is first
 *   executed. A lot of the time the data section is stored in a compressed format before
 *   it's loaded into memory.
 *
 * BSS:
 *   This contains uninitialized variables. The size is defined but no data is stored.
 *   When the build first starts this section is allocated in RAM and is initilized with 0's.
 *
 * - libwtv doesn't make any changes to the original object format.
 * - The original format was based against MIPS ECOFF rather than ELF.
 * - Debug symbols are defined in an single .sym file.
 * - There cam be file system (ROMFS) image stored below the build object that stores the .sym file.
 *
 */
SECTIONS
{
	/* START WebTV object binary */

	PROVIDE(__executable_start = __code_start);
	__code_start = __build_base;
	. = __code_start;
	.code . :
	{
		/* .text.startup starts off with the branch instruction that jumps to code that inits
		 * the system, prepares the .data and .bss section in RAM then calls main()
		 *
		 * After the branch instruction, there's metadata that defines the .code section
		 * checksum, size of the build, .data and .bss attributes, build version and other
		 * build metadata. .text.startup is layed out in the src/crt0.S file.
		 */
		*(.text.startup .text.startup.*)
		. = ALIGN(0x10);

		/* main code that the build runs after startup */
		KEEP(*(SORT_NONE(.init)))
		_ftext = .; PROVIDE(ftext = .); PROVIDE(__ftext = .);
		*(.text.unlikely .text.*_unlikely .text.unlikely.*)
		*(.text.exit .text.exit.*)
		*(.text.hot .text.hot.*)
		*(SORT(.text.sorted.*))
		*(.text .stub .text.* .gnu.linkonce.t.*)
		*(.gnu.warning)
		*(.mips16.fn.*) *(.mips16.call.*)
		. = ALIGN(0x10);
		PROVIDE(_etext = .); PROVIDE(etext = .); PROVIDE(__etext = .);

		*(.rodata .rodata.* .gnu.linkonce.r.*)
		*(.rodata1)

		. = ALIGN(0x10);
	}
	PROVIDE(__code_end = .);

	PROVIDE(__data_stored_start = .);
	. = __data_start;
	.data . :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
		KEEP(*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
		PROVIDE_HIDDEN (__init_array_end = .);
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
		KEEP(*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
		PROVIDE_HIDDEN (__fini_array_end = .);

		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		/* gcc uses crtbegin.o to find the start of the constructors, so we make sure it is
		 * first.  Because this is a wildcard, it doesn't matter if the user does not actually
		 * link against crtbegin.o; the linker won't look for a file to match a wildcard. The
		 * wildcard also means that it doesn't matter which directory crtbegin.o is in.
		 */
		KEEP(*crtbegin.o(.ctors))
		KEEP(*crtbegin?.o(.ctors))
		/* We don't want to include the .ctor section from the crtend.o file until after the
		 * sorted ctors. The .ctor section from the crtend file contains the end of ctors marker
		 * and it must be last
		 */
		KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o ) .ctors))
		KEEP(*(SORT(.ctors.*)))
		KEEP(*(.ctors))
		LONG(0)
		__CTOR_END__ = .;

		__DTOR_LIST__ = .;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
		KEEP(*crtbegin.o(.dtors))
		KEEP(*crtbegin?.o(.dtors))
		KEEP(*(EXCLUDE_FILE(*crtend.o *crtend?.o ) .dtors))
		KEEP(*(SORT(.dtors.*)))
		KEEP(*(.dtors))
		LONG(0)
		__DTOR_END__ = .;

		_fdata = .; PROVIDE(fdata = .); PROVIDE(__fdata = .);
		/* primary .data sections */
		*(.data .data.* .gnu.linkonce.d.*)
		SORT(CONSTRUCTORS)
		*(.data1)
		/* thread data */
		PROVIDE_HIDDEN(__tdata_start = .);
		*(.tdata .tdata.* .gnu.linkonce.td.*)

		_gp = ALIGN(0x10) + 0x8000;
		/* small data */
		*(.sdata .sdata.* .gnu.linkonce.s.*)
		*(.lit8 .lit4)
		*(.got.plt .got)
		*(.rld_map)
		_edata = .; PROVIDE(__edata = .); PROVIDE(edata = .);
		. = ALIGN(0x10);
	} > memory0
	PROVIDE(__data_end = .);
	PROVIDE(__data_stored_end = __data_stored_start + (__data_end - __data_start));
	PROVIDE(__build_end = __data_stored_end);

	/*
	 * This is the end of what's stored in WebTV's object file.
	 * Anything below may be defined in the object file but data isn't stored.
	 */
	.bss (NOLOAD) :
	{
		PROVIDE(__bss_start = .);
		_fbss = .; PROVIDE(fbss = .); PROVIDE(__fbss = .);
		/* small bss data */
		*(.sbss .sbss.* .gnu.linkonce.sb.*)
		*(.scommon .scommon.*)
		*(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
		*(.dynsbss)

		/* regular .bss data */
		*(.dynbss)
		*(.bss .bss.* .gnu.linkonce.b.*)
		/* This is a legacy section from older versions of GCC */
		*(COMMON)

		/* thread data */
		*(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon)
		PROVIDE(_ebss = .); PROVIDE(ebss = .); PROVIDE(__ebss = .);
		PROVIDE(__bss_end = .);

	} > memory0
	/* O1Heap requires (sizeof(void*) * 4U) alignment. */
	PROVIDE(__heap_start = ALIGN(0x10));

	/* END WebTV object binary */

	/* ---------------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------------- */

	/*
	 * Sections below are included in the GCC ELF file
	 * but aren't included in the WebTV object binary file.
	 */

	.note.gnu.build-id    . : { *(.note.gnu.build-id) }
	.note.GNU-stack       . : { *(.note.GNU-stack) }
	.gcc_compiled_long32  . : { KEEP(*(.gcc_compiled_long32)) }
	.gcc_compiled_long64  . : { KEEP(*(.gcc_compiled_long64)) }
	.gnu.build.attributes . : { *(.gnu.build.attributes .gnu.build.attributes.*) }
	.gnu.version          . : { *(.gnu.version) }
	.gnu.version_d        . : { *(.gnu.version_d) }
	.gnu.version_r        . : { *(.gnu.version_r) }
	.gnu.hash             . : { *(.gnu.hash) }
	.MIPS.abiflags        . : { *(.MIPS.abiflags) }
	.MIPS.xhash           . : { *(.MIPS.xhash) }
	.MIPS.stubs           . : { *(.MIPS.stubs) }
	.hash                 . : { *(.hash) }
	.reginfo              . : { *(.reginfo) }
	.dynamic              . : { *(.dynamic .dynsym .dynstr) }
	.rel.dyn              . : { *(.rel.dyn) }
	.interp               . : { *(.interp) }
	.gnu_debuglink        . : { *(.gnu_debuglink) }
	.gnu.lto              . : { *(.gnu.lto_*) }
	.comment              . (INFO) : { *(.comment); LINKER_VERSION; }
	.eh_frame             . : { *(.eh_frame) }
	/* Stabs debugging sections.  */
	.stab                 . : { *(.stab) }
	.stabstr              . : { *(.stabstr) }
	.stab.excl            . : { *(.stab.excl) }
	.stab.exclstr         . : { *(.stab.exclstr) }
	.stab.index           . : { *(.stab.index) }
	.stab.indexstr        . : { *(.stab.indexstr) }
	/* DWARF debug sections.
	 * Symbols in the DWARF debugging sections are relative to the beginning
	 * of the section so we begin them at 0.  */
	/* DWARF 1.  */
	.debug                . : { *(.debug) }
	.line                 . : { *(.line) }
	/* GNU DWARF 1 extensions.  */
	.debug_srcinfo        . : { *(.debug_srcinfo) }
	.debug_sfnames        . : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2.  */
	.debug_aranges        . : { *(.debug_aranges) }
	.debug_pubnames       . : { *(.debug_pubnames) }
	/* DWARF 2.  */
	.debug_info           . : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev         . : { *(.debug_abbrev) }
	.debug_line           . : { *(.debug_line .debug_line.* .debug_line_end) }
	.debug_frame          . : { *(.debug_frame) }
	.debug_str            . : { *(.debug_str) }
	.debug_loc            . : { *(.debug_loc) }
	.debug_macinfo        . : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions.  */
	.debug_weaknames      . : { *(.debug_weaknames) }
	.debug_funcnames      . : { *(.debug_funcnames) }
	.debug_typenames      . : { *(.debug_typenames) }
	.debug_varnames       . : { *(.debug_varnames) }
	/* DWARF 3.  */
	.debug_pubtypes       . : { *(.debug_pubtypes) }
	.debug_ranges         . : { *(.debug_ranges) }
	/* DWARF 5.  */
	.debug_addr           . : { *(.debug_addr) }
	.debug_line_str       . : { *(.debug_line_str) }
	.debug_loclists       . : { *(.debug_loclists) }
	.debug_macro          . : { *(.debug_macro) }
	.debug_names          . : { *(.debug_names) }
	.debug_rnglists       . : { *(.debug_rnglists) }
	.debug_str_offsets    . : { *(.debug_str_offsets) }
	.debug_sup            . : { *(.debug_sup) }
	.gnu.attributes       . : { KEEP (*(.gnu.attributes)) }
	.gptab.sdata          . : { *(.gptab.data) *(.gptab.sdata) }
	.gptab.sbss           . : { *(.gptab.bss) *(.gptab.sbss) }
	.mdebug.abi32         . : { KEEP(*(.mdebug.abi32)) }
	.mdebug.abiN32        . : { KEEP(*(.mdebug.abiN32)) }
	.mdebug.abi64         . : { KEEP(*(.mdebug.abi64)) }
	.mdebug.abiO64        . : { KEEP(*(.mdebug.abiO64)) }
	.mdebug.eabi32        . : { KEEP(*(.mdebug.eabi32)) }
	.mdebug.eabi64        . : { KEEP(*(.mdebug.eabi64)) }
	.symtab               . : { *(.symtab) }
	.strtab               . : { *(.strtab) }
	.shstrtab             . : { *(.shstrtab) }
}
