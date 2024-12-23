
ifneq ("$(wildcard $(CURDIR)/external/heyigor/miniBAE/minibae/Makefile.libwtv)","")
	include $(CURDIR)/external/heyigor/miniBAE/minibae/Makefile.libwtv
	LIBWTV_OBJS += $(BUILD_DIR)/audio/minibae.o
	LIBWTV_OBJS += libminibae-$(WTVLIB_FILE_SUFFIX).a
	INSTALL_OBJS += install-minibae
	CLEAN_OBJS += clean-minibae
	INC += $(MINIBAE_INC)
else
$(error miniBAE with a libwtv Makefile not found. Did you clone the libwtv repo with --recurse-submodules? I need the external/heyigor/miniBAE/minibae/Makefile.libwtv file. The repo can be found at https://github.com/wtvemac/miniBAE)
endif
