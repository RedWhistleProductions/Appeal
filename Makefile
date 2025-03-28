# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -g -Wall	-pthread

# Directories
SRC_DIR = ./Source
OBJ_DIR = ./Obj
BIN_DIR = .

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Executable
TARGET = $(BIN_DIR)/Appeal

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = del /Q
    MKDIR = if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
else
    RM = rm -rf
    MKDIR = mkdir -p $(OBJ_DIR)
endif

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean