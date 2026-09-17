BIN := test_all
BLD_DIR := build
COMPCOM := compile_commands.json

SRC_DIR := src
TEST_DIR := test
USRC_DIR := unity

CC := clang

CFLAGS := -g -O0 -std=gnu23 -Wall -Wextra -fcolor-diagnostics
CPPFLAGS := -I. -Iinc -I$(USRC_DIR) -I$(TEST_DIR) -Iinc/partty
DFLAGS := -DUNITY_OUTPUT_COLOR -DUNITY_FIXTURE_NO_EXTRAS
DEPFLAGS := -MMD -MP
LDFLAGS := -lunibilium

SRCS := $(wildcard $(SRC_DIR)/*.c)
SRCS += $(wildcard $(SRC_DIR)/partty/*.c)
SRCS += $(wildcard $(TEST_DIR)/*.c)
SRCS += $(wildcard $(USRC_DIR)/*.c)

OBJS := $(patsubst %.c,$(BLD_DIR)/%.o,$(SRCS))
DEPS := $(patsubst %.c,$(BLD_DIR)/%.d,$(SRCS))

COMP_FLAGS := $(CFLAGS) $(CPPFLAGS) $(DFLAGS) $(DEPFLAGS)

.PHONY: all
all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(COMP_FLAGS) $(LDFLAGS) $^ -o $@

$(BLD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(COMP_FLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BIN) $(BLD_DIR) $(COMPCOM)

-include $(DEPS)
