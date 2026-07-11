# Compiler
CXX ?= g++
WINDRES ?= windres

# Compiler flags
CXXFLAGS = -g -Wall -pthread
LDFLAGS =
LDLIBS =

# Directories
SRC_DIR = ./Source
OBJ_ROOT = ./Obj
BIN_DIR = .
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib/appeal
DATADIR ?= $(PREFIX)/share/appeal
DESTDIR ?=

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
RES_FILES =

# Executable
ifeq ($(OS),Windows_NT)
	EXE_EXT = .exe
	OBJ_DIR = $(OBJ_ROOT)/Windows
	RC_FILES = $(wildcard $(SRC_DIR)/*.rc)
	RES_FILES = $(patsubst $(SRC_DIR)/%.rc, $(OBJ_DIR)/%.res, $(RC_FILES))
else
	EXE_EXT =
	OBJ_DIR = $(OBJ_ROOT)/Linux
	LDLIBS += -ldl
endif
TARGET = $(BIN_DIR)/Appeal$(EXE_EXT)

# Default target
all: $(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)$(LIBDIR)
	mkdir -p $(DESTDIR)$(BINDIR)
	mkdir -p $(DESTDIR)$(DATADIR)
	cp $(TARGET) $(DESTDIR)$(LIBDIR)/Appeal$(EXE_EXT)
	cp -R Resources $(DESTDIR)$(DATADIR)/
	printf '#!/bin/sh\nAPPEAL_RESOURCE_FOLDER="$${APPEAL_RESOURCE_FOLDER:-$(DATADIR)/Resources}" exec "$(LIBDIR)/Appeal$(EXE_EXT)" "$$@"\n' > $(DESTDIR)$(BINDIR)/appeal
	chmod +x $(DESTDIR)$(BINDIR)/appeal

# Link object files to create the executable
$(TARGET): $(OBJ_FILES) $(RES_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Compile Windows resource files
$(OBJ_DIR)/%.res: $(SRC_DIR)/%.rc
	@mkdir -p $(OBJ_DIR)
	$(WINDRES) $< -O coff -o $@

# Clean up
clean:
	rm -rf $(OBJ_ROOT) $(TARGET)

.PHONY: all install clean
