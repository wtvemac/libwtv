# The build (.code, .rodata, compressed .data, ROMFS) and data section (.data, .bss, stack, heap etc...) sizes can be adjusted.
# On systems where the build is stored on disk-based system, the build and data are shared in RAM.
# Flash (but not flashdisk) and ROM based systems can execute te build direct from the storage chips and use RAM for data.

# These are default values used if a custom target box is selected. These default values correspond to an LC2 box (original US WebTV Plus).
TARGET_BOX              ?= box_is_lc2-app
BOX_SYSTEM_ASIC         ?= box_asic_solo
BOX_CPU_ARCHITECTURE    ?= mips
BOX_CPU_SUBARCHITECTURE ?= 4650
BUILD_STORAGE_TYPE      ?= box_storage_idedisk
BUILD_BASE_ADDRESS      ?= 0x80440000
BUILD_REGION_LENGTH     ?= 3932160    # ~3.75MB
DATAS_BASE_ADDRESS      ?= 0x80001000
DATAS_REGION_LENGTH     ?= 4452352    # ~4.25MB

AVAILABLE_TARGET_BOXES  = box_is_bf0-boot, bf0-4m-boot, bfe-boot, bf0-app, bf0-4m-app, bfe-app
AVAILABLE_TARGET_BOXES += box_is_lc2-boot, lc2jp-classic-boot, lc2jp-plus-boot, lc2w-boot, derby-boot, lc2-app, lc2jp-classic-app, lc2jp-plus-app, lc2w-app, derby-app
AVAILABLE_TARGET_BOXES += box_is_bps-boot, lc25-boot, bps-app, lc25-app
AVAILABLE_TARGET_BOXES += box_is_estar-boot, utv-boot, estar-app, utv-app
AVAILABLE_TARGET_BOXES += custom

BUILD_TARGET_CONSTANTS += box_is_bf0=1
BUILD_TARGET_CONSTANTS += box_is_bfe=1
BUILD_TARGET_CONSTANTS += box_is_lc2=1
BUILD_TARGET_CONSTANTS += box_is_bps=1
BUILD_TARGET_CONSTANTS += box_is_lc25=1
BUILD_TARGET_CONSTANTS += box_is_estar=1
BUILD_TARGET_CONSTANTS += box_is_utv=1
BUILD_TARGET_CONSTANTS += build_is_bootrom=1
BUILD_TARGET_CONSTANTS += build_is_approm=1
BUILD_TARGET_CONSTANTS += mips=1
BUILD_TARGET_CONSTANTS += mipsel=2
BUILD_TARGET_CONSTANTS += box_asic_solo=1
BUILD_TARGET_CONSTANTS += box_asic_spot=2
BUILD_TARGET_CONSTANTS += box_storage_rom=1
BUILD_TARGET_CONSTANTS += box_storage_flash=5
BUILD_TARGET_CONSTANTS += box_storage_idedisk=2
BUILD_TARGET_CONSTANTS += box_storage_flashdisk=6

OPTIMIZATION_OPTIONS   ?= -O3

SOURCE_DIR             ?= .
BUILD_DIR              ?= .

INC                    ?= -I$(LIBWTV_INSTALL_DIR)/include -I$(LIBWTV_INSTALL_DIR)/include/audio/minibae/
LIB_DIR                ?= $(LIBWTV_INSTALL_DIR)/lib
TOOLS_DIR              ?= $(LIBWTV_INSTALL_DIR)/bin

WTV_GCC_DIR             ?= /usr
ifeq ($(BOX_CPU_ARCHITECTURE),$(filter $(BOX_CPU_ARCHITECTURE),mips64 mips64el))
WTV_GCC_TRIPLET_PREFIX  ?= $(WTV_GCC_DIR)/bin/mips64-linux-gnu-
else
WTV_GCC_TRIPLET_PREFIX  ?= $(WTV_GCC_DIR)/bin/mips-linux-gnu-
endif
WTV_CC                  ?= $(WTV_GCC_TRIPLET_PREFIX)gcc
WTV_CXX                 ?= $(WTV_GCC_TRIPLET_PREFIX)g++
WTV_AS                  ?= $(WTV_GCC_TRIPLET_PREFIX)as
WTV_AR                  ?= $(WTV_GCC_TRIPLET_PREFIX)ar
WTV_LD                  ?= $(WTV_GCC_TRIPLET_PREFIX)ld
WTV_OBJCOPY             ?= $(WTV_GCC_TRIPLET_PREFIX)objcopy
WTV_OBJDUMP             ?= $(WTV_GCC_TRIPLET_PREFIX)objdump
WTV_SIZE                ?= $(WTV_GCC_TRIPLET_PREFIX)size
WTV_NM                  ?= $(WTV_GCC_TRIPLET_PREFIX)nm

#
# BootROM images
#
ifeq ($(TARGET_BOX),bf0-boot)                # US classic (Sony INT-W100; Philips Magnavox MAT960A101, MAT960A102, MAT960A103)
BOX_SYSTEM_CLASS         = box_is_bf0
BOX_SYSTEM_ASIC          = box_asic_spot
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 2093056
else ifeq ($(TARGET_BOX),bf0-4m-boot)        # Early prototype US classic with 4MB RAM and flash
BOX_SYSTEM_CLASS         = box_is_bf0
BOX_SYSTEM_ASIC          = box_asic_spot
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 2093056
else ifeq ($(TARGET_BOX),bfe-boot)           # Early prototype US classic
BOX_SYSTEM_CLASS         = box_is_bfe
BOX_SYSTEM_ASIC          = box_asic_spot
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 2093056
else ifeq ($(TARGET_BOX),lc2-boot)           # US plus (Sony INT-W200, Philips Magnavox MAT972; Samsung SIS-100; Mitsubishi WB-2000)
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 8384512
else ifeq ($(TARGET_BOX),lc2jp-classic-boot) # Japan classic (Sony INT-WJ200; Fujitsu F993000)
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 4194304
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 8384512
else ifeq ($(TARGET_BOX),lc2jp-plus-boot)    # Japan plus (Sony INT-WJ300; Matsushita TU-WE100)
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 4194304
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 8384512
else ifeq ($(TARGET_BOX),lc2w-boot)          # Italian flash-only prototype. NOTE: This needs wtvlib to be compiled with BOX_CPU_ARCHITECTURE=mipsel
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mipsel
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_flash
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 4194304
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 8384512
else ifeq ($(TARGET_BOX),derby-boot)         # US plus with faster CPU and softmodem
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 8384512
else ifeq ($(TARGET_BOX),bps-boot)           # New classic (Sony INT-W150; Philips Magnavox MAT965, MAT965KB; RCA RW2100)
BOX_SYSTEM_CLASS         = box_is_bps
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 8384512
else ifeq ($(TARGET_BOX),lc25-boot)          # New plus (Sony INT-W250; Philips Magnavox MAT976, MAT976KB; RCA RW2110)
BOX_SYSTEM_CLASS         = box_is_lc25
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 16773120
else ifeq ($(TARGET_BOX),estar-boot)         # DishNetwork satellite receiver (Echostar DishPlayer 7100, DishPlayer 7200)
BOX_SYSTEM_CLASS         = box_is_estar
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 16773120
else ifeq ($(TARGET_BOX),utv-boot)           # DirectTV satellite receiver (Sony SAT-W60; RCA DWD490RE, DWD495RG)
BOX_SYSTEM_CLASS         = box_is_utv
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_bootrom
BUILD_STORAGE_TYPE       = box_storage_rom
BUILD_BASE_ADDRESS       = 0x9fc00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 33550336
#
# AppROM images
#
else ifeq ($(TARGET_BOX),bf0-app)            # US classic (Sony INT-W100; Philips Magnavox MAT960A101, MAT960A102, MAT960A103)
BOX_SYSTEM_CLASS         = box_is_bf0
BOX_SYSTEM_ASIC          = box_asic_spot
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_flash
BUILD_BASE_ADDRESS       = 0x9f000000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 2093056
else ifeq ($(TARGET_BOX),mame-bf0-8m4m-app)  # US classic emulated in MAME with 8MB RAM and 4MB flash
BOX_SYSTEM_CLASS         = box_is_bf0
BOX_SYSTEM_ASIC          = box_asic_spot
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_flash
BUILD_BASE_ADDRESS       = 0x9f000000
BUILD_REGION_LENGTH      = 4194304
DATAS_BASE_ADDRESS       = 0x80001000
#DATAS_REGION_LENGTH      = 8384512
DATAS_REGION_LENGTH      = 4452352
else ifeq ($(TARGET_BOX),bf0-4m-app)         # Early prototype US classic with 4MB RAM and flash
BOX_SYSTEM_CLASS         = box_is_bf0
BOX_SYSTEM_ASIC          = box_asic_spot
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_flash
BUILD_BASE_ADDRESS       = 0x9f000000
BUILD_REGION_LENGTH      = 4194304
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 4190208
else ifeq ($(TARGET_BOX),bfe-app)            # Early prototype US classic
BOX_SYSTEM_CLASS         = box_is_bfe
BOX_SYSTEM_ASIC          = box_asic_spot
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_flash
BUILD_BASE_ADDRESS       = 0x9fe00000
BUILD_REGION_LENGTH      = 2097152
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 2093056
else ifeq ($(TARGET_BOX),lc2-app)            # US plus (Sony INT-W200, Philips Magnavox MAT972; Samsung SIS-100; Mitsubishi WB-2000)
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 4650
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_idedisk
BUILD_BASE_ADDRESS       = 0x80440000
BUILD_REGION_LENGTH      = 3932160
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 4452352
else ifeq ($(TARGET_BOX),lc2jp-classic-app)  # Japan classic (Sony INT-WJ200; Fujitsu F993000)
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_idedisk
BUILD_BASE_ADDRESS       = 0x80440000
BUILD_REGION_LENGTH      = 3932160
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 4452352
else ifeq ($(TARGET_BOX),lc2jp-plus-app)     # Japan plus (Sony INT-WJ300; Matsushita TU-WE100)
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_idedisk
BUILD_BASE_ADDRESS       = 0x80440000
BUILD_REGION_LENGTH      = 3932160
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 4452352
else ifeq ($(TARGET_BOX),lc2w-app)           # Italian flash-only prototype. NOTE: This needs wtvlib to be compiled with BOX_CPU_ARCHITECTURE=mipsel
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mipsel
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_flash
BUILD_BASE_ADDRESS       = 0x9f000000
BUILD_REGION_LENGTH      = 4194304
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 8384512
else ifeq ($(TARGET_BOX),derby-app)          # US plus with faster CPU and softmodem
BOX_SYSTEM_CLASS         = box_is_lc2
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_idedisk
BUILD_BASE_ADDRESS       = 0x80440000
BUILD_REGION_LENGTH      = 3932160
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 4452352
else ifeq ($(TARGET_BOX),bps-app)            # New classic (Sony INT-W150; Philips Magnavox MAT965, MAT965KB; RCA RW2100)
BOX_SYSTEM_CLASS         = box_is_bps
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_STORAGE_TYPE       = box_storage_flashdisk
BUILD_BASE_ADDRESS       = 0x803c0000
BUILD_REGION_LENGTH      = 4456448
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 3928064
else ifeq ($(TARGET_BOX),lc25-app)           # New plus (Sony INT-W250; Philips Magnavox MAT976, MAT976KB; RCA RW2110)
BOX_SYSTEM_CLASS         = box_is_lc25
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_flashdisk
BUILD_BASE_ADDRESS       = 0x80a40000
BUILD_REGION_LENGTH      = 6029312
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 10743808
else ifeq ($(TARGET_BOX),estar-app)           # DishNetwork satellite receiver (Echostar DishPlayer 7100, DishPlayer 7200)
BOX_SYSTEM_CLASS         = box_is_estar
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_idedisk
BUILD_BASE_ADDRESS       = 0x80aa0000
BUILD_REGION_LENGTH      = 5636096
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 11137024
else ifeq ($(TARGET_BOX),utv-app)            # DirectTV satellite receiver (Sony SAT-W60; RCA DWD490RE, DWD495RG)
BOX_SYSTEM_CLASS         = box_is_utv
BOX_SYSTEM_ASIC          = box_asic_solo
BOX_CPU_ARCHITECTURE     = mips
BOX_CPU_SUBARCHITECTURE  = 5000
BUILD_CLASS              = build_is_approm
BUILD_STORAGE_TYPE       = box_storage_idedisk
BUILD_BASE_ADDRESS       = 0x80ce0000
BUILD_REGION_LENGTH      = 20054016
DATAS_BASE_ADDRESS       = 0x80001000
DATAS_REGION_LENGTH      = 13496320
else ifeq ($(TARGET_BOX),custom)             # Custom box target parameters
else ifeq ($(TARGET_BOX),all)                # For building the library
else
$(error Bad or no target box '$(TARGET_BOX)' specified. You need to specify TARGET_BOX=[TARGET BOX]. [TARGET BOX] may be one of the following: $(AVAILABLE_TARGET_BOXES))
endif

ifneq ($(TARGET_BOX),all)
BUILD_TARGET_CONSTANTS  += BOX_SYSTEM_CLASS=$(BOX_SYSTEM_CLASS)
BUILD_TARGET_CONSTANTS  += BOX_SYSTEM_ASIC=$(BOX_SYSTEM_ASIC)
BUILD_TARGET_CONSTANTS  += BOX_CPU_ARCHITECTURE=$(BOX_CPU_ARCHITECTURE)
BUILD_TARGET_CONSTANTS  += BUILD_STORAGE_TYPE=$(BUILD_STORAGE_TYPE)
BUILD_TARGET_CONSTANTS  += BUILD_BASE_ADDRESS=$(BUILD_BASE_ADDRESS)
BUILD_TARGET_CONSTANTS  += BUILD_REGION_LENGTH=$(BUILD_REGION_LENGTH)
BUILD_TARGET_CONSTANTS  += DATAS_BASE_ADDRESS=$(DATAS_BASE_ADDRESS)
BUILD_TARGET_CONSTANTS  += DATAS_REGION_LENGTH=$(DATAS_REGION_LENGTH)
endif

WTVLIB_FILE_SUFFIX      ?= $(BOX_CPU_ARCHITECTURE)-$(BOX_CPU_SUBARCHITECTURE)

WTV_CCOMPILE_FLAGS       = -march=$(BOX_CPU_SUBARCHITECTURE) $(OPTIMIZATION_OPTIONS) -MMD $(INC)
WTV_CCOMPILE_FLAGS      += -mxgot -mno-abicalls -fno-pic -fno-plt
WTV_CCOMPILE_FLAGS      += -nostdlib -nostdlib++ -nodefaultlibs -nostartfiles -ffreestanding
WTV_CCOMPILE_FLAGS      += -falign-functions=32   # NOTE: if you change this, also change backtrace() in backtrace.c
WTV_CCOMPILE_FLAGS      += -ffunction-sections -fdata-sections
WTV_CCOMPILE_FLAGS      += -ffast-math -ftrapping-math -fno-associative-math
WTV_CCOMPILE_FLAGS      += -Wall -fdiagnostics-color=always -Werror -Wno-error=deprecated-declarations -Wno-error=unused-variable -Wno-error=unused-but-set-variable -Wno-error=unused-function -Wno-error=unused-parameter -Wno-error=unused-but-set-parameter -Wno-error=unused-label -Wno-error=unused-local-typedefs -Wno-error=unused-const-variable
WTV_CCOMPILE_FLAGS      += -DX_PLATFORM=X_LIBWTV -D_THREAD_SAFE
WTV_CCOMPILE_FLAGS      += -DPRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS=0 -DPRINTF_SUPPORT_DECIMAL_SPECIFIERS=0 -DPRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD

#-DPRINTF_SUPPORT_LONG_LONG=0  

ifeq ($(BOX_CPU_ARCHITECTURE),$(filter $(BOX_CPU_ARCHITECTURE),mipsel mips64el))
WTV_CCOMPILE_FLAGS      += -EL
else
WTV_CCOMPILE_FLAGS      += -EB
endif

WTV_CFLAGS               = -std=gnu99 $(WTV_CCOMPILE_FLAGS)
WTV_CXXFLAGS             = -std=gnu++17 $(WTV_CCOMPILE_FLAGS)
WTV_ASFLAGS              = $(WTV_CCOMPILE_FLAGS)

WTV_LDFLAGS              = -L$(LIB_DIR) --warn-multiple-gp --gc-sections -lm

ifeq ($(DEBUG),1)
WTV_CFLAGS              += -g3 -D_DEBUG=1
WTV_CXXFLAGS            += -g3 -D_DEBUG=1
WTV_ASFLAGS             += -g
WTV_LDFLAGS             += -g
endif


#-mtune=$(BOX_CPU_SUBARCHITECTURE)
%.wtv.o: CC=$(WTV_CC)
%.wtv.o: CXX=$(WTV_CXX)
%.wtv.o: AS=$(WTV_AS)
%.wtv.o: LD=$(WTV_LD)
%.wtv.o: WTVO_COMPILE_FLAGS=-ffile-prefix-map="$(CURDIR)"= $(patsubst %,-D%,$(strip $(BUILD_TARGET_CONSTANTS)))
%.wtv.o: CFLAGS=$(WTV_CFLAGS) $(WTVO_COMPILE_FLAGS)
%.wtv.o: CXXFLAGS=$(WTV_CXXFLAGS) $(WTVO_COMPILE_FLAGS)
%.wtv.o: ASFLAGS=$(WTV_ASFLAGS) $(WTVO_COMPILE_FLAGS)
%.wtv.o: LDFLAGS=$(WTV_LDFLAGS) -lwtv-$(WTVLIB_FILE_SUFFIX) -lminibae-$(WTVLIB_FILE_SUFFIX) -Twtv.ld $(patsubst %,--defsym=%,$(strip $(BUILD_TARGET_CONSTANTS)))
%.wtv.o: $(BUILD_DIR)/%.elf
	@echo "    [WTV-$(WTVLIB_FILE_SUFFIX)] $@"
	set -e; \
	if [ -f "$(LIB_DIR)/libwtv-$(WTVLIB_FILE_SUFFIX).a" ]; \
	then \
		./do-poop.sh; \
		rm -f $@; \
	else \
		>&2 echo "Can't find library 'libwtv-$(WTVLIB_FILE_SUFFIX).a'. I need a matching library file to build."; \
		>&2 echo "If you haven't built libwtv, then build it. If you built libwtv, then make sure your install folder is correct and you built it with the '$(WTVLIB_FILE_SUFFIX)' CPU target."; \
		>&2 echo "More information is at https://github.com/wtvemac/libwtv"; \
	fi

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.S
	@mkdir -p $(dir $@)
	@echo "    [AS $(WTVLIB_FILE_SUFFIX)] $<"
	$(CC)  -c $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c 
	@mkdir -p $(dir $@)
	@echo "    [CC $(WTVLIB_FILE_SUFFIX)] $<"
	$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo "    [CXX $(WTVLIB_FILE_SUFFIX)] $<"
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.elf: LDFLAGS+=-lc -Map=$(BUILD_DIR)/$(notdir $(basename $@)).map
%.elf: $(LIB_DIR)/libwtv-$(WTVLIB_FILE_SUFFIX).a $(LIB_DIR)/wtv.ld
	@mkdir -p $(dir $@)
	@echo "    [LD -$(WTVLIB_FILE_SUFFIX)] $@"
	$(LD) -o $@ $(filter-out $(LIB_DIR)/wtv.ld,$^) $(LDFLAGS)

ifneq ($(V),1)
.SILENT:
endif
