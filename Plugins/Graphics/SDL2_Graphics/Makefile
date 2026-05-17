# Build the SDL2 backend as an Appeal Graphics plugin.
#
# Plugin naming convention:
#   Appeal_<BackendLibrary>_<Module>
#
# This builds:
#   Appeal_SDL2_Graphics.dll on Windows
#   Appeal_SDL2_Graphics.so  on Linux

SRC_DIR := ./src
OBJ_DIR := ./obj
IMGUI_DIR ?= ../../../Third_Party/imgui
IMGUI_SRCS := \
	$(IMGUI_DIR)/imgui.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp \
	$(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp
SRCS := $(SRC_DIR)/main.cpp $(IMGUI_SRCS)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
OBJS := $(OBJS:$(IMGUI_DIR)/%.cpp=$(OBJ_DIR)/imgui/%.o)

PLUGIN_NAME := Appeal_SDL2_Graphics

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
	ASSIMP_ROOT ?= ../../../Third_Party/Assimp/Windows/x86_64
	ASSIMP_INCLUDE ?= $(ASSIMP_ROOT)/include
	ASSIMP_LIB ?= $(ASSIMP_ROOT)/lib
	ifeq ($(wildcard $(ASSIMP_INCLUDE)/assimp/Importer.hpp),)
		ifneq ($(wildcard C:/3rd_Party_Libs/Include/assimp/Importer.hpp),)
			ASSIMP_INCLUDE := C:/3rd_Party_Libs/Include
		endif
	endif
	ifeq ($(wildcard $(ASSIMP_LIB)/libassimp.dll.a),)
		ifneq ($(wildcard C:/3rd_Party_Libs/Lib/assimp/libassimp.dll.a),)
			ASSIMP_LIB := C:/3rd_Party_Libs/Lib/assimp
		endif
	endif
	CPPFLAGS := -I$(SDL_INCLUDE) -I$(SDL_INCLUDE)/SDL2 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
	ifneq ($(wildcard $(ASSIMP_INCLUDE)/assimp/Importer.hpp),)
	ifneq ($(wildcard $(ASSIMP_LIB)/libassimp.dll.a),)
		CPPFLAGS += -I$(ASSIMP_INCLUDE) -DAPPEAL_USE_ASSIMP
		LDFLAGS_ASSIMP := -L$(ASSIMP_LIB)
		LDLIBS_ASSIMP := -lassimp
	endif
	endif
	CXXFLAGS := -Wall
	LDFLAGS := -shared -L$(SDL_LIB) $(LDFLAGS_ASSIMP)
	LDLIBS := -lSDL2_image -lSDL2_ttf -lSDL2 $(LDLIBS_ASSIMP)
	RUNTIME_DIR := ../../../Resources/Modules/Graphics/$(PLUGIN_NAME)
else
CXX ?= g++
	SDL2_CONFIG ?= sdl2-config
	TARGET := $(PLUGIN_NAME).so
	ASSIMP_CFLAGS := $(shell pkg-config --cflags assimp 2>/dev/null)
	ASSIMP_LIBS := $(shell pkg-config --libs assimp 2>/dev/null)
	ifneq ($(ASSIMP_LIBS),)
		ASSIMP_DEFINE := -DAPPEAL_USE_ASSIMP
	endif
	CPPFLAGS := $(shell $(SDL2_CONFIG) --cflags) -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends $(ASSIMP_CFLAGS) $(ASSIMP_DEFINE)
	CXXFLAGS := -fPIC -Wall
	LDFLAGS := -shared
	LDLIBS := $(shell $(SDL2_CONFIG) --libs) -lSDL2_image -lSDL2_ttf $(ASSIMP_LIBS)
	RUNTIME_DIR := ../../../Resources/Modules/Graphics/$(PLUGIN_NAME)
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

$(OBJ_DIR)/imgui/%.o: $(IMGUI_DIR)/%.cpp
	@mkdir -p "$(dir $@)"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p "$(RUNTIME_DIR)"
	cp "$(TARGET)" "$(RUNTIME_DIR)/$(TARGET)"
ifeq ($(OS),Windows_NT)
	cp "$(SDL_BIN)"/SDL2.dll ../../../
	cp "$(SDL_BIN)"/SDL2_image.dll ../../../
	cp "$(SDL_BIN)"/SDL2_ttf.dll ../../../
endif

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)"

.PHONY: all check-sdl install clean
