# Playlist Editor

Run standalone:

```text
Appeal.exe Utilities/Game_Editor/Playlist_Editor/Main
```

In the Level Editor, click **Playlist Editor** in Components. Its two dockable panels show the selected level's playlist and a recursive Music Library. The level name and existing-level selection in Level Editor determine the playlist being edited, including when a Tile Map Editor draft is open. Selecting Level Editor or Tile Map Editor hides both playlist panels. Either panel's title-bar X also hides both panels; click Playlist Editor in Components to reopen them.

Drag songs from Music Library into the playlist. Drop on a row to insert, or on **Drop songs here to append** to add at the end. Drag playlist rows to reorder, and click the X at the start of a row to remove that song. Every edit saves automatically; Save playlist also saves an empty list. Clear playlist empties only the selected level's list. Rescan music refreshes the cached recursive inventory after adding files. Supported library extensions are MP3, OGG, WAV, FLAC, IT, MOD, and XM.

The standalone utility opens the same `Tile_Editor` database as Level Editor. Enter a level name and click **Open level playlist** to select it. Show Playlist Editor reopens closed panels in the standalone utility. The utility owns its own window/loop; the reusable Editor script does not.

## Persistence and playback

`Setup/Playlists` creates this table in the currently selected database:

```sql
CREATE TABLE Playlists (
    Level_Name TEXT PRIMARY KEY,
    File TEXT NOT NULL UNIQUE,
    Updated_At TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
);
```

Level and Tile editors currently store their designs in `Resources/Storage/Tile_Editor.appealdb`. Their playlist links live there too. `Setup/Test_Game` also creates the same table for game data. No level placement or tile-map data is changed by editing playlists.

`File` is relative to `Resources/Audio/Playlist`. New playlists use `Levels/Level_<hex UTF-8 level name>.txt`, so spaces, apostrophes, path separators, and case differences in level names remain safe and distinct. Links are recorded only after a successful file save. Existing links are reused. Unedited levels do not need playlist records.

Files use Audio's existing format, one entry per line:

```text
"Song title" Subfolder/Song File.mp3
```

Song paths are relative to `Resources/Audio/Music`, and titles containing quotes are saved with apostrophes. Duplicate songs are allowed. Load a selected level's playlist in a game after selecting its database:

```text
Set_Var Level_Playlist (Storage Get_Field Playlists Level_Name Var Level_Name File)
If Var Level_Playlist != ""
{
    Audio Load_Play_List Var Level_Playlist
}
```

## Listening to music

**Play** starts the current level playlist, **Stop** stops it, and **Next** / **Back** step through its songs. The **Volume (%)** slider adjusts music volume from 0 to 100. The current song is displayed above the controls. Playback automatically advances through the playlist.

To audition music before adding it, select a song in Music Library and click **Play selected song**. Stop ends that preview; the playlist controls return to the level playlist. Closing the Playlist Editor, selecting another editor or level, or editing the playlist stops playback. Reopening and playing uses the current saved song order, without accumulating duplicate tracks.

The component loads the SDL2 Audio module when playback or volume controls are first used. For a custom audio resource root, call `PE_Init_Audio` and configure `Audio Set_Paths` before playback. The host should call `PE_Stop` on exit, as the bundled standalone and Level Editor hosts do.

## Embedding

Load Graphics and Storage and select the host's database, then:

```text
Run Utilities/Game_Editor/Playlist_Editor/Editor
Set_Var PE_Next_Level My_Level
```

Each frame, between the host's `Graphics GUI_Begin_Frame` and `Graphics Render`, call `PE_GUI`. Set `PE_Next_Level` before this call when the selected level changes. The component's `PE_Level` remains bound to the displayed list throughout that frame, so queued GUI callbacks save to the correct level. Call `PE_Save` to save explicitly, or `PE_Clear` to save an empty playlist.

The host owns module loading, the selected Storage connection, resource paths, GUI frame, window, input focus, rendering, and cleanup. Keep its database selected while drawing the component or running its callbacks.

All editor panels have title-bar X buttons. Use the **Windows** menu to reopen a closed panel or **Restore all windows** to restore them all. Closing a panel preserves its draft data; closing either playlist panel also stops playback.
