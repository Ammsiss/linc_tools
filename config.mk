LT_MAKE := $(lastword $(MAKEFILE_LIST))
LT_ROOT := $(patsubst %/,%,$(dir $(abspath $(LT_MAKE))))

LT_DARR_SRC := $(LT_ROOT)/src/darr.c
LT_DSTR_SRC := $(LT_ROOT)/src/dstr.c
LT_ARGS_SRC := $(LT_ROOT)/src/args.c
LT_LLOG_SRC := $(LT_ROOT)/src/llog.c
LT_SPLITTY_SRC := $(LT_ROOT)/src/termsplit.c
LT_XFUNCS_SRC := $(LT_ROOT)/src/xfuncs.c

LT_SRC_CORE := \
	$(LT_DARR_SRC) \
	$(LT_DSTR_SRC) \
	$(LT_ARGS_SRC) \
	$(LT_LLOG_SRC) \
	$(LT_XFUNCS_SRC)

LT_INC_DIR := $(LT_ROOT)/inc
