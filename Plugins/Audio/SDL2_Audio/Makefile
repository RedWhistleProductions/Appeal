# Build the SDL2 backend as an Appeal Audio plugin.
#
# Plugin naming convention:
#   Appeal_<BackendLibrary>_<Module>
#
# This builds:
#   Appeal_SDL2_Audio.dll on Windows
#   Appeal_SDL2_Audio.so  on Linux
#
# Windows expects the SDL2 MinGW development packages under:
#   ../../../Third_Party/SDL2/Windows/x86_64
#
# Required Windows layout:
#   include/SDL2/SDL.h
#   include/SDL2/SDL_mixer.h
#   lib/libSDL2.dll.a
#   lib/libSDL2_mixer.dll.a
#   bin/SDL2.dll
#   bin/SDL2_mixer.dll

UNAME_S := $(shell uname -s)

SRC_DIR := ./src
OBJ_DIR := ./obj
SRCS := $(SRC_DIR)/main.cpp $(SRC_DIR)/File_IO.cpp
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

PLUGIN_NAME := Appeal_SDL2_Audio

ifeq ($(OS),Windows_NT)
	CXX := x86_64-w64-mingw32-g++
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
	CPPFLAGS := -I$(SDL_INCLUDE)
	CXXFLAGS := -Wall
	LDFLAGS := -shared -L$(SDL_LIB)
	LDLIBS := -lSDL2_mixer -lSDL2
	RUNTIME_DIR := ../../../Resources/Modules/Audio/$(PLUGIN_NAME)
else
	CXX ?= g++
	SDL2_CONFIG ?= sdl2-config
	TARGET := $(PLUGIN_NAME).so
	CPPFLAGS := $(shell $(SDL2_CONFIG) --cflags)
	CXXFLAGS := -fPIC -Wall
	LDFLAGS := -shared
	LDLIBS := $(shell $(SDL2_CONFIG) --libs) -lSDL2_mixer
	RUNTIME_DIR := ../../../Resources/Modules/Audio/$(PLUGIN_NAME)
endif

all: check-sdl $(TARGET)

ifeq ($(OS),Windows_NT)
check-sdl:
	@test -f "$(SDL_INCLUDE)/SDL2/SDL.h" || (echo "Missing $(SDL_INCLUDE)/SDL2/SDL.h"; exit 1)
	@test -f "$(SDL_INCLUDE)/SDL2/SDL_mixer.h" || (echo "Missing $(SDL_INCLUDE)/SDL2/SDL_mixer.h"; exit 1)
	@test -f "$(SDL_LIB)/libSDL2.dll.a" || (echo "Missing $(SDL_LIB)/libSDL2.dll.a"; exit 1)
	@test -f "$(SDL_LIB)/libSDL2_mixer.dll.a" || (echo "Missing $(SDL_LIB)/libSDL2_mixer.dll.a"; exit 1)
else
check-sdl:
	@sdl2-config --version >/dev/null
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
	cp "$(SDL_BIN)"/*.dll ../../../
endif

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)" SDL2_Audio.dll SDL2_Audio.so

.PHONY: all check-sdl install clean
