all: poopies.wtv.o
.PHONY: all

LIBWTV_INSTALL_DIR ?= /opt/libwtv
OPTIMIZATION_OPTIONS = -O3

BUILD_DIR = build
TARGET_BOX ?= bf0-app
#TARGET_BOX ?= lc2-app
include $(LIBWTV_INSTALL_DIR)/include/wtv.mk

OBJS = $(BUILD_DIR)/poopies.o

poopies.wtv.o: N64_ROM_TITLE = "Timer Test"

$(BUILD_DIR)/poopies.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR) *.wtv.o
.PHONY: clean

-include $(wildcard $(BUILD_DIR)/*.d)
