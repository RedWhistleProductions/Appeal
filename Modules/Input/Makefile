# Detect the OS
UNAME_S := $(shell uname -s)

# Compiler and flags
ifeq ($(UNAME_S),Linux)
	CXX = g++
	CXXFLAGS = -shared -fPIC -Wall
	TARGET = Input.so
else
	CXX = x86_64-w64-mingw32-g++
	CXXFLAGS = -shared -Wall
	TARGET = Input.dll
endif

SRCS = ./src/main.cpp ./src/Data_Source.cpp ./src/Plugin.cpp
OBJDIR = ./obj
OBJS = $(SRCS:./src/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: ./src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p ../../Resources/Modules/Input
	cp $(TARGET) ../../Resources/Modules/Input/$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OBJDIR)

.PHONY: all install clean
