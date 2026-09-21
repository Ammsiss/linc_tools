LT_MAKE := $(lastword $(MAKEFILE_LIST))
LT_ROOT := $(patsubst %/,%,$(dir $(abspath $(LT_MAKE))))

# Most includes
LT_INC_DIR := $(LT_ROOT)/inc

# Core
LT_DARR_SRC := $(LT_ROOT)/src/darr.c
LT_DSTR_SRC := $(LT_ROOT)/src/dstr.c
LT_ARGS_SRC := $(LT_ROOT)/src/args.c
LT_LLOG_SRC := $(LT_ROOT)/src/llog.c
LT_XFUNCS_SRC := $(LT_ROOT)/src/xfuncs.c
LT_HDA_SRC := $(LT_ROOT)/src/hda.c
LT_KVAL_SRC := $(LT_ROOT)/src/kval.c

LT_SRC_CORE := \
	$(LT_DARR_SRC) \
	$(LT_DSTR_SRC) \
	$(LT_ARGS_SRC) \
	$(LT_LLOG_SRC) \
	$(LT_XFUNCS_SRC) \
	$(LT_HDA_SRC) \
	$(LT_KVAL_SRC)

# UI split manager
LT_SPLITTY_SRC := $(LT_ROOT)/src/splitty.c

# PTY input parser
LT_PARTTY_SRC := $(LT_ROOT)/src/partty/partty.c
LT_PARTTY_INC := $(LT_ROOT)/inc/partty

