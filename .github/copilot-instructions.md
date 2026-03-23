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

### Memory Management

Manual `new`/`delete` throughout. DirectX COM objects released via `SAFE_RELEASE()` macro. No smart pointers or RAII. Each scene's `End()` method is responsible for releasing its own resources.

### Naming Style

- Classes/methods: PascalCase (`MapPiece`, `Sprite_Transform_Draw`)
- Variables: mixed camelCase (`mousePoint`, `bulletlisthead`)
- Globals/namespaces: PascalCase (`Global::Window`, `Resource::Texture`)
- DirectX types use Win32 Hungarian notation (`LPDIRECT3DTEXTURE9`, `HWND`)
