all: timers.wtv.o
.PHONY: all

LIBWTV_INSTALL_DIR ?= /opt/libwtv

BUILD_DIR = build
#TARGET_BOX ?= bf0-app
TARGET_BOX ?= lc2-app
include $(LIBWTV_INSTALL_DIR)/include/wtv.mk

OBJS = $(BUILD_DIR)/timers.o

timers.wtv.o: N64_ROM_TITLE = "Timer Test"

$(BUILD_DIR)/timers.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR) *.wtv.o
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d))
