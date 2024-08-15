CC = gcc
CFLAGS = -Iinclude
LIBS = -lSDL2_ttf -lSDL2 -lSDL2_image -lm
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
TARGET = $(BIN_DIR)/run
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Build the main executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

# Compile source files and generate dependencies
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -MMD -c $< -o $@ $(CFLAGS)

# Include dependencies
-include $(DEPS)

# Create necessary directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: clean
