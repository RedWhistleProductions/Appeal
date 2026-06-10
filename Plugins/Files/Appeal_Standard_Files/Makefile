PLUGIN_NAME := Appeal_Standard_Files
SRC_DIR := ./src
OBJ_DIR := ./obj

ifeq ($(OS),Windows_NT)
CXX ?= g++
	TARGET := $(PLUGIN_NAME).dll
	CXXFLAGS := -std=c++17 -Wall
	LDFLAGS := -shared
else
CXX ?= g++
	TARGET := $(PLUGIN_NAME).so
	CXXFLAGS := -std=c++17 -fPIC -Wall
	LDFLAGS := -shared
endif

RUNTIME_DIR := ../../../Resources/Modules/Files/$(PLUGIN_NAME)
SRCS := $(SRC_DIR)/main.cpp
OBJS := $(OBJ_DIR)/main.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p "$(RUNTIME_DIR)"
	cp "$(TARGET)" "$(RUNTIME_DIR)/$(TARGET)"

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)"

.PHONY: all install clean
