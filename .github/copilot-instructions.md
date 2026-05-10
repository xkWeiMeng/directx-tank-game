# Copilot Instructions — DirectX Tank Game

## Build

- **IDE**: Visual Studio 2022 (v143 toolset) or 2017+ (v141 toolset, Windows SDK 10.0.15063.0)
- **Solution**: `TankGame.sln` → single project `TankGame/TankGame.vcxproj`
- **Configurations**: Debug/Release × Win32/x64
- **DirectX SDK**: June 2010 (`C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)`)
- **Character set**: MultiByte (required for Chinese filename support)
- **Output**: `Tank.exe`

Build from Visual Studio or:
```
msbuild TankGame.sln /p:Configuration=Debug /p:Platform=Win32
```

If using VS 2022 with v143 toolset (vcxproj may reference v145 which doesn't exist):
```
msbuild TankGame.sln /p:Configuration=Debug /p:Platform=x64 /p:PlatformToolset=v143
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

`Sprite_Transform_Draw(texture, x, y, width, height, frame, columns, rotation, scaling, color)` — the `color` parameter is a tint filter (white = no tint), not a color key. Color keys are set at `LoadTexture()` time. See [Resource Inventory](#resource-inventory) for all sprite dimensions, frame counts, and color keys.

Render order: scene content → debug info → cursor (topmost).

### Window / 4K Display

`ScreenWidth`/`ScreenHeight` are always set to `BaseWidth`/`BaseHeight` (1024×960). The D3D backbuffer uses this fixed resolution and stretches to fill the window client area. Window physical size is calculated from screen metrics. This ensures correct rendering on all DPI settings.

### Game Objects (GamingScene.cpp)

Game objects (`Player`, `Enemy`, `Bullet`, `BoomFire`, `MapPiece`, `AwardItem`) use **manual linked lists** — not STL containers. Each list has a `*ListHead` struct. Dead objects go into `UselessObjHead` for batch cleanup.

Heavy game state lives in the `GS` namespace inside `GamingScene.cpp` (not as class members). Key state variables:
- `FreezeEndTime` / `FortifyEndTime` — power-up timers (GetTickCount-based)
- `FlagGameX` / `FlagGameY` — base (eagle) position in game area, grid (6,12) = pixel (448, 832)
- `BaseDestroyed` — flag for destroyed eagle rendering
- `NowLevel` — current stage number

### Player Grade System

Players have a `Grade` (0–3) that affects stats (speed, bullet speed, power level). Grade 0 is the starting level. Picking up a star award calls `LevelUp()` → `ApplyGradeStats()`. At Grade 3, bullets can destroy steel walls (`PowerLevel >= 3`).

### Power-Up System (AwardItem)

Six award types (frames in `奖励.bmp`, 6 columns of 30×28):

| Frame | Type | Effect |
|-------|------|--------|
| 0 | Helmet | Invincibility for 10 seconds (Shield texture overlay) |
| 1 | Timer | Freeze all enemies for 10 seconds |
| 2 | Shovel | Fortify base for 20 seconds (base becomes invulnerable) |
| 3 | Grenade | Destroy all enemies on screen |
| 4 | Star | Level up player (Grade+1, max 3) |
| 5 | Extra Life | +1 life |

Awards spawn when a "flash enemy" (`IsFlashEnemy = true`) is destroyed. Invincibility uses `InvincibleEndTime` (GetTickCount-based) checked in `Player::GetHurt()`.

### Map System

Maps are 13×13 grids stored as binary `.map` files (169 bytes) in `Map/`. The `DesignMapScene` provides a visual editor. Maps load via `ReadMapInHD()`.

Key map values: 0=empty, 1-13=brick configs, 14-26=steel configs, 27=grass, 28=water, 29=ice, 31=P1 spawn, 32=P2 spawn, 33=enemy spawn.

The base (eagle) is NOT stored in map files — it's rendered separately at a fixed position using the Tile texture.

### Tile Texture Layout (砖.bmp)

The tile spritesheet has **7 columns of 32×32** tiles. Rendering uses `Sprite_Transform_Draw()` with frame index or `MapPiece::Draw()` with source RECTs.

| Index (0-based) | X offset | Content |
|-----------------|----------|---------|
| 0 | 0 | Brick |
| 1 | 32 | Steel |
| 2 | 64 | Grass/Trees |
| 3 | 96 | Water frame 1 |
| 4 | 128 | Water frame 2 |
| 5 | 160 | Eagle (normal) |
| 6 | 192 | Eagle (destroyed) |

Water tiles animate by alternating frames 3 and 4 in `MapPiece::Draw()`. Brick sub-tiles use 16×16 pieces (14-column addressing at half-tile resolution).

### MapPiece Rendering

Tiles are rendered via `MapPiece::Draw()` (the linked list walk in `Render()`), **not** via `DrawMap()` which is commented out. `MapPiece::Draw()` routes rendering by `rect->left` ranges:
- `<32`: brick | `<64`: steel | `<96`: grass | `<128`: water (animated) | `<160`: ice | `<192`: eagle | `>=192`: destroyed eagle

### Wall Collision (BeingCrash)

`BeingCrash()` determines wall type by `rect->left`: `<32` = brick (always destroyable), `32-63` = steel (only destroyable if `powerLevel >= 3`), `>=64` = non-destructible (grass, water, ice). The `powerLevel` parameter flows from `Bullet::Logic()` → `Crash()` → `BeingCrash()`.

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

## Resource Inventory

All resource paths are centralized in `Resource.h` (`Resource::*` namespaces). Some files are loaded dynamically with hardcoded paths. When adding/modifying resources, update both the file on disk and the corresponding `Resource.h` entry.

### Texture Spritesheets (`Resources/Texture/`)

All textures load via `LoadTexture()` with a color key for transparency. The color key removes a specific color from the image (makes it transparent); it is NOT the same as the tint parameter in `Sprite_Transform_Draw()`.

| File | Const | Frame W×H | Cols | Total Frames | Color Key | Notes |
|------|-------|-----------|------|-------------|-----------|-------|
| `玩家一.bmp` | `Player_1` | 28×28 | 8 | 32 | `(0,0,0)` black | 224×112 sheet. Frame = `Dir*8 + Grade*2 + (0\|1)` |
| `玩家二.bmp` | `Player_2` | 28×28 | 8 | 32 | `(0,0,0)` black | Same layout as Player 1 |
| `敌人.bmp` | `Enemy` | 28×28 | 8 | 32 | `(4,4,4)` near-black | Same layout as Players |
| `砖.bmp` | `Tile` | 32×32 | 7 | 7 | `(4,4,4)` near-black | See [Tile Texture Layout](#tile-texture-layout-砖bmp). Sub-tiles use 16×16 |
| `子弹.bmp` | `Bullet` | 8×8 | 4 | 4 | `(4,4,4)` near-black | One frame per direction |
| `爆炸一.bmp` | `Boom1` | 28×28 | 1 | 1 | `(0,0,0)` black | Small explosion |
| `爆炸二.bmp` | `Boom2` | 64×64 | 1 | 1 | `(4,4,4)` near-black | Large explosion |
| `奖励.bmp` | `Award` | 30×28 | 6 | 6 | `(234,234,234)` light gray | 180×28 sheet. See [Power-Up System](#power-up-system-awarditem) |
| `盾牌.bmp` | `Shield` | 32×32 | 1 | 2 | `(4,4,4)` near-black | 32×64 sheet. Invincibility overlay animation |
| `游戏结束.bmp` | `GameOver` | 248×160 | 1 | 1 | `(0,0,0)` black | Rendered at 2× scale |
| `旗子.bmp` | `Flag` | 32×32 | 1 | 1 | `(255,255,255)` white | Sidebar flag icon |
| `数字.bmp` | `Number` | 14×14 | 10 | 10 | `(255,255,255)` white | Digits 0-9 for HUD |
| `杂项.bmp` | `Something` | — | — | — | `(255,255,255)` white | Loaded but never drawn (dead code) |
| `孔.bmp` | `Hole` | — | — | — | `(4,4,4)` near-black | Loaded but never drawn (dead code) |
| `0.png`–`8.png` | *(hardcoded)* | 800×600 | 1 | 9 total | `(0,0,0)` black | Flicker/transition animation. Loaded dynamically in `GamingScene::Init()` |
| `GameSetting.png` | `GameSetting` | — | — | — | — | **Declared in Resource.h but never loaded** |

Unreferenced files on disk (orphans): `面包.png`, `wxz.png`

### About Scene Textures (`Resources/About/`)

| File | Const | Color Key | Notes |
|------|-------|-----------|-------|
| `天空背景.JPG` | `SkyBG` | none | Full-screen sky background |
| `山景-远景.PNG` | `MountainFar` | `(255,255,255)` white | Parallax distant mountains |
| `前景-树草.png` | `ForegroundGrass` | `(255,255,255)` white | Foreground trees/grass layer |
| `前景-地面.png` | `ForegroundGround` | `(255,255,255)` white | Foreground ground layer |
| `云朵精灵表.PNG` | `CloudSheet` | `(255,255,255)` white | 384×320, 4 cols × 2 rows spritesheet |
| `飞艇精灵表.PNG` | `AirshipSheet` | `(255,255,255)` white | 896×400, 4 cols × 2 rows spritesheet |
| `树叶精灵表.png` | `LeafSheet` | `(255,255,255)` white | 128×128, 4 cols × 2 rows spritesheet |

Legacy / unused (declared in `Resource.h` but never loaded): `作者.jpg`, `山景.bmp`, `云1.png`, `云2.png`, `云3.png`, `飞艇.tga`

Unreferenced files on disk (orphans): `前景-树草9.png`, `山景.PNG`

### Home / Cursor / Sound

| Directory | File | Const | Status |
|-----------|------|-------|--------|
| `Home/` | `UI.bmp` | `Home::Backgroud` | **Declared but never loaded** |
| `Cursor/` | `光标.png` | `Cursor::Normal` | Active — custom cursor |

Unreferenced files: `Home/1.bmp`, `Home/Background.jpg`, `Cursor/Normal.png`

### Sound Files (`Resources/Sound/`)

| File | Const | Used for |
|------|-------|----------|
| `开始1.wav` | `Start` | Game start SFX |
| `坦克移动.wav` | `Moving` | Tank move loop |
| `坦克停止移动.wav` | `Stop` | Tank stop SFX |
| `BGM.wav` | `BGM` | Background music |
| `爆炸.wav` | `PlayerBoom` | Explosion SFX |

**Known issue**: `Resource::Sound_Rescource::BGM` references `"bgm.wav"` (lowercase) but the file on disk is `BGM.wav` — works on case-insensitive NTFS but would break on case-sensitive filesystems.

**Missing**: `游戏结束.aif` is declared in Resource.h but does not exist on disk (loading code is commented out).

### Map Files (`Map/`)

Maps are 169-byte binary files (13×13 grid). Loaded dynamically by name — no `Resource.h` entries.

| Pattern | Loader | Notes |
|---------|--------|-------|
| `Map/stage{N}.map` | `StageSelectScene` scans `stage1..stageN` sequentially | Currently only `stage1.map`, `stage2.map`, `stage7.map` exist |
| `Map/custom/*.map` | `StageSelectScene` via `FindFirstFileA` glob | User-created maps from DesignMapScene |
| `Map/{name}.map` | `GamingScene` loads by exact name | |

**Missing stages**: `stage3.map`–`stage6.map` don't exist (only `*_backup.map` variants). The stage scanner stops at the first gap, so `stage7.map` is unreachable unless `stage3`–`stage6` are restored from backups.

Backup/test files on disk: `stage1_backup.map`–`stage6_backup.map`, `test.map`, `test.txt`, `xczxc.map`, `xk1.map`

### Settings File

`GameSet.set` — 11 bytes of binary (Player1 keybindings[5] + Player2 keybindings[5] + SoundSwitch). Read in `Game_Init()`, written in `GameSettingScene`.

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

### Add a new power-up

1. Add a frame to `奖励.bmp` (append column) or reuse an existing type index
2. Add a `case` in `CheckAwardCollision()` in `GamingScene.cpp` to define the effect
3. If the effect is time-based, add an `int XxxEndTime = 0` variable in the `GS` namespace
4. Check the timer in the relevant game loop section (`Update()`, `Crash()`, etc.)
5. Reset the timer in `NewStage()` and `RestartThisStage()`

### Add a new scene

1. Inherit `Scene`, implement `Init()`, `End()`, `Update()`, `Render()`
2. Add a `GAME_STATE` enum value in `Global.h`
3. Add a `case` in `Game_ChangeScene()` in `GameMain.cpp`
