CXX ?= g++
CXXFLAGS = -shared -fPIC -Wall

TARGET_LINUX = Test_AI.so
TARGET_WINDOWS = Test_AI.dll
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
	@mkdir -p ../../../Resources/Modules/AI/Test_AI
	cp $(TARGET) ../../../Resources/Modules/AI/Test_AI/$(TARGET)

clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS) $(OBJDIR)/*.o
	rm -rf $(OBJDIR)

.PHONY: all install clean
