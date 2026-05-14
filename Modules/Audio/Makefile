# Detect the OS
UNAME_S := $(shell uname -s)

# Compiler and flags
ifeq ($(UNAME_S),Linux)
	CXX = g++
	CXXFLAGS = -shared -fPIC
	TARGET = Audio.so
else
	CXX = x86_64-w64-mingw32-g++
	CXXFLAGS = -shared
	TARGET = Audio.dll
endif

# Source files
SRCS = ./src/main.cpp ./src/Data_Source.cpp ./src/Plugin.cpp

# Object files directory
OBJDIR = ./obj

# Object files
OBJS = $(SRCS:./src/%.cpp=$(OBJDIR)/%.o)

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile
$(OBJDIR)/%.o: ./src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OBJDIR)
