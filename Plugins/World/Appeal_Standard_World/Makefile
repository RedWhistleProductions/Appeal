# Build the default in-memory backend for the Appeal World module.

SRC_DIR := ./src
OBJ_DIR := ./obj
PLUGIN_NAME := Appeal_Standard_World
SRCS := $(SRC_DIR)/main.cpp
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

ifeq ($(OS),Windows_NT)
CXX ?= g++
	TARGET := $(PLUGIN_NAME).dll
	CXXFLAGS := -std=c++17 -Wall
	LDFLAGS := -shared
	RUNTIME_DIR := ../../../Resources/Modules/World/$(PLUGIN_NAME)
else
CXX ?= g++
	TARGET := $(PLUGIN_NAME).so
	CXXFLAGS := -std=c++17 -fPIC -Wall
	LDFLAGS := -shared
	RUNTIME_DIR := ../../../Resources/Modules/World/$(PLUGIN_NAME)
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p "$(RUNTIME_DIR)"
	cp "$(TARGET)" "$(RUNTIME_DIR)/$(TARGET)"

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)"

.PHONY: all install clean
