# Appeal IDE â€” text editor

Choose **Appeal IDE** in Game Editor's Components toolbar. It is available before
a game is opened and keeps its document when switching components or games.

Standalone launch:

```text
Appeal.exe "Utilities/Game_Editor/Appeal_IDE/Main"
```

## Editing files

The text editor fills Editing View. It supports multiline text, Tab insertion,
selection, clipboard copy/paste, and the text control's undo/redo shortcuts.
UTF-8 text and large scripts are supported; this first version edits one document
at a time. Syntax highlighting, code completion, and script execution are future
IDE features.

- **New** starts an empty, untitled document.
- **Browse scripts...** opens the Resources tree to select a file.
- **Open path** opens the path entered in Editor Controls. Absolute filesystem
  paths and paths relative to the configured Resources folder are accepted.
- **Save** writes the current document back to its opened file. For an untitled
  document, it uses the path box.
- **Save As** writes to the path box and makes that file the current document.

Changing the path box does not redirect Save for an already opened document;
use Save As for that. The proposed untitled path is `Appeal/Untitled.Appeal`,
relative to Resources. Parent directories are created when saving a new file.

The current file and an unsaved-change indicator appear in Editor Controls.
New/Open require saving or explicitly checking **Discard unsaved text** when
the document is dirty. **Exit editor** closes immediately for an unchanged
document. With unsaved text, it opens an **Unsaved script** window offering
**Save and Exit**, **Discard and Exit**, and **Cancel**. A failed save keeps
the editor open with the text intact. Closing that prompt cancels the exit. Save As requires **Allow
overwriting another file** before replacing a different existing file. Those
checkboxes reset after a successful open/save/new operation. Failed opens retain
the document; failed saves retain its unsaved-change flag and text.

Drafts live in memory until saved. Switching components retains them. Plain text
is written without adding a trailing newline; opening and saving without edits
preserves the original bytes, including CRLF line endings. The text control may
normalize line endings when edits are made.

## Embedding

Load Graphics and Files, then run `Editor.Appeal`. It opens no windows and owns
no application loop. Compose `IDE_View` inside Editing View and call `IDE_GUI`
for Editor Controls and the optional browser. `IDE_Dirty` reports unsaved edits;
`IDE_Check_Draft` checks the explicit discard choice before replacing a document.
The component does not require Storage or game-specific database tables.

Graphics exposes the reusable `GUI_Text_Editor Name Label Value Callback`
control in both SDL2 OpenGL plugins. It fills the available GUI content area,
accepts tabs, and grows its input buffer during large edits/pastes. Existing
multiline form controls retain their normal Tab navigation behavior.

Validation: `python Tests/Appeal_IDE_Tests.py` covers UTF-8/CRLF round trips, empty
files, large GUI buffers, Save/Save As, overwrite and discard guards, failed IO,
file-picker opening, component switching, and standalone/host startup.


## Font appearance

Click **Font settings...** in Editor Controls to open the appearance window.
Choose Built-in, Consolas, Courier New, Arial, Segoe UI, or a custom font file;
set the size from 8 to 72 pixels, bold/italic faces, and text RGB color. Click
**Apply** to update the code pane, or **Reset font** to restore the built-in
13-pixel light-gray appearance. Closing the window keeps the applied appearance.

Preset families use installed Windows font files. Bold/italic choose real face
files, including combined bold italic. Requesting those styles while Built-in is
selected switches to Consolas. With a custom font, select the desired face file
directly; preset bold/italic checkboxes do not transform custom fonts. Browse
starts in `C:/Windows/Fonts`; other font paths may be typed in the custom-path box.
Unavailable or invalid files leave the previous appearance intact and show a
status message.

These are session appearance settings for the source pane only. They neither
modify script text nor mark it unsaved, and other component panels retain their
normal font. Changes remain while switching components; they reset when the app
restarts.

`Fonts.Appeal` supplies the settings GUI and invokes
`Graphics GUI_Text_Editor_Style Name File Size R G B`. The command validates font
faces and schedules atlas rebuilding between frames. Rendering checks:
`python Tests/IDE_Font_Render_Tests.py` verifies size/color pixels, failed font
retention, and reset with both OpenGL plugins.
