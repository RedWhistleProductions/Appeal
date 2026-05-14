# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -shared -fPIC

# Output file
TARGET_LINUX = Test_Audio.so
TARGET_WINDOWS = Test_Audio.dll

# Source files
SRCS = ./src/Main.cpp

# Object files directory
OBJDIR = ./obj
OBJS = $(SRCS:./src/%.cpp=$(OBJDIR)/%.o)

# Detect operating system
ifeq ($(OS),Windows_NT)
	TARGET = $(TARGET_WINDOWS)
	COMPILER = x86_64-w64-mingw32-g++
	LINKFLAGS = -lmingw32
else
	TARGET = $(TARGET_LINUX)
	COMPILER = $(CXX)
	LINKFLAGS =
endif

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJS)
	$(COMPILER) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LINKFLAGS)

# Compile source files to object files
$(OBJDIR)/%.o: ./src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS) $(OBJDIR)/*.o

.PHONY: all clean