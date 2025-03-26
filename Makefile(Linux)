# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -g -Wall

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