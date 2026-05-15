# SDL2 Windows x86_64 Dependencies

This folder is for the Windows MinGW development packages used by SDL-based Appeal plugins.

Expected layout:

```text
include/
  SDL2/
    SDL.h
    SDL_mixer.h
lib/
  libSDL2.dll.a
  libSDL2_mixer.dll.a
bin/
  SDL2.dll
  SDL2_mixer.dll
```

The `include` and `lib` files are needed to compile plugins. The `bin` DLLs are needed at runtime.

For now, the SDL runtime DLLs should be copied beside `Appeal.exe`. The `Appeal_SDL2_Audio` plugin Makefile does this with:

```sh
make install
```

Download the matching MinGW development packages for SDL2 and SDL2_mixer, then copy their `include`, `lib`, and `bin` contents into this folder.

Appeal plugin DLLs should use this naming convention:

```text
Appeal_<BackendLibrary>_<Module>.dll
```

For example, the SDL2 audio plugin installs as:

```text
Resources/Modules/Audio/Appeal_SDL2_Audio/Appeal_SDL2_Audio.dll
```
