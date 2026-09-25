BUILD := build
BIN   := $(BUILD)/test_all

SRC_DIR  := src
TEST_DIR := test
USRC_DIR := unity

CC       := clang
CFLAGS   := -g -O0 -std=gnu23 -Wall -Wextra -fcolor-diagnostics
CPPFLAGS := -I. -I$(SRC_DIR) -I$(USRC_DIR) -I$(TEST_DIR)
CPPFLAGS += -D_GNU_SOURCE -DUNITY_OUTPUT_COLOR -DUNITY_FIXTURE_NO_EXTRAS
DEPFLAGS := -MMD -MP

CC_FLAGS := $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS)

SRCS := $(wildcard $(TEST_DIR)/*.c)
SRCS += $(wildcard $(USRC_DIR)/*.c)

OBJS := $(patsubst %.c,$(BUILD)/%.o,$(SRCS))
DEPS := $(patsubst %.c,$(BUILD)/%.d,$(SRCS))

.PHONY: all clean amalgamate

all:
	lua amalgamate.lua
	$(MAKE) $(BIN)
	valgrind --leak-check=full --quiet ./build/test_all

$(BIN): $(OBJS)
	$(CC) $^ -o $@

linc_tools.h:


$(BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD) linc_tools.h

-include $(DEPS)
