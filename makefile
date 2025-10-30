# --- Variables ---

# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -Wall -Wextra -g -Iinclude -MMD -MP

# Linker Libraries
LDLIBS = -pthread

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Executable Name
TARGET_EXEC = mar1server
TARGET = $(BIN_DIR)/$(TARGET_EXEC)

# --- Files ---

SOURCES = $(shell find $(SRC_DIR) -name '*.c')
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS = $(OBJECTS:.o=.d)

# --- Rules ---

.PHONY: all
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDLIBS)
	@echo "Build complete: $(TARGET)"

# Pattern rule to compile .c files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	@echo "Cleaning..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(DEPS)