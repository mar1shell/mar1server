# --- Variables ---

# Compiler
CC = gcc

# Compiler Flags
# -Wall -Wextra = Show all warnings
# -g            = Include debugging symbols (for GDB)
# -Isrc         = Tell the compiler to look for headers in the 'src/' directory
# -MMD -MP      = Generate .d dependency files
CFLAGS = -Wall -Wextra -g -Iinclude -MMD -MP

# Linker Libraries
# -pthread = Needed for multi-threaded applications
LDLIBS = -pthread

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Executable Name
TARGET_EXEC = main
TARGET = $(BIN_DIR)/$(TARGET_EXEC)

# --- Files ---

# Find all .c files in the src/ directory and subdirectories
# e.g., src/main.c src/server/server.c src/http/http.c src/utils/helpers.c
SOURCES = $(shell find $(SRC_DIR) -name '*.c')

# Create a list of all object files in the obj/ directory
# Preserve relative paths from src/ directory
# e.g., obj/main.o obj/server/server.o obj/http/http.o obj/utils/helpers.o
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Create a list of all .d dependency files
# e.g., obj/main.d obj/server.d obj/http.d
DEPS = $(OBJECTS:.o=.d)

# --- Rules ---

# The default rule, 'all', depends on the final executable
.PHONY: all
all: $(TARGET)

# Rule to link the final executable
# Depends on all object files
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking executable..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)
	@echo "Build complete: $@"

# Pattern rule to compile .c files from src/ (and subdirectories) into .o files in obj/
# The dependency on header files is now handled automatically
# by including the .d files (see bottom of file)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c -o $@ $<

# Rule to clean up compiled files
.PHONY: clean
clean:
	@echo "Cleaning up compiled files..."
	# This removes the entire obj/ directory,
	# which includes all .o (object) and .d (dependency) files
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)

# Rule to run the server
.PHONY: run
run: $(TARGET)
	@echo "Starting server..."
	./$(TARGET)

# --- Dependency Inclusion ---

# Include all .d files.
# The '-' tells 'make' not to error if the files don't exist
# (which they won't on the very first 'make' command).
# These files contain the rules that tell 'make' which headers
# each object file depends on.
-include $(DEPS)