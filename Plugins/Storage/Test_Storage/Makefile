CXX ?= g++
CXXFLAGS = -shared -fPIC -Wall

TARGET_LINUX = Test_Storage.so
TARGET_WINDOWS = Test_Storage.dll
SRCS = ./src/Main.cpp
OBJDIR = ./obj
OBJS = $(SRCS:./src/%.cpp=$(OBJDIR)/%.o)

ifeq ($(OS),Windows_NT)
	TARGET = $(TARGET_WINDOWS)
	COMPILER = $(CXX)
	LINKFLAGS = -lmingw32
else
	TARGET = $(TARGET_LINUX)
	COMPILER = $(CXX)
	LINKFLAGS =
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(COMPILER) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LINKFLAGS)

$(OBJDIR)/%.o: ./src/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p ../../../Resources/Modules/Storage/Test_Storage
	cp $(TARGET) ../../../Resources/Modules/Storage/Test_Storage/$(TARGET)

clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS) $(OBJDIR)/*.o
	rm -rf $(OBJDIR)

.PHONY: all install clean
