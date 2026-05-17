# Build the SDL2_net backend as an Appeal Network plugin.
#
# Plugin naming convention:
#   Appeal_<BackendLibrary>_<Module>
#
# This builds:
#   Appeal_SDL2_Network.dll on Windows
#   Appeal_SDL2_Network.so  on Linux

SRC_DIR := ./src
OBJ_DIR := ./obj
SRCS := $(SRC_DIR)/Main.cpp
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

PLUGIN_NAME := Appeal_SDL2_Network

ifeq ($(OS),Windows_NT)
	CXX := x86_64-w64-mingw32-g++
	TARGET := $(PLUGIN_NAME).dll
	SDL_ROOT ?= ../../../Third_Party/SDL2/Windows/x86_64
	SDL_INCLUDE ?= $(SDL_ROOT)/include
	SDL_LIB ?= $(SDL_ROOT)/lib
	SDL_BIN ?= $(SDL_ROOT)/bin
	ifeq ($(wildcard $(SDL_INCLUDE)/SDL2/SDL_net.h),)
		ifneq ($(wildcard C:/3rd_Party_Libs/Include/SDL2/SDL_net.h),)
			SDL_INCLUDE := C:/3rd_Party_Libs/Include
		endif
	endif
	ifeq ($(wildcard $(SDL_LIB)/libSDL2_net.dll.a),)
		ifneq ($(wildcard C:/3rd_Party_Libs/Lib/SDL2/libSDL2_net.dll.a),)
			SDL_LIB := C:/3rd_Party_Libs/Lib/SDL2
		endif
	endif
	CPPFLAGS := -I$(SDL_INCLUDE) -I$(SDL_INCLUDE)/SDL2
	CXXFLAGS := -Wall
	LDFLAGS := -shared -L$(SDL_LIB)
	LDLIBS := -lSDL2_net -lSDL2
	RUNTIME_DIR := ../../../Resources/Modules/Network/$(PLUGIN_NAME)
else
	CXX ?= g++
	SDL2_CONFIG ?= sdl2-config
	TARGET := $(PLUGIN_NAME).so
	CPPFLAGS := $(shell $(SDL2_CONFIG) --cflags)
	CXXFLAGS := -fPIC -Wall
	LDFLAGS := -shared
	LDLIBS := $(shell $(SDL2_CONFIG) --libs) -lSDL2_net
	RUNTIME_DIR := ../../../Resources/Modules/Network/$(PLUGIN_NAME)
endif

all: check-sdl-net $(TARGET)

ifeq ($(OS),Windows_NT)
check-sdl-net:
	@test -f "$(SDL_INCLUDE)/SDL2/SDL_net.h" || (echo "Missing $(SDL_INCLUDE)/SDL2/SDL_net.h"; exit 1)
	@test -f "$(SDL_LIB)/libSDL2_net.dll.a" || (echo "Missing $(SDL_LIB)/libSDL2_net.dll.a"; exit 1)
else
check-sdl-net:
	@test -f /usr/include/SDL2/SDL_net.h || pkg-config --exists SDL2_net || (echo "Missing SDL2_net development files"; exit 1)
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
	if test -f "$(SDL_BIN)/SDL2_net.dll"; then cp "$(SDL_BIN)"/SDL2_net.dll ../../../; fi
	if test -f "$(SDL_BIN)/SDL2.dll"; then cp "$(SDL_BIN)"/SDL2.dll ../../../; fi
endif

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)"

.PHONY: all check-sdl-net install clean
