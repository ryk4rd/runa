# Variables
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS =
TARGET = nova
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
DEPS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.d,$(SOURCES))

# Phony Targets
.PHONY: all clean

# Default Target
all: $(BIN_DIR)/$(TARGET)

# Link Object Files to Create Executable
$(BIN_DIR)/$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

# Compile Source Files to Object Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -MMD -MP

# Create Directories if They Don't Exist
$(BIN_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

# Include Dependency Files
-include $(DEPS)

# Clean Up Build Artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
