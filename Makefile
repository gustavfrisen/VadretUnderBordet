CC := gcc
CFLAGS := -g -Wall -Wextra -std=c11 -MMD -MP  -Wno-format-truncation  -Wno-unused-parameter -Wno-unused-function 
LFLAGS := -lcurl

# Directories
SRC_DIR := src
JANSSON_DIR := src/libs/jansson
BUILD_DIR := build
CACHE_DIR := cache
INC_DIR := include

# Include paths
CFLAGS += -I$(INC_DIR) -I$(JANSSON_DIR)

# Source files
PROJECT_SRC := $(shell find $(SRC_DIR) -name '*.c' -not -path '$(JANSSON_DIR)/*')
JANSSON_SRC := $(wildcard $(JANSSON_DIR)/*.c)
ALL_SRC := $(PROJECT_SRC) $(JANSSON_SRC)

# Object files
PROJECT_OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(PROJECT_SRC))
JANSSON_OBJ := $(patsubst $(JANSSON_DIR)/%.c,$(BUILD_DIR)/jansson/%.o,$(JANSSON_SRC))
OBJ := $(PROJECT_OBJ) $(JANSSON_OBJ)

# Dependency files
DEP := $(OBJ:.o=.d)

# Final executable
BIN := $(BUILD_DIR)/weatherapi

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LFLAGS)

# Rule for project source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for jansson source files
$(BUILD_DIR)/jansson/%.o: $(JANSSON_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(BIN)
	./$(BIN)

clean:
	$(RM) -rf $(BUILD_DIR) $(CACHE_DIR)

-include $(DEP)

.PHONY: all run clean