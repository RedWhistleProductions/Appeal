# Appeal Libraries

Reusable libraries written in Appeal live here, organized into subfolders by
purpose. Unlike standalone utilities, libraries supply functions for a host
script to use and normally do not own its window or application loop.

Load a library using its resource-relative name:

```text
Run Libraries/Game/Runtime
Call Game_Start
```

`Libraries/...` resolves beneath the configured resource folder, independently
of the working directory. The `.Appeal` suffix is optional. Use subfolders such
as `Game`, and document each library's inputs, outputs, dependencies, and naming
prefix to avoid collisions in Appeal's shared function/variable namespace.

Available libraries:

- `Game/Runtime`: game state, timing, events, and timers.
- `Game/Scenes`: scene transitions, reload/cleanup, and update/draw hooks.
- `Game/Menus`: renderer-independent menu selection and action events.
- `Game/Interface`: optional Graphics GUI adapter for menu entries.
- `Game/Saves`: named save slots, custom progress values, and settings.
