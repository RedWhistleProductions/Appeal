# Build the SDL2 backend as an Appeal Input plugin.
#
# Plugin naming convention:
#   Appeal_<BackendLibrary>_<Module>
#
# This builds:
#   Appeal_SDL2_Input.dll on Windows
#   Appeal_SDL2_Input.so  on Linux

SRC_DIR := ./src
OBJ_DIR := ./obj
SRCS := $(SRC_DIR)/main.cpp
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

PLUGIN_NAME := Appeal_SDL2_Input

ifeq ($(OS),Windows_NT)
CXX ?= g++
	TARGET := $(PLUGIN_NAME).dll
	SDL_ROOT ?= ../../../Third_Party/SDL2/Windows/x86_64
	SDL_INCLUDE ?= $(SDL_ROOT)/include
	SDL_LIB ?= $(SDL_ROOT)/lib
	SDL_BIN ?= $(SDL_ROOT)/bin
	ifeq ($(wildcard $(SDL_INCLUDE)/SDL2/SDL.h),)
		ifneq ($(wildcard C:/3rd_Party_Libs/Include/SDL2/SDL.h),)
			SDL_INCLUDE := C:/3rd_Party_Libs/Include
		endif
	endif
	ifeq ($(wildcard $(SDL_LIB)/libSDL2.dll.a),)
		ifneq ($(wildcard C:/3rd_Party_Libs/Lib/SDL2/libSDL2.dll.a),)
			SDL_LIB := C:/3rd_Party_Libs/Lib/SDL2
		endif
	endif
	CPPFLAGS := -I$(SDL_INCLUDE) -I$(SDL_INCLUDE)/SDL2
	CXXFLAGS := -Wall
	LDFLAGS := -shared -L$(SDL_LIB)
	LDLIBS := -lSDL2
	RUNTIME_DIR := ../../../Resources/Modules/Input/$(PLUGIN_NAME)
else
CXX ?= g++
	SDL2_CONFIG ?= sdl2-config
	TARGET := $(PLUGIN_NAME).so
	CPPFLAGS := $(shell $(SDL2_CONFIG) --cflags)
	CXXFLAGS := -fPIC -Wall
	LDFLAGS := -shared
	LDLIBS := $(shell $(SDL2_CONFIG) --libs)
	RUNTIME_DIR := ../../../Resources/Modules/Input/$(PLUGIN_NAME)
endif

all: check-sdl $(TARGET)

ifeq ($(OS),Windows_NT)
check-sdl:
	@test -f "$(SDL_INCLUDE)/SDL2/SDL.h" || (echo "Missing $(SDL_INCLUDE)/SDL2/SDL.h"; exit 1)
	@test -f "$(SDL_LIB)/libSDL2.dll.a" || (echo "Missing $(SDL_LIB)/libSDL2.dll.a"; exit 1)
else
check-sdl:
	@$(SDL2_CONFIG) --version >/dev/null
endif

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p "$(RUNTIME_DIR)"
	cp "$(TARGET)" "$(RUNTIME_DIR)/$(TARGET)"
ifeq ($(OS),Windows_NT)
	cp "$(SDL_BIN)"/SDL2.dll ../../../
endif

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)"

.PHONY: all check-sdl install clean
