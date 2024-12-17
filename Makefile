all: libwtv

BOX_CPU_ARCHITECTURE    ?= mips
BOX_CPU_SUBARCHITECTURE ?= 4650
LIBWTV_INSTALL_DIR      ?= /opt/libwtv
INCLUDE_DIR              = $(CURDIR)/include
SOURCE_DIR               = $(CURDIR)/src
BUILD_DIR                = $(CURDIR)/build/$(WTVLIB_FILE_SUFFIX)
INSTALL_DIR              = $(LIBWTV_INSTALL_DIR)
WTVLIB_FILE_SUFFIX       = $(BOX_CPU_ARCHITECTURE)-$(BOX_CPU_SUBARCHITECTURE)

TARGET_BOX              ?= all

include wtv.mk

libwtv: CC=$(WTV_CC)
libwtv: CXX=$(WTV_CXX)
libwtv: AS=$(WTV_AS)
libwtv: LD=$(WTV_LD)
libwtv: LIBWTV_COMPILE_FLAGS=-ffile-prefix-map=$(CURDIR)=libwtv
libwtv: CFLAGS=$(WTV_CFLAGS) $(LIBWTV_COMPILE_FLAGS)
libwtv: CXXFLAGS=$(WTV_CXXFLAGS) $(LIBWTV_COMPILE_FLAGS)
libwtv: ASFLAGS=$(WTV_ASFLAGS) $(LIBWTV_COMPILE_FLAGS)
libwtv: LDFLAGS=$(WTV_LDFLAGS)
libwtv: libwtv-$(WTVLIB_FILE_SUFFIX).a

libwtv-$(WTVLIB_FILE_SUFFIX).a: \
          $(BUILD_DIR)/exception.o $(BUILD_DIR)/interrupt.o $(BUILD_DIR)/exception-handler.o \
          $(BUILD_DIR)/backtrace.o \
          $(BUILD_DIR)/debug.o \
          $(BUILD_DIR)/crt0.o \
          $(BUILD_DIR)/wtvsys.o \
          $(BUILD_DIR)/console.o \
          $(BUILD_DIR)/memory.o \
          $(BUILD_DIR)/leds.o \
          $(BUILD_DIR)/timer.o \
          $(BUILD_DIR)/serial.o \
          $(BUILD_DIR)/surface.o \
          $(BUILD_DIR)/sprite.o \
          $(BUILD_DIR)/display.o \
          $(BUILD_DIR)/graphics.o \
          $(BUILD_DIR)/audio.o \
          $(BUILD_DIR)/ssid.o \
          $(BUILD_DIR)/hid.o \
          $(BUILD_DIR)/iic.o \
          $(BUILD_DIR)/hid/ir.o \
          $(BUILD_DIR)/hid/ps2.o \
          $(BUILD_DIR)/hid/event.o \
          $(BUILD_DIR)/storage/nvram.o \
          $(BUILD_DIR)/storage/atapwd.o \
          $(BUILD_DIR)/libc/o1heap.o \
          $(BUILD_DIR)/libc/string.o \
          $(BUILD_DIR)/libc/printf.o \
          $(BUILD_DIR)/libc/math.o \
          $(BUILD_DIR)/libc.o
	@echo "    [AR -$(WTVLIB_FILE_SUFFIX)] $@ $(BUILD_DIR)"
	$(WTV_AR) -rcs -o $@ $^

install-mk: $(INSTALL_DIR)/include/wtv.mk

$(INSTALL_DIR)/include/wtv.mk: wtv.mk
# Always update timestamp of n64.mk. This make sure that further targets
# depending on install-mk won't always try to re-install it.
	mkdir -p $(INSTALL_DIR)/include
	install -cv -m 0644 wtv.mk $(INSTALL_DIR)/include/wtv.mk

install: install-mk libwtv
	mkdir -p $(INSTALL_DIR)/lib
	install -Cv -m 0644 libwtv-$(WTVLIB_FILE_SUFFIX).a $(INSTALL_DIR)/lib/libwtv-$(WTVLIB_FILE_SUFFIX).a
	install -Cv -m 0644 wtv.ld $(INSTALL_DIR)/lib/wtv.ld
	mkdir -p $(INSTALL_DIR)/include
	install -Cv -m 0644 include/audio.h $(INSTALL_DIR)/include/audio.h
	install -Cv -m 0644 include/display.h $(INSTALL_DIR)/include/display.h
	install -Cv -m 0644 include/font.h $(INSTALL_DIR)/include/font.h
	install -Cv -m 0644 include/wtvsys.h $(INSTALL_DIR)/include/wtvsys.h
	install -Cv -m 0644 include/graphics.h $(INSTALL_DIR)/include/graphics.h
	install -Cv -m 0644 include/interrupt.h $(INSTALL_DIR)/include/interrupt.h
	install -Cv -m 0644 include/backtrace.h $(INSTALL_DIR)/include/backtrace.h
	install -Cv -m 0644 include/debug.h $(INSTALL_DIR)/include/debug.h
	install -Cv -m 0644 include/exception.h $(INSTALL_DIR)/include/exception.h
	install -Cv -m 0644 include/iic.h $(INSTALL_DIR)/include/iic.h
	install -Cv -m 0644 include/kbinput.h $(INSTALL_DIR)/include/kbinput.h
	install -Cv -m 0644 include/ssid.h $(INSTALL_DIR)/include/ssid.h
	install -Cv -m 0644 include/console.h $(INSTALL_DIR)/include/console.h
	install -Cv -m 0644 include/timer.h $(INSTALL_DIR)/include/timer.h
	install -Cv -m 0644 include/leds.h $(INSTALL_DIR)/include/leds.h
	install -Cv -m 0644 include/libwtv.h $(INSTALL_DIR)/include/libwtv.h
	install -Cv -m 0644 include/libc.h $(INSTALL_DIR)/include/libc.h
	install -Cv -m 0644 include/memory.h $(INSTALL_DIR)/include/memory.h
	install -Cv -m 0644 include/sprite.h $(INSTALL_DIR)/include/sprite.h
	install -Cv -m 0644 include/cop0.h $(INSTALL_DIR)/include/cop0.h
	install -Cv -m 0644 include/cop1.h $(INSTALL_DIR)/include/cop1.h
	install -Cv -m 0644 include/serial.h $(INSTALL_DIR)/include/serial.h
	install -Cv -m 0644 include/surface.h $(INSTALL_DIR)/include/surface.h
	install -Cv -m 0644 include/hid.h $(INSTALL_DIR)/include/hid.h
	mkdir -p $(INSTALL_DIR)/include/hid
	install -Cv -m 0644 include/hid/ir.h $(INSTALL_DIR)/include/hid/ir.h
	install -Cv -m 0644 include/hid/ps2.h $(INSTALL_DIR)/include/hid/ps2.h
	install -Cv -m 0644 include/hid/event.h $(INSTALL_DIR)/include/hid/event.h
	mkdir -p $(INSTALL_DIR)/include/hid/event
	install -Cv -m 0644 include/hid/event/keyboard.h $(INSTALL_DIR)/include/hid/event/keyboard.h
	mkdir -p $(INSTALL_DIR)/include/storage
	install -Cv -m 0644 include/storage/nvram.h $(INSTALL_DIR)/include/storage/nvram.h
	install -Cv -m 0644 include/storage/atapwd.h $(INSTALL_DIR)/include/storage/atapwd.h
	mkdir -p $(INSTALL_DIR)/include/libc
	install -Cv -m 0644 include/libc/io.h $(INSTALL_DIR)/include/libc/io.h
	install -Cv -m 0644 include/libc/o1heap.h $(INSTALL_DIR)/include/libc/o1heap.h
	install -Cv -m 0644 include/libc/printf.h $(INSTALL_DIR)/include/libc/printf.h
	install -Cv -m 0644 include/libc/__strerror.h $(INSTALL_DIR)/include/libc/__strerror.h
	install -Cv -m 0644 include/libc/string.h $(INSTALL_DIR)/include/libc/string.h

clean:
	rm -f *.o *.a
	rm -rf $(CURDIR)/build

clobber: clean

.PHONY: clobber clean install-mk

# Automatic dependency tracking
-include $(wildcard $(BUILD_DIR)/$(WTVLIB_FILE_SUFFIX)/*.d) $(wildcard $(BUILD_DIR)/$(WTVLIB_FILE_SUFFIX)/*/*.d)
