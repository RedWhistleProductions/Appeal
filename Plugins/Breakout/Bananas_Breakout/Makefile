CXX ?= g++
CXXFLAGS = -shared -fPIC -Wall

PLUGIN_NAME = Bananas_Breakout
TARGET_LINUX = $(PLUGIN_NAME).so
TARGET_WINDOWS = $(PLUGIN_NAME).dll
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
	@mkdir -p ../../../Resources/Modules/Breakout/$(PLUGIN_NAME)
	cp $(TARGET) ../../../Resources/Modules/Breakout/$(PLUGIN_NAME)/$(TARGET)

clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS) $(OBJDIR)/*.o
	rm -rf $(OBJDIR)

.PHONY: all install clean
