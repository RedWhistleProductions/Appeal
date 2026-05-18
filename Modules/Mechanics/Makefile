# Detect the OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
CXX ?= g++
	CXXFLAGS = -shared -fPIC -Wall
	TARGET = Mechanics.so
else
CXX ?= g++
	CXXFLAGS = -shared -Wall
	TARGET = Mechanics.dll
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
	@mkdir -p ../../Resources/Modules/Mechanics
	cp $(TARGET) ../../Resources/Modules/Mechanics/$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OBJDIR)

.PHONY: all install clean
