# Appeal Content System Architecture

## Purpose

This document captures the proposed architecture for Appeal/Bananas content loading, resource organization, scenario logic, save/live state, bytecode compilation, and portal-based world transitions.

The design goal is to keep Appeal readable for humans, efficient for machines, friendly to tools like Codex/Copilot, and flexible enough for commercial games, mods, DLC, online worlds, and third-party developers.

---

# 1. Core Design Principles

## 1.1 Files store source content

Large or author-created content should usually live in files:

- Appeal scripts
- Worlds
- Levels
- Scenarios
- Objectives
- Conditions
- Models
- Textures
- Audio
- Music
- Shaders
- Fonts
- Maps
- Scenes

Files are better for:

- version control
- modding
- editing tools
- packaging
- debugging
- portability

## 1.2 Databases store indexes and mutable state

Databases should not usually replace source files. Instead, they should store:

- asset indexes
- GUID lookups
- dependency metadata
- save state
- live online world state
- player state
- inventory
- quest/scenario progress
- runtime cache records
- marketplace/package information

Example:

```text
Asset ID: redwhistle.common.tree_oak_01
Type: Model
File: /Resources/Packages/RedWhistle/CommonNature/Assets/Models/tree_oak_01.glb
Tags: tree, nature, forest
```

The database points to files. It does not need to contain the full model, texture, script, or level source.

## 1.3 Game-owned assets and shared packages

Appeal should support both private game assets and shared reusable packages.

Recommended structure:

```text
/Resources
  /Appeal
    Main.Appeal
    /Setup
    /GUI
    /Test
    /Bananas
    /Games
      /GameName
        GameName.Appeal
        /Levels
        /LevelName
          LevelName.Appeal
          LevelName.Playlist
        /Worlds
        /Scenarios
        /Objectives
        /Conditions
        /Scripts
        /Data

  /Audio
    /Music
    /SFX

  /Graphics
    /Fonts
    /GUI
    /Icons
    /Images
    /Materials
    /Meshes
    /Shaders
    /Textures

  /Input
    /Bindings
    /Profiles

  /Modules
    /ModuleName
      ModuleName.dll or ModuleName.so
      /PluginName
        PluginName.dll or PluginName.so

  /Package
    /Cache
    /Extracted
    /Installed
    /LocalRepo

  /Storage
  /World
  /UserData
  /Cache
```

Rule:

```text
Games own private Appeal scripts and game data under `Resources/Appeal/Games`. Module-specific assets live under the matching module resource folder, such as `Resources/Audio` or `Resources/Graphics`.
Packages share reusable assets.
Databases index assets and store mutable state.
Files store source content.
```

A game should share assets by declared dependency, not merely by folder location.

Example:

```appeal
Game "BananaQuest"
UsesPackage "RedWhistle/CommonNature"
UsesPackage "RedWhistle/BananasCore"
Startup "main.appeal"
```

---

# 2. Worlds, Levels, Scenarios, Objectives, and Conditions

## 2.1 Concept hierarchy

The chosen hierarchy is:

```text
World
  -> Level / Scene data
  -> Portals
  -> Scenarios
       -> Objectives
            -> Conditions
```

## 2.2 World

A World is the physical or logical place where gameplay happens.

A World may contain:

- scene reference
- terrain
- map data
- spawn points
- default music
- environmental settings
- world objects
- portals
- scenario attachments

Example:

```appeal
World "Forest"
    Scene "forest.scene"
    Music "forest_theme"
    SpawnPoint "NorthGate", 100, 0, 50
    SpawnPoint "SouthGate", 0, 0, 100
    Portal "Forest_To_Town"
EndWorld
```

## 2.3 Scenario

A Scenario is a scoped gameplay container. It may represent:

- quest
- mission
- story chapter
- tutorial step group
- achievement chain
- event
- multiplayer game mode objective set
- world event
- level challenge

A Scenario has a scope argument when called or defined.

Possible scopes:

```text
Global      whole game / main story
World       one world/map/region
Level       one level instance
Player      one player
Party       shared player group
Session     current online match/server instance
Temporary   short-lived event
```

A Scenario is complete when all required Objectives are complete.

Optional Objectives do not block Scenario completion, but they may trigger bonus callbacks.

Example:

```appeal
Scenario "Forest_Trial", Scope "Level"
    RequireObjective "Collect_100_Coins"
    RequireObjective "Defeat_Forest_Boss"
    OptionalObjective "Find_Secret_Room"
    OnComplete "Unlock_Next_Level"
EndScenario
```

One-line form may also be supported:

```appeal
Define_Scenario "Forest_Trial", "Level", ["Collect_100_Coins", "Defeat_Forest_Boss"], ["Find_Secret_Room"], "Unlock_Next_Level"
```

## 2.4 Objective

An Objective is a goal inside a Scenario.

An Objective contains one or more Conditions.

An Objective is complete when all of its required Conditions are true.

An Objective may have:

- name
- required/optional flag
- completion mode
- callback
- conditions
- saved state

Example block form:

```appeal
Objective "Collect_100_Coins"
    Required true
    CompletionMode "Milestone"
    Condition "Collection", "Coins", 100
    OnComplete "Give_OneUp"
EndObjective
```

Example one-line form:

```appeal
Define_Objective "Collect_100_Coins", true, "Milestone", Condition("Collection", "Coins", 100), "Give_OneUp"
```

Both forms should compile to the same internal representation.

## 2.5 Condition

A Condition is a reusable test that returns true or false.

Examples:

```appeal
Condition "Collection", "Coins", 100
Condition "Defeated", "Boss_01"
Condition "LocationReached", "SecretRoom"
Condition "HasItem", "CastleKey"
Condition "MultipleOf", "XP", 1000
```

A Condition may also have its own callback, though most callbacks should probably live at the Objective or Scenario layer.

---

# 3. Objective Completion Modes

Objectives should support different callback/completion behaviors.

Recommended modes:

| Mode | Meaning |
|---|---|
| `None` | Objective can become true, but no callback fires |
| `Once` | Callback fires one time only |
| `Repeat` | Callback may fire repeatedly when the objective cycles |
| `Milestone` | Callback fires at repeated thresholds, such as 100, 200, 300 coins |

## 3.1 One-time objective

```appeal
Objective "Defeat_First_Boss"
    Required true
    CompletionMode "Once"
    Condition "Defeated", "Boss_01"
    OnComplete "Unlock_Next_World"
EndObjective
```

## 3.2 No-callback objective

```appeal
Objective "Reach_Castle"
    Required true
    CompletionMode "None"
    Condition "LocationReached", "Castle"
EndObjective
```

## 3.3 Milestone objective

```appeal
Objective "Coin_Reward"
    Required false
    CompletionMode "Milestone"
    Condition "Collection", "Coins", 100
    OnComplete "Give_OneUp"
EndObjective
```

This should fire at:

```text
100 coins -> Give_OneUp
200 coins -> Give_OneUp
300 coins -> Give_OneUp
```

Internally this requires threshold tracking:

```text
NextThreshold = 100

If Coins >= NextThreshold:
    Fire callback
    NextThreshold += 100
```

This avoids firing every frame after the player reaches 100 coins.

## 3.4 Objective state fields

Recommended internal state:

```text
IsComplete
CallbackFired
CompletionCount
NextThreshold
LastValue
```

Callbacks should generally fire on state transition, not merely because a condition remains true.

---

# 4. Scenario, Objective, and Condition Loading

A Scenario file may call Objective files.

An Objective file may call Condition definitions.

Example layout:

```text
/Resources/Appeal/Games/BananaQuest/Scenarios/Forest_Trial.scenario.appeal
/Resources/Appeal/Games/BananaQuest/Objectives/Collect_100_Coins.objective.appeal
/Resources/Appeal/Games/BananaQuest/Objectives/Defeat_Forest_Boss.objective.appeal
/Resources/Appeal/Games/BananaQuest/Conditions/Collection.condition.appeal
/Resources/Appeal/Games/BananaQuest/Conditions/Defeated.condition.appeal
```

Loading order:

```text
Load Game Manifest
Load Package Dependencies
Load World
Load Scenario
Load Objectives
Load Conditions
Apply Save/Live State
Start Update Loop
```

Completion flow:

```text
Condition true
    -> Objective true when all required conditions are true
        -> Scenario true when all required objectives are true
```

Optional Objectives:

```text
Optional Objective true
    -> optional callback may fire
    -> Scenario completion is not blocked if this objective remains false
```

---

# 5. Source Form and Function Form

Appeal should support both readable block syntax and compact function syntax where appropriate.

## 5.1 Block syntax

Good for humans, designers, and modders.

```appeal
Objective "Collect_100_Coins"
    Required true
    CompletionMode "Milestone"
    Condition "Collection", "Coins", 100
    OnComplete "Give_OneUp"
EndObjective
```

## 5.2 Function syntax

Good for tools, code generators, AI systems, editors, and compiled output.

```appeal
Define_Objective "Collect_100_Coins", true, "Milestone", Condition("Collection", "Coins", 100), "Give_OneUp"
```

## 5.3 Same internal representation

Both forms should compile to the same internal structure:

```cpp
ObjectiveDefinition
{
    Name = "Collect_100_Coins";
    Required = true;
    CompletionMode = Milestone;
    Conditions = {...};
    Callback = "Give_OneUp";
}
```

Recommended rule:

```text
Blocks are syntactic sugar.
Function calls are the normalized internal form.
```

---

# 6. Source, Expanded Source, Bytecode, and Native Compilation

Appeal should eventually support multiple execution forms.

```text
Source Appeal
    -> Expanded Appeal
        -> Tokenized/Bytecode Appeal
            -> Optional Generated C++
                -> Optional Native Binary
```

## 6.1 Source Appeal

Human-readable `.appeal` files.

Example:

```appeal
Run "Forest.world.appeal"
Run "CoinReward.objective.appeal"
```

## 6.2 Expanded Appeal

The compiler can replace `Run` commands with the contents of the target file, creating one larger expanded script.

This is similar to a C/C++ preprocessor include stage.

```text
main.appeal
    Run Forest.world.appeal
    Run CoinReward.objective.appeal
```

becomes:

```text
expanded.appeal
    [contents of Forest.world.appeal]
    [contents of CoinReward.objective.appeal]
```

## 6.3 Bytecode Appeal

Bytecode is a compact machine-readable representation of commands.

The key design principle:

```text
One command definition = readable name + byte opcode + function pointer
```

The Appeal dictionary should be the single source of truth.

Example C++ structure:

```cpp
struct AppealCommand
{
    std::string name;
    uint8_t byteCode;
    void (*function)(AppealContext&);
};
```

Example registration:

```cpp
RegisterCommand("Print", 0x01, Print_Command);
RegisterCommand("Run",   0x02, Run_Command);
RegisterCommand("World", 0x03, World_Command);
```

Source mode:

```text
lookup by command name
```

Bytecode mode:

```text
lookup by byte opcode
```

Both should call the same underlying C++ function.

## 6.4 Module bytecode

Modules should also have byte IDs.

Recommended model:

```text
[ModuleByte][ModuleCommandByte]
```

Example:

```text
0x10 0x01 = Scenario.Objective
0x10 0x02 = Scenario.Condition
```

Readable equivalent:

```appeal
Scenario Objective ...
Scenario Condition ...
```

This avoids using one global byte for every possible command and lets each module own its own command table.

## 6.5 Mixed source and bytecode projects

Appeal should allow a project to use both source and bytecode files together.

Example:

```appeal
Run "Compiled/core_world.apbc"
Run "Source/debug_tools.appeal"
```

Recommended rule:

```text
A project can mix .appeal and .apbc files.
Each file should be either source or bytecode.
```

This allows developers to compile stable parts while continuing to edit unfinished parts.

## 6.6 Command registration should prevent mismatch

Because commands are registered once with both name and byte, this prevents:

- commands without bytecodes
- bytecodes without readable names
- duplicated definitions
- mismatched behavior
- debugging confusion

A bytecode disassembler can use the dictionary to translate bytecode back into readable command names.

## 6.7 Optional generated C++

A later compiler target may generate C++ from Appeal scripts.

Flow:

```text
Appeal source
    -> Appeal compiler
        -> Generated C++
            -> Platform C++ compiler
                -> Standalone executable
```

This would allow Appeal projects to compile into standalone binaries for the current platform.

Recommended compiler roadmap:

```text
Phase 1: Combine Run files into one expanded script
Phase 2: Convert commands to command IDs / bytecode
Phase 3: Save/load compiled bytecode
Phase 4: Add validation and dependency checking
Phase 5: Generate C++ from Appeal
Phase 6: Build standalone binaries
```

---

# 7. Portal World Object

## 7.1 Concept

A Portal is a special world object used to move characters between worlds, scenes, levels, servers, or time states.

A Portal may be visible or invisible.

Portals can represent:

- scene edges
- doors
- cave entrances
- building interiors
- teleporters
- time portals
- space portals
- dungeon entrances
- multiplayer spawn gates
- DLC/expansion entrances
- server transfer gates
- seamless streaming transitions

## 7.2 Core portal data

Recommended fields:

```text
Name
TargetWorld
TargetPortal
Locked
Visible
CachePreviousWorld
TriggerType
PreloadDistance
ActivationDistance
VisualObject optional
```

Example:

```appeal
Portal "Forest_To_Castle"
    TargetWorld "Castle"
    TargetPortal "Castle_Entrance"
    Locked true
    Visible false
    CachePreviousWorld false
    Trigger "Approach"
    PreloadDistance 50
EndPortal
```

One-line form:

```appeal
Define_Portal "Forest_To_Castle", "Castle", "Castle_Entrance", true, false, false, "Approach", 50
```

## 7.3 Portal defaults

Portals should default to locked.

Recommended defaults:

```text
Locked = true
Visible = false
CachePreviousWorld = false
TargetWorld = optional
TargetPortal = optional
```

This lets level designers place portals before the target level/world exists.

A locked portal should:

```text
not preload the target world
not activate transition
not require a valid target
```

Only unlocked portals need target validation.

## 7.4 Locked behavior

```text
If Locked == true:
    Do not preload target world
    Do not activate transition

If Locked == false:
    Allow preload
    Allow transition
```

This prevents unfinished content from crashing the game and avoids unnecessary background loading.

## 7.5 Visible behavior

Visible controls whether the portal has a visual representation.

Examples:

```text
Visible false = invisible level-edge trigger
Visible true  = door, glowing portal, gate, cave entrance, etc.
```

The Scenario system may toggle portal visibility.

## 7.6 CachePreviousWorld

CachePreviousWorld controls what happens after transition.

| Value | Behavior |
|---|---|
| `true` | Keep the previous world loaded in memory |
| `false` | Free/unload the previous world after transition |

Examples:

```text
Outdoor area -> indoor shop: cache previous world
Large dungeon -> distant region: free previous world
Open-world streaming: cache nearby worlds
Server transfer: free previous server/world state locally
```

The active Scenario should be allowed to toggle this flag.

## 7.7 Scenario control of portals

Portals should be mostly data-driven. Scenarios can change their state.

Example commands:

```appeal
SetPortalLocked "CastleDoor", false
SetPortalVisible "CastleDoor", true
SetPortalCachePreviousWorld "CastleDoor", true
SetPortalTargetWorld "CastleDoor", "CastleInterior"
SetPortalTargetPortal "CastleDoor", "CastleEntrance"
```

Example Objective callback:

```appeal
Function "Unlock_CastleDoor"
    SetPortalLocked "CastleDoor", false
    SetPortalVisible "CastleDoor", true
EndFunction
```

## 7.8 DLC and unfinished content

Because portals default to locked, they can be used as placeholders for future content.

Example:

```appeal
Portal "Future_DLC_Gate"
    Locked true
    Visible false
EndPortal
```

Later, after the DLC exists:

```appeal
SetPortalTargetWorld "Future_DLC_Gate", "NorthernKingdom"
SetPortalTargetPortal "Future_DLC_Gate", "SouthernEntrance"
SetPortalLocked "Future_DLC_Gate", false
SetPortalVisible "Future_DLC_Gate", true
```

## 7.9 Online game usage

Portals can also support online infrastructure:

- moving players between servers
- load balancing
- maintenance migration
- expansion zones
- private instances
- dungeon matchmaking
- event worlds

Example:

```appeal
SetPortalTargetServer "ServerTransferGate", "US-West-04"
SetPortalTargetWorld "ServerTransferGate", "CapitalCity"
SetPortalTargetPortal "ServerArrival"
SetPortalLocked "ServerTransferGate", false
```

## 7.10 Portal design principle

```text
Portal stores transition data.
Scenario changes portal state.
World loader obeys portal state.
```

---

# 8. Save and Live State

Save files or live state databases should store changes made after the source content is loaded.

Examples:

```text
Player position
Inventory
Health
XP
Coins
Quest/scenario progress
Objective progress
Condition state
Portal locked/unlocked state
Portal visible state
Portal cache behavior
Opened chests
Defeated enemies
Unlocked worlds
Online player sessions
```

Loading sequence:

```text
Load source world/scenario/objective definitions
Apply save state or live server state
Start gameplay/update loop
```

Example save data:

```text
PortalID: CastleDoor
Locked: false
Visible: true
CachePreviousWorld: true

ScenarioID: Forest_Trial
Complete: false

ObjectiveID: Coin_Reward
CompletionCount: 3
NextThreshold: 400
```

---

# 9. Summary of Final Architecture

## 9.1 Content ownership

```text
Game folder = private game content
Package folder = reusable shared content
Database = index and mutable state
Files = source assets and definitions
```

## 9.2 Gameplay logic hierarchy

```text
World
  -> Scenario(scope)
       -> Objective(required/optional, completion mode)
            -> Condition
```

## 9.3 Completion rules

```text
All required Conditions true -> Objective complete
All required Objectives complete -> Scenario complete
Optional Objectives can trigger callbacks but do not block Scenario completion
```

## 9.4 Script formats

```text
.appeal = human-readable source
.apbc   = compiled Appeal bytecode
.apack  = optional packaged Appeal project
```

## 9.5 Compiler roadmap

```text
Source Appeal
Expanded Appeal
Bytecode Appeal
Generated C++
Native Binary
```

## 9.6 Portal rules

```text
Portals default to locked.
Locked portals do not preload, activate, or require valid targets.
Unlocked portals can preload and transfer characters.
Scenarios can change portal locked, visible, target, and cache behavior.
```

---

# 10. Runtime, Modules, Plugins, and Tools

## 10.1 Appeal, Bananas, and specialized apps

Appeal is the general-purpose language and runtime.

Bananas is the first major specialized Appeal app. It focuses on game development first because games use many parts of the computer at once: graphics, audio, input, storage, networking, packaging, world state, physics, AI, and editor tooling.

Recommended relationship:

```text
Appeal = universal language and runtime model
Bananas = game-focused Appeal distribution
Future Browser = web/content-focused Appeal distribution
Future Tools = specialized Appeal apps built on the same language
```

Bananas may include game-focused default scripts, modules, plugins, editor tools, and package repositories, while still preserving Appeal script compatibility.

## 10.2 Modules and plugins

Modules define the platform-independent Appeal API.

Plugins implement that API for a platform, library, or backend.

Recommended rule:

```text
Appeal scripts call modules.
Modules expose stable commands.
Plugins provide platform-specific implementations.
```

This lets the same Appeal script run on different systems while allowing each system to use optimized plugins.

Example:

```text
Graphics module = stable Appeal API
SDL2/OpenGL Graphics plugin = cross-platform default implementation
Future Vulkan/DirectX/Metal plugins = optimized implementations
```

Plugins should be named so that a user can identify both Appeal ownership and the backend library.

Example convention:

```text
Appeal_SDL2_Graphics.dll
Appeal_SDL2_Audio.dll
Appeal_SQLite_Storage.dll
```

## 10.3 Default and test plugins

Every module should have a test script and a test plugin.

Recommended convention:

```text
Resources/Appeal/Test/Test_ModuleName.Appeal
Plugins/ModuleName/Test_ModuleName
```

The test plugin should export every function listed by the module, but each function may initially only print its function name. This gives the module a working placeholder, verifies the API surface, and gives future plugin authors a template.

SDL-based plugins are useful default plugins because SDL is cross-platform and can help Appeal/Bananas work out of the box before more specialized plugins exist.

## 10.4 Loading and unloading

`Config.Appeal` is the standard startup configuration script. It should load the default modules and plugins for the current app or profile.

Modules and plugins may still be loaded and unloaded by other scripts so that uncommon systems can be used briefly and then freed.

Recommended startup flow:

```text
Appeal executable starts
    -> Main.Appeal by default
        -> Setup/Config.Appeal
            -> setup scripts
            -> module/plugin loading
        -> app/game/editor state scripts
```

The executable may also accept a startup script argument for testing or direct game launch.

## 10.5 Script organization

Appeal scripts should be organized by purpose so that humans and tools can edit them safely.

Recommended folders:

```text
/Resources/Appeal
  Main.Appeal
  /Setup
    Config.Appeal
    Graphics_Setup.Appeal
    Audio_Setup.Appeal
  /GUI
    Graphics_GUI.Appeal
    GUI_Control_Demo.Appeal
  /Test
    Test_Graphics.Appeal
    Test_Input.Appeal
  /Bananas
    Editor.Appeal
    Splash.Appeal
    /Editor_Tabs
      Game.Appeal
      Level.Appeal
      Audio.Appeal
```

For module default settings, use this naming convention:

```text
Default_ModuleName_Config.Appeal
```

Example:

```text
Default_Graphics_Config.Appeal
Default_Audio_Config.Appeal
```

## 10.6 Repositories and device profiles

Appeal should support repository-based installation for modules, plugins, packages, assets, and updates.

Different devices or operating systems may use different repositories instead of requiring every client to report detailed device information.

Recommended approach:

```text
Linux desktop repo
Windows desktop repo
Steam Deck handheld repo
Steam Deck docked repo
Switch-like handheld repo
Low-resource mobile repo
High-resource desktop repo
```

A device or distribution can select the correct repo at install time. Some systems may have more than one repo profile and switch between them, such as handheld and docked modes.

This keeps the script universal while letting each repo provide suitable plugins, assets, graphics settings, and performance defaults.

## 10.7 Security and permissions

Remote scripts should not be able to replace modules or plugins directly.

Security should live primarily in trusted plugins, modules, package systems, and permissions rather than in untrusted site or game scripts.

Recommended rule:

```text
Scripts request capabilities.
Trusted modules/plugins enforce permissions.
Users or distributors control permission grants.
```

Appeal should eventually support app-style permissions similar to mobile platforms:

```text
Network access
File access
Microphone access
Camera access
Controller input
User data access
Package installation
Plugin installation
```

Permissions should be revocable by the user or platform manager.

The Network module may begin in an insecure debug mode for easier testing, but the long-term design should use encryption and key-based authentication. This may work similarly to SSH concepts without necessarily being a literal SSH connection.

## 10.8 Script variables and validation

Appeal script variables are stored as strings so that modules and plugins can parse, validate, clamp, or reject values according to the function being called.

Validation should happen at the module/plugin boundary.

Possible validation responses:

```text
Accept = value is valid
Clamp = value is outside range but safely adjustable
Reject = value is invalid and should not be used
Reject with counter = repeated invalid values trigger error/shutdown
```

Clamping is useful when it prevents crashes and produces predictable behavior. Rejection is better when the value is far enough outside expectations that continuing could hide a bug or security problem.

Performance matters: validation rules should be efficient because game commands may run every frame.

## 10.9 GUI and editor design

ImGui is part of the Graphics module for now. This keeps the system simpler while Bananas is still evolving.

ImGui should be used mainly for apps, tools, debugging, settings, and development workflows. In-game menus can later use custom game GUI systems.

Editor GUIs should be script-driven. Appeal scripts should be able to add tabs, controls, file pickers, and command callbacks without requiring C++ edits.

Recommended editor model:

```text
Main editor window
    -> dockable ImGui interface
    -> tabs added by Appeal scripts
    -> optional second SDL window for live level preview
```

GUI controls should support callback names so script authors can connect controls to Appeal functions or module commands.

## 10.10 Files and file selection

The Files module should provide cross-platform directory scanning, file reading, file writing, and path helpers.

File picker GUI controls should use a root directory, a start directory, and a remembered current directory.

Recommended behavior:

```text
Root = highest allowed folder
Start = initial folder for this picker
Current = folder remembered after navigation
Single click = select
Double click = open folder/file
.. = go up one folder within root
```

File type detection should eventually inspect file contents where possible rather than trusting file extensions alone. Extensions are useful for convention and filtering, but content-based checks are better for security.

## 10.11 Game and level editor conventions

Game-specific files should live under the game's folder.

Recommended structure:

```text
/Resources/Appeal/Games/GameName
  GameName.Appeal
  /Levels
    /LevelName
      LevelName.Appeal
      LevelName.Playlist
```

The editor should let the user select or create games and levels through GUI tools. Selecting a game should update level paths. Selecting a level should make it the active level and allow the editor to preview or run it.

Music playlists should be level-specific by convention, using the same base name as the level and a playlist extension.

## 10.12 Scenario triggers and timers

Scenarios may monitor values and trigger callbacks when conditions become true.

Triggers may be based on:

```text
value comparison
threshold crossing
milestone count
real time
frame count
```

Frame-based timing is useful for deterministic games when the frame limiter is active. Real-time timing is useful when behavior should continue consistently even if the frame rate changes or the game is paused.

Recommended rule:

```text
Use frame triggers for deterministic gameplay simulation.
Use time triggers for real-world timers, networking, cooldowns, and UI behavior.
```

## 10.13 Formatting conventions

Appeal scripts should use Red Whistle Productions formatting.

Opening braces go on their own line. Code inside braces is tab-indented.

Example:

```appeal
If Var Main_Open == True
{
	Graphics Update
	Input Update Default
	Graphics Render
}
```

Use the same brace style for C and C++ code in this project.

Boolean values should be readable and flexible where possible:

```text
True, true, On, on, 1 = true
False, false, Off, off, 0 = false
```

---

# 11. Implementation Notes for Codex/Copilot

When implementing this system, prefer small, separable interfaces:

```cpp
ICommandRegistry
IScriptLoader
ISourceParser
IBytecodeReader
IBytecodeWriter
IScenarioManager
IObjective
ICondition
IPortal
IWorldLoader
ISaveStateProvider
IAssetRegistry
```

The command registry should be the single source of truth for:

```text
command name
command byte opcode
module byte opcode
function pointer
argument format
help/documentation string
```

Source and bytecode execution should call the same command functions.

Block syntax and function syntax should produce the same internal representation.

Portals should be data-driven world objects, not custom hardcoded transitions.

Scenarios should modify portal data rather than requiring custom code for every door, gate, level edge, DLC entrance, or server transfer.


