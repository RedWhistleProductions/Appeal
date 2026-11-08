# Splash / Title Screen Editor

Choose **Splash / Title Screens** in the Bananas Game Editor Components toolbar.
Create/open a game first. The component shares Editing View and Editor Controls
with the other editors, retaining unsaved settings when switching components.

Standalone launch:

```text
Appeal.exe "Utilities/Game_Editor/Screen_Editor/Main"
```

The standalone component selects or creates games from the same database.

## Settings

- **Splash image**: an image displayed at the start of the test.
- **Title image**: the image displayed after the transition.
- **Splash SFX**: optional sound effect played once when the splash starts.
- **Theme song**: optional music file started when the fade into the title begins.
- **Splash duration (seconds)**: how long the splash stays fully visible before
  transitioning. Accepts fractional values from 0 to 86400; the default is 3.

Use Browse to select assets from the Resources tree, or enter a filesystem path
directly. Images can be PNG, JPEG, BMP, or other formats supported by SDL_image;
audio support follows the installed SDL_mixer codecs. Empty audio paths mean
silence. Both images are needed for Test. Files remain external references;
moving them requires updating the paths.

**Save screen settings** stores the configuration per game in
`Editor_Game_Screens` in the existing `Tile_Editor.appealdb` database. Save or
explicitly allow discarding changes before switching games.

## Testing

**Test** previews current settings, including unsaved edits, in Editing View.
The image fills that view. Full-screen source images are stretched to the view's
aspect ratio. The splash holds for the chosen duration, then the title fades
over it in a one-second crossfade while the theme starts. The title stays visible
until **Stop test**, another test, or a component/game change. Music plays once;
the title remains visible after it ends. Test does not save settings.

**Show splash** and **Show title** inspect the individual images silently.
Editing settings stops an active test. Leaving this component or closing the
application stops its music and SFX. Alt+Enter toggles the shared application
window between windowed and fullscreen modes.

## Embedding

`Editor.Appeal` assumes Graphics, Files, and Storage are loaded and the host's
database is selected. It defines functions without creating a window or loop.
Audio loads lazily when a configured test needs it.

Set `SS_Next_Game` and call `SS_Use_Game` to select a saved game. Compose `SS_View`
inside Editing View and call `SS_GUI` for Editor Controls and the optional file
picker. Call `SS_Update` once per host frame while selected, and `SS_Stop` on
component changes and shutdown. `SS_Dirty` indicates unsaved settings.

The preview owns Audio playback while testing; it stops other playlist playback
and marks the embedded Playlist Editor for reloading its playlist afterward.
`SS_Phase` is Splash, Fade, Title, or Stopped; `SS_Blend` is the title opacity.
`SS_Step` accepts `SS_Now` in milliseconds for deterministic phase tests.
