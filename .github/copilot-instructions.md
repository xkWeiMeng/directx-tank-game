# Copilot Instructions — DirectX Tank Game

## Build

- **IDE**: Visual Studio 2017+ (v141 toolset, Windows SDK 10.0.15063.0)
- **Solution**: `TankGame.sln` → single project `TankGame/TankGame.vcxproj`
- **Configurations**: Debug/Release × Win32/x64
- **DirectX SDK**: June 2010 (`C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)`)
- **Character set**: MultiByte (required for Chinese filename support)
- **Output**: `Tank.exe`

Build from Visual Studio or:
```
msbuild TankGame.sln /p:Configuration=Debug /p:Platform=Win32
```

## Architecture

### Game Loop (WinMain.cpp → GameMain.cpp)

```
WinMain
├─ Game_Init()           → Direct3D, DirectInput, DirectSound, load settings
├─ Main loop (while !Gameover)
│  ├─ PeekMessage()      → non-blocking Windows messages
│  ├─ Game_Update()      → DirectInput poll → cursor update → scene->Update()
│  ├─ Game_Render()      → Clear → BeginScene → scene->Render() → debug overlay → cursor → Present
│  └─ Frame throttle     → 480 FPS logical cap via timeGetTime()
└─ Game_Free()
```

### Scene System

All screens inherit from the `Scene` base class (`Scene.h`) with virtual `Init()`, `End()`, `Update()`, `Render()`. Scene transitions go through `Game_ChangeScene(GAME_STATE)` in `GameMain.cpp`, which deletes the old scene and creates the new one.

| GAME_STATE enum | Scene class | Purpose |
|---|---|---|
| `Home` (1) | HomeScene | Main menu (5 options) |
| `SinglePlayer` (2) | GamingScene | Gameplay (single) |
| `DoublePlayer` (3) | GamingScene | Gameplay (two-player) |
| `DesignMap` (4) | DesignMapScene | Visual map editor |
| `About` (5) | AboutScene | Credits |
| `GameSatting` (6) | GameSettingScene | Key binding config |

To add a new scene: inherit `Scene`, implement the 4 virtual methods, add a `GAME_STATE` enum value in `Global.h`, and add a case in `Game_ChangeScene()`.

### Rendering (DirectX.h/cpp)

DirectX 9.0c with D3DXSprite for 2D rendering. Key functions:
- `LoadSurface()` / `DrawSurface()` — full-screen backgrounds (StretchRect)
- `LoadTexture()` / `Sprite_Transform_Draw()` — animated sprites with spritesheet support (frame, columns, rotation, scale)
- `MakeFont()` / `FontPrint()` — text rendering
- `Collision()` (AABB) / `CollisionD()` (distance-based) — collision detection

Render order: scene content → debug info → cursor (topmost).

### Game Objects (GamingScene.cpp)

Game objects (`Player`, `Enemy`, `Bullet`, `BoomFire`, `MapPiece`) use **manual linked lists** — not STL containers. Each list has a `*ListHead` struct. Dead objects go into `UselessObjHead` for batch cleanup.

### Map System

Maps are 13×13 grids stored as binary `.map` files (169 bytes) in `Map/`. Tile values encode walls, spawn points, etc. The `DesignMapScene` provides a visual editor. Maps load via `ReadMapInHD()`.

### Audio (DirectSound.h/cpp, Sound.h/cpp)

DirectSound wrapper (`CSoundManager` / `CSound` / `CWaveFile`). WAV-only. Sound objects declared in `Sound.h`, paths in `Resource.h` under `Resource::Sound_Rescource`. Global toggle: `Global::Sound::SoundSwicth`.

### Input (DirectX.h/cpp)

DirectInput 8 — keyboard (non-exclusive) + mouse (exclusive, custom cursor replaces system cursor). Query via `Key_Down()`, `Key_Up()`, `Mouse_Button()`, `Mouse_X()`/`Mouse_Y()`. Player keybindings stored in `Global::PlayerControl::Player1[5]`/`Player2[5]` (up/down/left/right/shoot), persisted to `GameSet.set` as 11 bytes of binary.

## Conventions

### Global State

All shared state lives in nested namespaces in `Global.h` — access as `Global::Window::ScreenWidth`, `Global::PlayerControl::Player1`, `Global::Sound::SoundSwicth`, etc. Resource paths are centralized in `Resource.h` under `Resource::*` namespaces.

### Language

Comments are ~70% Chinese (Simplified), ~30% English. Resource filenames use Chinese characters (e.g., `坦克移动.wav`, `光标.png`). UI strings are Chinese. The codebase uses MultiByte character set — **do not switch to Unicode**.

### Known Typos in Code (do not "fix" — they are used as identifiers)

- `GameSatting` (enum value, should be GameSetting)
- `SoundSwicth` (should be SoundSwitch)
- `Backgroud` (should be Background)
- `Sound_Rescource` (should be Sound_Resource)
- `Dirction` enum (should be Direction): values `up`, `right`, `below` (= down), `lift` (= left)

### `Now_GAME_STATE` vs `GAME_STATE`

`GAME_STATE` is the scene enum (Home, SinglePlayer, etc.). `Now_GAME_STATE` is an unrelated `int` flag: 0 = "Home screen, ESC quits", 1 = "in a sub-scene". Do not confuse them.

### Circular Include

`DirectX.h` ↔ `Global.h` have a circular include (safe via `#pragma once`). Do not add type dependencies between them.

### Scene-local State Pattern

Heavy scene state goes in a named namespace in the `.cpp` file (e.g., `namespace DMS` in `DesignMapScene.cpp`), not as class members.

### Memory Management

Manual `new`/`delete` throughout. DirectX COM objects released via `SAFE_RELEASE()` macro. No smart pointers or RAII. Each scene's `End()` method is responsible for releasing its own resources.

### Naming Style

- Classes/methods: PascalCase (`MapPiece`, `Sprite_Transform_Draw`)
- Variables: mixed camelCase (`mousePoint`, `bulletlisthead`)
- Globals/namespaces: PascalCase (`Global::Window`, `Resource::Texture`)
- DirectX types use Win32 Hungarian notation (`LPDIRECT3DTEXTURE9`, `HWND`)

## How-To Patterns

### Add a new sound

1. Place `.wav` file in `Resources/Sound/`
2. Add `char* const` entry in `Resource::Sound_Rescource` (Resource.h)
3. Add `extern CSound*` in Sound.h, define in Sound.cpp
4. Add `LoadSound()` call in `Sound::Sound_Init()` (Sound.cpp)
5. Play with `PlaySound()` or `LoopSound()`, stop with `StopSound()`

### Add a new texture

1. Place image in `Resources/Texture/`
2. Add `char* const` entry in `Resource::Texture` (Resource.h)
3. Load via `LoadTexture()` in the scene's `Init()`, release in `End()`
4. Draw with `Sprite_Transform_Draw()` — use `D3DCOLOR_XRGB(r,g,b)` color key for transparency (default: black)

### Add a new GUI element

1. Create new header with a namespace (e.g., `namespace GUI::MyWidget`)
2. Provide `Init()`, `Update()`, `Render()` functions
3. `#include` it from `GUIs.h`
