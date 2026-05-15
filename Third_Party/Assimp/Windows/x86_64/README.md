# Assimp Windows x86_64 Dependencies

This folder is for the Windows MinGW build of Assimp used by SDL-based Appeal graphics plugins.

Expected layout:

```text
include/
  assimp/
    Importer.hpp
    scene.h
    postprocess.h
lib/
  libassimp.dll.a
bin/
  assimp*.dll
```

When these files are present, `Plugins/Graphics/SDL2_Graphics/Makefile` enables `APPEAL_USE_ASSIMP` automatically and links `Appeal_SDL2_Graphics.dll` against Assimp.

Without these files, the plugin still builds and uses its built-in text mesh loader plus a cube fallback.
