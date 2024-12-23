
ifneq ("$(wildcard $(CURDIR)/external/heyigor/miniBAE/minibae/Makefile.libwtv)","")
	include $(CURDIR)/external/heyigor/miniBAE/minibae/Makefile.libwtv
	LIBWTV_OBJS += libminibae-$(WTVLIB_FILE_SUFFIX).a
	INSTALL_OBJS += install-minibae
	CLEAN_OBJS += clean-minibae
	INC += $(MINIBAE_INC)
endif
