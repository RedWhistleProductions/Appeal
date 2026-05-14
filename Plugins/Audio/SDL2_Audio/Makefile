# Build SDL2_Audio as an Appeal Audio plugin.
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

ifeq ($(OS),Windows_NT)
	CXX ?= x86_64-w64-mingw32-g++
	TARGET := SDL2_Audio.dll
	SDL_ROOT ?= ../../../Third_Party/SDL2/Windows/x86_64
	SDL_INCLUDE := $(SDL_ROOT)/include
	SDL_LIB := $(SDL_ROOT)/lib
	CXXFLAGS := -shared -Wall -I$(SDL_INCLUDE)
	LDFLAGS := -L$(SDL_LIB) -lSDL2_mixer -lSDL2
	RUNTIME_DIR := ../../../Resources/Modules/Audio/SDL2_Audio
else
	CXX ?= g++
	TARGET := SDL2_Audio.so
	CXXFLAGS := -shared -fPIC -Wall $(shell sdl2-config --cflags)
	LDFLAGS := $(shell sdl2-config --libs) -lSDL2_mixer
	RUNTIME_DIR := ../../../Resources/Modules/Audio/SDL2_Audio
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
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

install: all
	@mkdir -p "$(RUNTIME_DIR)"
	cp "$(TARGET)" "$(RUNTIME_DIR)/$(TARGET)"
ifeq ($(OS),Windows_NT)
	cp "$(SDL_ROOT)"/bin/*.dll ../../../
endif

clean:
	rm -rf "$(OBJ_DIR)" "$(TARGET)"

.PHONY: all check-sdl install clean
