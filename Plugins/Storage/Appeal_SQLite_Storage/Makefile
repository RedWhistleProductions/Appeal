# Build the default SQLite backend for the Appeal Storage module.

SRC_DIR := ./src
OBJ_DIR := ./obj
SQLITE_DIR := ../../../Third_Party/SQLite

PLUGIN_NAME := Appeal_SQLite_Storage
SRCS := $(SRC_DIR)/main.cpp $(SQLITE_DIR)/sqlite3.c
OBJS := $(SRC_DIR)/main.cpp $(SQLITE_DIR)/sqlite3.c

ifeq ($(OS),Windows_NT)
CXX ?= g++
CC ?= gcc
	TARGET := $(PLUGIN_NAME).dll
	CXXFLAGS := -std=c++17 -Wall -I$(SQLITE_DIR)
	CFLAGS := -Wall -I$(SQLITE_DIR) -DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS5
	LDFLAGS := -shared
	RUNTIME_DIR := ../../../Resources/Modules/Storage/$(PLUGIN_NAME)
else
CXX ?= g++
CC ?= gcc
	TARGET := $(PLUGIN_NAME).so
	CXXFLAGS := -std=c++17 -fPIC -Wall -I$(SQLITE_DIR)
	CFLAGS := -fPIC -Wall -I$(SQLITE_DIR) -DSQLITE_THREADSAFE=1 -DSQLITE_ENABLE_FTS5
	LDFLAGS := -shared
	RUNTIME_DIR := ../../../Resources/Modules/Storage/$(PLUGIN_NAME)
endif

CPP_OBJS := $(OBJ_DIR)/main.o
C_OBJS := $(OBJ_DIR)/sqlite3.o

all: $(TARGET)

$(TARGET): $(CPP_OBJS) $(C_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/sqlite3.o: $(SQLITE_DIR)/sqlite3.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

install: all
	@mkdir -p "$(RUNTIME_DIR)"
	cp "$(TARGET)" "$(RUNTIME_DIR)/$(TARGET)"

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)"

.PHONY: all install clean
